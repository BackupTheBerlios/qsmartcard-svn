/*
 *  Copyright (C) 2006 Frank Büttner frank-buettner@gmx.net
 * 
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *  
 */
#include <QtXml>
#include "GSM.h"
#include "ZeichensatzWandlung.h"
#include <EFAntwort.h>
#include <MF_DFAntwort.h>
#include <Lesegeraet.h>

//Zum übersetzten wird min. Version 0.2.0 des Lesermodells benötigt.
#if LesegeraetAPI_Version < 0x000200
#error Es wird min. Version 0.2.0 des Lesermodells benötigt.
#endif

QFrankGSMKarte::QFrankGSMKarte(QObject* eltern):QFrankSmartCard(eltern)
{
	//Warnung bei DEBUG
#ifndef QT_NO_DEBUG
	qWarning("WARNUNG Debugversion wird benutzt.\r\nEs könnten sicherheitsrelevante Daten ausgegeben werden!!!!!");
#endif
	setObjectName("QFrankGSMKarte");
	K_PIN1gesetzt=false;
	K_PIN2gesetzt=false;
	K_KarteAkiviert=false;
	K_PIN1korrektEingegeben=false;
	K_SichereEingabeNutzen=true;
	K_Leser=0;
	K_Seriennummer="";
	K_MF_DFAntwort=new QFrankGSMKarteMF_DFAntwort(this);
	K_EFAntwort=new QFrankGSMKarteEFAntwort(this);
	K_Zeichenwandlung=new QFrankZeichensatzWandlung(this);
}

bool QFrankGSMKarte::KarteAktivieren()
{
	if(K_Leser==0)
	{
		K_Fehlertext=tr("Keine Verbindung zum Kartenleser");
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte: keine Verbindung zum Leser";
#endif
		return false;
	}
	//Testen ob der Leser API =>0.3.0 ist.
	if(K_Leser->Version()<0x000300)
	{
#ifndef QT_NO_DEBUG
		qDebug("QFrankGSMKarte: Lesemodul zu alt statt 0x000300 0x%X",K_Leser->Version());
#endif
		K_Fehlertext=trUtf8("Kartenlesermodul ist zu alt. Es wird min. Version 0.3.0 benötigt");
		return false;
	}
	QByteArray ATR;
	if(K_Leser->KarteAnfordern(ATR)!=QFrankLesegeraet::CommandSuccessfulAsynchron)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte Karte aktivieren: keine/falsche Karte im Leser";
		qDebug()<<K_FeldNachHex(ATR);
#endif
		K_Fehlertext=trUtf8("Keine oder ungültige Karte eingelegt");
		return false;	
	}
	K_KarteAkiviert=true;
	/* haben wir eine SIM Karte???
	   Jede Karte hat eine Seriennummer(10 Bytes)	   
	*/
	if (!K_SeriennummerErmitteln())
	{
		K_KarteAkiviert=false;
		return false;
	}
	else
		return true;
}

bool QFrankGSMKarte::K_SeriennummerErmitteln()
{

	//  EF für die Seriennummer der Karte ist: 2fe2 unter dem MF 0x3f00
	if (!K_SelectFile(0x3f00))
		return false;
	K_GetResponse(QFrankGSMKarte::MF_DF,(uchar)(K_Antwortkode&0x00ff) );
	//Datei lesen
	if (!K_SelectFile(0x2fe2))
		return false;
	K_GetResponse(QFrankGSMKarte::EF,(uchar)(K_Antwortkode&0x00ff) );
	//Datei Lesen
	if (!K_ReadBinary((uchar)K_EFAntwort->Dateigroesse()))
		return false;
	//Auswerten
	K_Seriennummer="";
	for (uchar Stelle=0;Stelle<(uchar)K_EFAntwort->Dateigroesse();Stelle++)
	{
		K_Seriennummer=K_Seriennummer+QString("%1").arg(((uchar)K_Kartenantwort.at(Stelle)) & 0x0f);
		K_Seriennummer=K_Seriennummer+QString("%1").arg((((uchar)K_Kartenantwort.at(Stelle)) & 0xf0)>>4);
	}
#ifndef QT_NO_DEBUG
	qDebug()<<QString("QFrankGSMKarte Seriennummer ermitteln Ergebnis: %1").arg(K_Seriennummer);
#endif
	return true;
}

void QFrankGSMKarte::PinUebertragen(const QByteArray &pinfeld)
{
	K_Pinspeicher=pinfeld;
#ifndef QT_NO_DEBUG
	qDebug()<<QString("QFrankGSMKarte Pin übertragen: %1").arg(K_FeldNachHex(K_Pinspeicher));
#endif
}

const bool QFrankGSMKarte::K_PinEingabe(const uchar &Pinnummer)
{
	if (!K_VerbindungZurKarte())
		return false;
	/*
		Verify CHV
		Klasse=0xA0
		INS=0x20
		P1=0x00
		P2=Nummer der PIN
		Länge Daten=0x08
		Daten=PIN immer 8 Bytes lang. Der Rest wird mit FF gefüllt
		Kodierung ASCII Bit 8=0
	*/

	//sichere oder unsichere Eingabe
	if(K_SichereEingabeNutzen)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte Pin sichere Eingabe";
#endif
		K_Kartenbefehl.resize(17);
		K_Kartenbefehl[0]=0x52;//TAG 
		K_Kartenbefehl[1]=0x0f;//Länge des Tags
		K_Kartenbefehl[2]=0x01;//Länge der Pin unbekannt Format ASCII bit8=0
		K_Kartenbefehl[3]=0x06;//Wo soll die PIN beginnen
		K_Kartenbefehl[4]=0xa0;//Klasse
		K_Kartenbefehl[5]=0x20;//Kartenbefehl
		K_Kartenbefehl[6]=0x00;
		K_Kartenbefehl[7]=Pinnummer;//Pin Nummer
		K_Kartenbefehl[8]=0x08;
		//Pin ab hier einfügen
		K_Kartenbefehl[9]=0x0ff;
		K_Kartenbefehl[10]=0x0ff;
		K_Kartenbefehl[11]=0x0ff;
		K_Kartenbefehl[12]=0x0ff;
		K_Kartenbefehl[13]=0x0ff;
		K_Kartenbefehl[14]=0x0ff;
		K_Kartenbefehl[15]=0x0ff;
		K_Kartenbefehl[16]=0x0ff;
		K_Antwortkode=K_Leser->SicherePineingabe(K_Kartenbefehl);
	}
	else
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte Pin unsichere Eingabe";
#endif
		//test mit hardcode
		K_Kartenbefehl.resize(13);
		K_Kartenbefehl[0]=0xA0;
		K_Kartenbefehl[1]=0x20;
		K_Kartenbefehl[2]=0x00;
		K_Kartenbefehl[3]=Pinnummer;
		K_Kartenbefehl[4]=0x08;
		//8 Byte PIN
		if (K_Pinspeicher.size()>8 || K_Pinspeicher.size()<4)
		{
#ifndef QT_NO_DEBUG
			qDebug()<<"QFrankGSMKarte Pin unsichere Eingabe: Pin zu lang/kurz";
#endif
			K_Fehlertext=trUtf8("Die Pin darf nicht länger als 8 und nicht kürzer als 4 Stellen sein.");
			K_Pinspeicher.clear();
			return false;
		}
		//Pin prüfen und ins Feld
		for (uint Stelle=0;Stelle<8;Stelle++)
		{
			//Ende erreicht?
			if(K_Pinspeicher.size()<Stelle+1)
				K_Kartenbefehl[Stelle+5]=0xff;			
			else
			{
				//teste auf gültiges Zeichen 0x30-0x39
				if(K_Pinspeicher.at(Stelle)<0x30 | K_Pinspeicher.at(Stelle)>0x39)
				{
#ifndef QT_NO_DEBUG
					qDebug()<<"QFrankGSMKarte Pin unsichere Eingabe: ungültiges Zeichen in der Pin";
#endif
					K_Fehlertext=tr("Die Pin darf nur aus den Zahlen 0-9 bestehen.");
					K_Pinspeicher.clear();
					return false;
				}
				K_Kartenbefehl[Stelle+5]=K_Pinspeicher.at(Stelle);
			}
			
		}
		K_Pinspeicher.clear();
		K_Antwortkode=K_Leser->UniversalIO(K_Kartenbefehl,K_Kartenantwort);
	}
	/*
	Auswertung
	Rückgabe Codes:
			0x9000 PinX akzeptiert
			0x9840 falscher PinX->PinX jetzt gesperrt
	*/
	switch (K_Antwortkode)
	{
		case QFrankLesegeraet::CommandSuccessful:
#ifndef QT_NO_DEBUG
													qDebug()<<"QFrankGSMKarte Pin Eingabe: Pin akzeptiert";
#endif
													return true;
													break;
		case QFrankLesegeraet::ResetNotSuccessful:
#ifndef QT_NO_DEBUG
													qDebug()<<"QFrankGSMKarte Pin Eingabe: Zeit abgelaufen";
#endif
													K_Fehlertext=tr("Zeit zu Eingabe der PIN abgelaufen.");					
													return false;
													break;
		case 0x9840:
#ifndef QT_NO_DEBUG
													qDebug()<<"QFrankGSMKarte Pin Eingabe: Pin gesperrt";
#endif
													K_Fehlertext=tr("PIN%1 gesperrt.").arg(Pinnummer);
													return false;
													break;
		case QFrankLesegeraet::CancelByCancelKey:
#ifndef QT_NO_DEBUG
													qDebug()<<"QFrankGSMKarte Pin Eingabe: abbruch gedrückt";
#endif
													K_Fehlertext=trUtf8("Abbruch gedrückt.");
													return false;
													break;
		case 0x9804:
#ifndef QT_NO_DEBUG
													qDebug()<<"QFrankGSMKarte Pin Eingabe: Pin falsch";
#endif
													//Kartenstus hohlen dazu wird das MF 0x3F00 gewählt
													if(!K_SelectFile(0x3f00))
													{
														K_Fehlertext=tr("Fehler beim Erkennen der verbleibenen Versuche");
														return false;
													}
													//Status des MF
													K_GetResponse(QFrankGSMKarte::MF_DF,(uchar)(K_Antwortkode&0x00ff));
													switch(Pinnummer)
													{
														case 1:
																	K_Fehlertext=tr("PIN%1 falsch noch %2 Versuche.").arg(Pinnummer)
																													.arg(K_MF_DFAntwort->
																														PIN1verbleibeneEingabeversuche());
																	break;
														case 2:
																	K_Fehlertext=tr("PIN%1 falsch noch %2 Versuche.").arg(Pinnummer)
																													.arg(K_MF_DFAntwort->
																														PIN2verbleibeneEingabeversuche());
																	break;
														default:
																	K_Fehlertext=tr("PIN falsch Anzahl der verbleibenen Versuche unbekannt");
																	break;
													}
													
													return false;
													break;
		case 0x9808:
#ifndef QT_NO_DEBUG
													qDebug()<<"QFrankGSMKarte Pin Eingabe: Pin war unnötig";
#endif
													return true;
													break;
		default:
#ifndef QT_NO_DEBUG
													qFatal("QFrankGSMKarte Pin Eingabe: Rückgabecode 0x%X nicht behandelt.",K_Antwortkode);
#endif
													K_Fehlertext=trUtf8("Fehler bei der PIN%1 Prüfung.").arg(Pinnummer);
													return false;
	}	
	return false;
}

const bool QFrankGSMKarte::Pin1Eingabe()
{
	if(K_PinEingabe(1))
		K_PIN1korrektEingegeben=true;
	else
		K_PIN1korrektEingegeben=false;
	return K_PIN1korrektEingegeben;
}

void QFrankGSMKarte::PinSichereEingabe(const bool &eingabeArt)
{
	K_SichereEingabeNutzen=eingabeArt;
}

QString const QFrankGSMKarte::K_TelefonbuchAuslesen(const QFrankGSMKarte::WelchesTelefonbuch &telefonbuch)
{
	/*	Diese stehen in der Datei 0x6f3a(kurz), 0x6f3b(fest), 0x6f40(meine), 0x6f44(zuletzt gewählt) im Verzeichnis Telekom 0x7f10 unter dem MF
		ArtDesTelefonbuch kann sein: Kurzwahnummern/Festwahlnummern/MeineNummern/ZuletztGeweaehlt
		XML Aufbau:
		<ArtDesTelefonbuch>
			<Speicherplaetze gesamt="xx" benutzt="xx"/>
			<Eintrag>
				<Beschreibung>Max Mustermann</Beschreibung>
				<Rufnummer>491234545</Rufnummer>
				<Nummerparameter ton="1" npi="1"/>
			</Eintrag>
		</ArtDesTelefonbuch>
	*/
	QString TelefonbuchText;
	uint DateiID;
	switch(telefonbuch)
	{
		case QFrankGSMKarte::Kurzwahl:
											TelefonbuchText=tr("Kurzwahlnummern");
											DateiID=0x6f3a;
											break;
		case QFrankGSMKarte::Festwahl:
											TelefonbuchText=tr("Festwahlnummern");
											DateiID=0x6f3b;
											break;
		case  QFrankGSMKarte::MeineNumern:
											TelefonbuchText=tr("Meine Nummern");
											DateiID=0x6f40;
											break;
		case QFrankGSMKarte::ZuletztGewaehlte:
											TelefonbuchText=trUtf8("Zuletzt gewählte Nummern");
											DateiID=0x6f44;
											break;
	}	
#ifndef QT_NO_DEBUG
	qDebug()<<"QFrankGSMKarte"<<TelefonbuchText;
#endif
	uchar AnzahlDerSpeicherplaetze;
	uchar LaengeEinesDatensatzes;
	uchar Textlaenge;
	if (!K_VerbindungZurKarte())
		return "";
	//In das Verzeichnis Telekom wechseln
	if (!K_SelectFile(0x7f10))
		return "";
	//Status des Verzeichnis:
	K_GetResponse(QFrankGSMKarte::MF_DF,(uchar)(K_Antwortkode&0x00ff));
	//Datei mit den eigentlichen Infos auswählen
	if (!K_SelectFile(DateiID))
		return "";
	//Status der Datei
	K_GetResponse(QFrankGSMKarte::EF,(uchar)(K_Antwortkode&0x00ff));
	LaengeEinesDatensatzes=K_EFAntwort->Datensatzlaenge();
	Textlaenge=LaengeEinesDatensatzes-0x0e;//Jeder daten min. 14 Bytes
	AnzahlDerSpeicherplaetze=K_EFAntwort->Dateigroesse()/LaengeEinesDatensatzes;
#ifndef QT_NO_DEBUG
	qDebug()<<"QFrankGSMKarte"<<TelefonbuchText<< ": Datensatzgröße"<<LaengeEinesDatensatzes;
	qDebug()<<"QFrankGSMKarte"<<TelefonbuchText<<": Datensaetze"<<AnzahlDerSpeicherplaetze;
#endif
	//Zum Auslesen muss PIN1 eingegeben worden sein.
	if (!K_PIN1korrektEingegeben)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte"<< TelefonbuchText <<": Pin1 nicht freigeschaltet";
#endif
		K_Fehlertext=trUtf8("Korrekter PIN1 wurde nicht übergeben.");
		return "";
	}
	/*	Auslesen der Nummern
		Aufbau des Datensatzes:
			Byte 1 - X Text wenn x=0 Dann kein Text
			Byte X+1 Bytes für die TON/NPI und Telefonnummer
			Byte X+2 Bit 8 =1 7-5 TON 4-1 NPI
					TON: 0x00=unbekannt
						 0x01=internationale Nummer angabe ohne Präfix(z.b +)
						 0x02=nationale Nummer
						 0x03=Netzwerk spezifische Nummer
						 0x04=Kurz Kode
						 0x05-7 reserviert
					NPI: 0x00=unbekannt
						 0x01=Telefon
						 0x03=Daten
						 0x04=Telex
						 0x08=National
						 0x09=Private
						 0x0b= reservier für CTS
						 rest reserviert
			Byte X+3 - X +12 Telefonnumer
			Byte X+13 Datensatznummer in der CCP Datei 0xff=kein Eintrag 
			Byte X+14 Datensatznummer in der EXT1/2 Datei 0xff=kein Eintrag
			EXT1 Datei kann zum Speichern von längeren Nummern oder mehr Wahlparametern genutzt werden.
	*/
	uchar BenutzteEintraege=0;
	QDomDocument XML;
	QDomElement Wurzel;
	QDomProcessingInstruction Kopf = XML.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""); 
	XML.appendChild(Kopf);
	switch(telefonbuch)
	{
		case QFrankGSMKarte::Kurzwahl:
											Wurzel = XML.createElement("Kurzwahlnummern");
											break;
		case QFrankGSMKarte::Festwahl:
											Wurzel = XML.createElement("Festwahlnummern");
											break;
		case  QFrankGSMKarte::MeineNumern:
											Wurzel = XML.createElement("MeineNummern");
											break;
		case QFrankGSMKarte::ZuletztGewaehlte:
											Wurzel = XML.createElement("ZuletztGeweaehlt");
											break;
	}
	QDomElement Speicherplatz= XML.createElement("Speicherplaetze");
	Speicherplatz.setAttribute("gesamt",AnzahlDerSpeicherplaetze);
	XML.appendChild(Wurzel);
	Wurzel.appendChild(Speicherplatz);
	for (uint Datensatz=1;Datensatz<=AnzahlDerSpeicherplaetze;Datensatz++)
	//for (uint Datensatz=1;Datensatz<=3;Datensatz++)
	{		
		K_ReadRecord(Datensatz,QFrankGSMKarte::Absolut,LaengeEinesDatensatzes);
		/* Ist der Datensatz leer? Wenn nein dann bearbeiten.
		   Wenn er leer ist, ist TON/NPI 0xff
		*/
		if ((uchar)K_Kartenantwort.at(Textlaenge+1)!=0xff)
		{
			QDomElement Eintrag=XML.createElement("Eintrag");
			Wurzel.appendChild(Eintrag);
			QDomElement Beschreibung=XML.createElement("Beschreibung");
			QDomElement Rufnummer=XML.createElement("Rufnummer");
			QDomElement Nummerparameter=XML.createElement("Nummerparameter");
		
			QDomText BeschreibungText=XML.createTextNode(K_Zeichenwandlung->SMSnachUTF8(K_Kartenantwort.left(Textlaenge)));
			//Nummer Länger als 20 stellen? wenn ja dann ist der Extensions1 ID !=0xff
			QDomText RufnummerText;
			if ((uchar)K_Kartenantwort.at(Textlaenge+13)!=0xff)
			{
#ifndef QT_NO_DEBUG
				qWarning()<<"QFrankGSMKarte" << TelefonbuchText <<": Nummer >20 Stellen";
#endif
				RufnummerText=XML.createTextNode(K_TelefonnummerDecodieren(K_Kartenantwort.mid(Textlaenge+2,10))+"...");
			}
			else
				RufnummerText=XML.createTextNode(K_TelefonnummerDecodieren(K_Kartenantwort.mid(Textlaenge+2,10)));
			Nummerparameter.setAttribute("TON",(((uchar) K_Kartenantwort.at(Textlaenge+1))& 0x70) >>4 );
			Nummerparameter.setAttribute("NPI",((uchar)K_Kartenantwort.at(Textlaenge+1))& 0x0f );
		
			Eintrag.appendChild(Beschreibung);
			Eintrag.appendChild(Rufnummer);
			Eintrag.appendChild(Nummerparameter);
			Beschreibung.appendChild(BeschreibungText);
			Rufnummer.appendChild(RufnummerText);
			BenutzteEintraege++;
		}
	}
	Speicherplatz.setAttribute("benutzt",BenutzteEintraege);
	
	//Nur zum Debuggen
	QFile Datei(TelefonbuchText+".xml");
	Datei.open(QIODevice::WriteOnly);	
	QTextStream Strom(&Datei);
	Strom<<XML.toString();
	Strom.flush();
	Datei.close();
	
	return XML.toString();
}

const QString QFrankGSMKarte::Kurzwahlnummern()
{	
	return K_TelefonbuchAuslesen(QFrankGSMKarte::Kurzwahl);
}

const QString QFrankGSMKarte::MeineNummern()
{
	return K_TelefonbuchAuslesen(QFrankGSMKarte::MeineNumern);
}

const QString QFrankGSMKarte::Festwahlnummern()
{
	return K_TelefonbuchAuslesen(QFrankGSMKarte::Festwahl);
}

const QString QFrankGSMKarte::ZuletztGewaehlteNummern()
{
	return K_TelefonbuchAuslesen(QFrankGSMKarte::ZuletztGewaehlte);
}

QString const QFrankGSMKarte::K_TelefonnummerDecodieren(const QByteArray &nummernfeld) const
{
	/*	Die Telefonnummer kann max. 20 Stellen je Datenblock sei.
		Kodierung: Bit 8-5 2. Stelle 4-1 1. Stelle. 0xff wenn nicht belegt.
	*/
	QString Telefonnummer="";
	uchar Stelle1,Stelle2;
	for (int Stelle=0;Stelle<20;Stelle++)
	{
		Stelle1=((uchar)nummernfeld.at(Stelle))&0x0f;
		Stelle2=(((uchar)nummernfeld.at(Stelle))&0xf0)>>4;
		//Ende erreicht??
		if(Stelle1==0x0f && Stelle2==0x0f)
			break;
		if(Stelle1==0x0f)
			break;
		Telefonnummer.append(K_TelefonnummerZeichenDecodieren(Stelle1));
		if(Stelle2==0x0f)
			break;
		Telefonnummer.append(K_TelefonnummerZeichenDecodieren(Stelle2));
	}
	return Telefonnummer;
}

QChar const QFrankGSMKarte::K_TelefonnummerZeichenDecodieren(const uchar &nummer)const
{
	switch(nummer)
	{
		case 0x0a:
					return QChar(0x2a);//*
					break;
		case 0x0b:
					return QChar(0x23);//#
					break;
		case 0x0c:
					return QChar();//DTMF Control Seperator
					break;
		case 0x0d:
					return QChar();//Steuerzeichen zum abfragen einer Ziffer
					break;
		case 0x0e:
					return QChar();//Erhöhe die nächte Ziffer um 10
					break;
		default:
					return QChar(nummer+0x30);
					break;
	}
	return QChar();
}

bool QFrankGSMKarte::K_ReadRecord(const uchar &datsatznummer,const QFrankGSMKarte::DatensatzLesemodus &modus,const uchar &datensatzlaenge)
{
	/* Read Record
		Klasse=0xA0
		INS=0xB2
		P1=Datensatznummer
		P2=Modus
		P3=Datensatzlänge
	*/
#ifndef QT_NO_DEBUG
	qDebug()<<QString("QFrankGSMKarte Read Record Nummer: %1 Modus: %2 Länge: %3").arg(datsatznummer).arg(modus).arg(datensatzlaenge);
#endif
	if (!K_VerbindungZurKarte())
		return false;
	K_Kartenbefehl.resize(5);
	K_Kartenbefehl[0]=0xA0;
	K_Kartenbefehl[1]=0xb2;
	K_Kartenbefehl[2]=datsatznummer;
	K_Kartenbefehl[3]=(uchar)modus;
	K_Kartenbefehl[4]=datensatzlaenge;
	K_Antwortkode=K_Leser->UniversalIO(K_Kartenbefehl,K_Kartenantwort);
	//Wenn alles gut ging 0x9000
	if (K_Antwortkode!=QFrankLesegeraet::CommandSuccessful)
	{
		K_Fehlertext=tr("Datensatz lesen gescheitert.");
#ifndef QT_NO_DEBUG
		qDebug()<<QString("QFrankGSMKarte Read Record: gescheitert Rückgabe Code 0x%1").arg(K_Antwortkode,0,16);
#endif
		K_Leser->KarteEntfernen();
		return false;
	}
#ifndef QT_NO_DEBUG
	qDebug()<<QString("QFrankGSMKarte Read Record: OK Rückgabe Code 0x%1").arg(K_Antwortkode,0,16);
	qDebug()<<"QFrankGSMKarte Read Record Daten:"<<K_FeldNachHex(K_Kartenantwort);
#endif
	return true;
}

const QString QFrankGSMKarte::Anbieter()
{
	//EF für den Mobilfunkanbieter ist 0x6f46 in dem Verzeichnis GSM 0x7f20
#ifndef QT_NO_DEBUG
	qDebug()<<"QFrankGSMKarte Anbieter";
#endif
	if (!K_VerbindungZurKarte())
		return "";
	//Ins Verzeichnis GSM wechseln
	if (!K_SelectFile(0x7f20))
		return "";
	//Status des Verzeichnis:
	K_GetResponse(QFrankGSMKarte::MF_DF,(uchar)(K_Antwortkode&0x00ff));
	//Datei mit den Eigentlichen Infos auswählen
	if (!K_SelectFile(0x6f46))
		return "";
	//Status der Datei
	K_GetResponse(QFrankGSMKarte::EF,(uchar)(K_Antwortkode&0x00ff));
	//Datei auslesen
	if (!K_ReadBinary((uchar)K_EFAntwort->Dateigroesse()))
		return "";
	//1. Byte unwichtig 2-Ende oder FF enthält den Text
	QString tmp=K_Zeichenwandlung->SMSnachUTF8(K_Kartenantwort.mid(1,K_EFAntwort->Dateigroesse()-1));
#ifndef QT_NO_DEBUG
	qDebug()<<"QFrankGSMKarte Anbieter:"<<tmp;
#endif
	return tmp;
}

bool QFrankGSMKarte::K_SelectFile(const uint &was)
{
	/*Select file
		Klasse=0xA0
		INS=0xA4
		P1=0x00
		P2=0x00
		Länge Daten=0x02
	*/
#ifndef QT_NO_DEBUG
	qDebug()<<QString("QFrankGSMKarte Select File 0x%1").arg(was,0,16);
#endif
if (!K_VerbindungZurKarte())
		return false;
	K_Kartenbefehl.resize(7);
	K_Kartenbefehl[0]=0xA0;
	K_Kartenbefehl[1]=0xA4;
	K_Kartenbefehl[2]=0x00;
	K_Kartenbefehl[3]=0x00;
	K_Kartenbefehl[4]=0x02;
	K_Kartenbefehl[5]=(uchar) ((was >>8)&0xff);
	K_Kartenbefehl[6]=(uchar) (was & 0xff);
	K_Antwortkode=K_Leser->UniversalIO(K_Kartenbefehl,K_Kartenantwort);
	/*  Wenn es geklappt hat dann bekommen wird eine 0x9FXX
		wobei XX die Länge der abzuhohlen Rückeantwort des Befehls ist.
	*/
	if (K_Antwortkode<0x9f00 || K_Antwortkode>0x9fff)
	{
		K_Fehlertext=trUtf8("Datei/Verzeichnis öffnen gescheitert.");
#ifndef QT_NO_DEBUG
		qDebug()<<QString("QFrankGSMKarte Select File: gescheitert Rückgabe Code 0x%1").arg(K_Antwortkode,0,16);
#endif
		K_Leser->KarteEntfernen();
		return false;
	}
#ifndef QT_NO_DEBUG
	qDebug()<<QString("QFrankGSMKarte Select File: OK Rückgabe Code 0x%1").arg(K_Antwortkode,0,16);
#endif
	return true;
}

bool QFrankGSMKarte::K_ReadBinary(const uchar &anzahl,const uint &offset)
{
	/*Read Binary
		Klasse=0xA0
		INS=0xBo
		P1=Offset high
		P2=Offset Low
		Länge Daten= anznzahö
	*/
#ifndef QT_NO_DEBUG
	qDebug()<<"QFrankGSMKarte ReadBinary";
#endif
	if (!K_VerbindungZurKarte())
		return false;
	K_Kartenbefehl.resize(5);
	K_Kartenbefehl[0]=0xA0;
	K_Kartenbefehl[1]=0xB0;
	K_Kartenbefehl[2]=(uchar)(offset & 0xff00);
	K_Kartenbefehl[3]=(uchar)(offset & 0x00ff);
	K_Kartenbefehl[4]=anzahl;
	K_Antwortkode=K_Leser->UniversalIO(K_Kartenbefehl,K_Kartenantwort);
	//wenn es gut ging dann 0x9000
	if(K_Antwortkode!=QFrankLesegeraet::CommandSuccessful)
	{
		K_Fehlertext=tr("Es konnte von der GSM Karte nicht gelesen werden.");
#ifndef QT_NO_DEBUG
		qDebug()<<QString("QFrankGSMKarte ReadBinary: Lesefehler Rückgabe Code: %1").arg(K_Antwortkode,0,16);
#endif
		K_Leser->KarteEntfernen();
		return false;
	}
#ifndef QT_NO_DEBUG
	qDebug()<<"QFrankGSMKarte ReadBinary Daten:"<<K_FeldNachHex(K_Kartenantwort);
#endif
	return true;
}

bool QFrankGSMKarte::K_VerbindungZurKarte()
{
	//Prüft ob eine Verbindung zum Leser besteht.
	if(K_Leser==0)
	{
		K_Fehlertext=tr("Keine Verbindung zum Kartenleser");
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte: keine Verbindung zum Leser";
#endif
		return false;
	}
	else if(!K_KarteAkiviert)
	{
		K_Fehlertext=tr("Keine Verbindung zur Karte");
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte: keine Verbindung zur Karte";
#endif
		return false;
	}
	return true;
}

void QFrankGSMKarte::K_GetResponse(QFrankGSMKarte::Antwort antwort,const uchar &antwortLaenge)
{
	if(!K_VerbindungZurKarte())
		return;
	/*  K_AntwortLaenge beinhaltet die Länge der Antwort die die Karte bereit hält
		Befehl: Get Response Antwortlänge
		Kode: 0xA0-C0-00-00-Antwortlänge
	*/
	K_Kartenbefehl.resize(5);
	K_Kartenbefehl[0]=0xA0;
	K_Kartenbefehl[1]=0xC0;
	K_Kartenbefehl[2]=0x00;
	K_Kartenbefehl[3]=0x00;
	K_Kartenbefehl[4]=antwortLaenge;
	K_Antwortkode=K_Leser->UniversalIO(K_Kartenbefehl,K_Kartenantwort);
	//9000 alles OK
	if (K_Antwortkode!=QFrankLesegeraet::CommandSuccessful)
	{
		K_Fehlertext=tr("Es konnte die Antwort eines Befehls nicht empfangen werden.");
#ifndef QT_NO_DEBUG
		qDebug()<<QString("QFrankGSMKarte Kartenantwort hohlen: Fehler Rückgabe Code: %1").arg(K_Antwortkode,0,16);
		qDebug()<<K_FeldNachHex(K_Kartenantwort);
#endif
		K_Leser->KarteEntfernen();
		K_KarteAkiviert=false;
		return;
	}
	//Auswertung
	switch (antwort)
	{
		case QFrankGSMKarte::MF_DF:
#ifndef QT_NO_DEBUG
						qDebug()<<"QFrankGSMKarte Kartenantwort hohlen: werte MF/DF aus";
#endif
						K_MF_DFAntwortBearbeiten();	
						
						break;
		case QFrankGSMKarte::EF:
#ifndef QT_NO_DEBUG
						qDebug()<<"QFrankGSMKarte Kartenantwort hohlen: werte EF aus";
#endif
						K_EFAntwortBearbeiten();
						break;
	}
}

void QFrankGSMKarte::K_MF_DFAntwortBearbeiten()
{
	/*  Struktur:
		Byte 1-2 RFU
		Byte 3-4 freier Speicher für EF/DF unter dem DF/MF
		Byte 5-6 Datei ID
		Byte 7 Dateitype 0x00=RFU,0x01=MF,0x02=DF,0x04=EF
		Byte 8-12 RFU
		Byte 13 Länge des folgenden Datenfeldes
		Byte 14-34 GSM spezifisch
		GSM Struktur:
		Byte 14 Datei Beschreibung. interissant nur Bit 8
			ist es 0=PIN1 aktiviert 1=PIN1 deaktiviert
		Byte 15 Anzahl der Unterverzeichnisse unter diesem
		Byte 16 Anzahl der Dateien unter diesem
		Byte 17 Anzahl der PIN's, super PIN's und Admin Codes
		Byte 18 RFU
		Byte 19 PIN1 Status Bit 8=0 PIN nicht gesetzt
			8=1 PIN gesetzt
			Bit 7-5 RFU
			Bit 4-3 Anzahl der verbleibenen Versuche der Pineingabe 0=gesperrt
		Byte 20 Super PIN1 Status siehe Byte 19
		Byte 21 PIN2 Status siehe Byte 19
		Byte 22 Super PIN2 Status siehe Byte 19
		Byte 23 RFU
		Byte 24-34 Reserviert für admin(wenn vorhanden)
		Byte 35> RFU
	*/
	//Freier Speicher
	uint Puffer=((uint)K_Kartenantwort.at(2)) <<8 |((uchar)K_Kartenantwort.at(3));
	K_MF_DFAntwort->FreierSpeicherSetzen(Puffer);
	//VerzeichnisID
	Puffer=((uint)K_Kartenantwort.at(4)) <<8 |((uchar)K_Kartenantwort.at(5));
	K_MF_DFAntwort->DateiIDSetzen(Puffer);
	//Verzeichnis Art
	switch(K_Kartenantwort.at(6))
	{
		case QFrankGSMKarteAntwortbasis::MF:
							K_MF_DFAntwort->DateiartSetzen(QFrankGSMKarteAntwortbasis::MF);
							break;
		case QFrankGSMKarteAntwortbasis::DF:
							K_MF_DFAntwort->DateiartSetzen(QFrankGSMKarteAntwortbasis::DF);
							break;
		default:
#ifndef QT_NO_DEBUG
							qFatal("QFrankGSMKarte Kartenantwort DF/MF: unerwarteter Verzeichnistype 0x%X",
								(uchar)K_Kartenantwort.at(6));
#endif
							break;
	}
	//PIN1 aktiviert??
	if( (((uchar)K_Kartenantwort.at(13)) & 0x80)==0x80)
		K_MF_DFAntwort->PIN1AktiviertSetzen(true);
	else
		K_MF_DFAntwort->PIN1AktiviertSetzen(false);
	//Anzahl der Unterverzeichnisse im Verzeichnis
	K_MF_DFAntwort->AnzahlDerUnterverzeichnisseSetzen((uchar)K_Kartenantwort.at(14));
	//Anzahl der Dateien im Verzeichnis
	K_MF_DFAntwort->AnzahlDerDateienSetzen((uchar)K_Kartenantwort.at(15));
	//Anzahl der Sicherheitscodes
	K_MF_DFAntwort->AnzahlDerPINsSetzen((uchar)K_Kartenantwort.at(16));
	//Status PIN1
	if( (((uchar)K_Kartenantwort.at(18)) &0x80)==0x80 )
		K_MF_DFAntwort->PIN1gesetztSetzen(true);
	else
		K_MF_DFAntwort->PIN1gesetztSetzen(false);
	K_MF_DFAntwort->PIN1verbleibeneEingabeversucheSetzen(((uchar)K_Kartenantwort.at(18))&0x0f);
	//Status SuperPin1
	if( (((uchar)K_Kartenantwort.at(19)) &0x80)==0x80 )
		K_MF_DFAntwort->SuperPIN1gesetztSetzen(true);
	else
		K_MF_DFAntwort->SuperPIN1gesetztSetzen(false);
	K_MF_DFAntwort->SuperPIN1verbleibeneEingabeversucheSetzen(((uchar)K_Kartenantwort.at(19))&0x0f);
	//Status PIN2
	if( (((uchar)K_Kartenantwort.at(20)) &0x80)==0x80 )
		K_MF_DFAntwort->PIN2gesetztSetzen(true);
	else
		K_MF_DFAntwort->PIN2gesetztSetzen(false);
	K_MF_DFAntwort->PIN2verbleibeneEingabeversucheSetzen(((uchar)K_Kartenantwort.at(20))&0x0f);
	//Status SuperPin2
	if( (((uchar)K_Kartenantwort.at(21)) &0x80)==0x80 )
		K_MF_DFAntwort->SuperPIN2gesetztSetzen(true);
	else
		K_MF_DFAntwort->SuperPIN2gesetztSetzen(false);
	K_MF_DFAntwort->SuperPIN2verbleibeneEingabeversucheSetzen(((uchar)K_Kartenantwort.at(21))&0x0f);
#ifndef QT_NO_DEBUG
	qDebug()<<"QFrankGSMKarte Kartenantwort DF/MF:";
	qDebug()<<QString("\tFreier Spreicher im Verzeichnis: 0x%1").arg(K_MF_DFAntwort->FreierSpeicher(),0,16);
	qDebug()<<QString("\tVerzeichnis ID: 0x%1").arg(K_MF_DFAntwort->DateiID(),0,16);
	switch (K_MF_DFAntwort->Dateiart())
	{
		case QFrankGSMKarteAntwortbasis::MF:
							qDebug()<<"\tVerzeichnistype: MF";
							break;
		case QFrankGSMKarteAntwortbasis::DF:
							qDebug()<<"\tVerzeichnistype: DF";
							break;
	}
	if(K_MF_DFAntwort->PIN1Aktiviert())
		qDebug()<<"\tPIN1 aktiviert";
	else
		qDebug()<<"\tPIN1 deaktiviert";
	qDebug()<<QString("\tAnzahl der Unterverzeichnisse: %1").arg(K_MF_DFAntwort->AnzahlDerUnterverzeichnisse());
	qDebug()<<QString("\tAnzahl der Dateien: %1").arg(K_MF_DFAntwort->AnzahlDerDateien());
	qDebug()<<QString("\tAnzahl der Sicherheitscodes: %1").arg(K_MF_DFAntwort->AnzahlDerPINs());
	qDebug()<<"\tStatus PIN1";
	if(K_MF_DFAntwort->PIN1gesetzt())
		qDebug()<<"\t\tgesetzt";
	else
		qDebug()<<"\t\tnicht gesetzt";
	qDebug()<<QString("\t\tverbleibene Eingabeversuche: %1 bei 0 gesperrt").arg(K_MF_DFAntwort->PIN1verbleibeneEingabeversuche());
	qDebug()<<"\tStatus SuperPIN1";
	if(K_MF_DFAntwort->SuperPIN1gesetzt())
		qDebug()<<"\t\tgesetzt";
	else
		qDebug()<<"\t\tnicht gesetzt";
	qDebug()<<QString("\t\tverbleibene Eingabeversuche: %1 bei 0 gesperrt").arg(K_MF_DFAntwort->SuperPIN1verbleibeneEingabeversuche());
	qDebug()<<"\tStatus PIN2";
	if(K_MF_DFAntwort->PIN2gesetzt())
		qDebug()<<"\t\tgesetzt";
	else
		qDebug()<<"\t\tnicht gesetzt";
	qDebug()<<QString("\t\tverbleibene Eingabeversuche: %1 bei 0 gesperrt").arg(K_MF_DFAntwort->PIN2verbleibeneEingabeversuche());
	qDebug()<<"\tStatus SuperPIN2";
	if(K_MF_DFAntwort->SuperPIN2gesetzt())
		qDebug()<<"\t\tgesetzt";
	else
		qDebug()<<"\t\tnicht gesetzt";
	qDebug()<<QString("\t\tverbleibene Eingabeversuche: %1 bei 0 gesperrt").arg(K_MF_DFAntwort->SuperPIN2verbleibeneEingabeversuche());
#endif
}

void QFrankGSMKarte::K_EFAntwortBearbeiten()
{
	/*  Struktur:
		Byte 1-2 RFU
		Byte 3-4 Dateigröße bei transparenten Bodygröße
			 sonst Anzahl der Einträge mal Länge eines einzelnen Datensatzes
		Byte 5-6 Datei ID
		Byte 7 Dateitype siehe Byte 7 MF/DF
		Byte 8 RFU für transparente und Dateien fixer Länge.
		       Für zyklische Dateien alle Bits ausser Bit 7 RFU
		       Bit 7= INCREASE Befehl zulässig
		Byte 9-11 Zugriffsmöglichkeiten:
			  Wertigkeit der Einträge: 0=immer,1=PIN1(wenn gesetzt),2=PIN2(wenn gesetzt)
			  3=RFU, 4-14=admin,15=nie
		Byte 9 Bit 8-5 Kommando Read,Seek
		       Bit 4-1 Kommando Update
		Byte 10 Bit 8-5 Kommando INCREASE
		       Bit 4-1 RFU
		Byte 11 Bit 8-5 Kommando Rehabilitate
		       Bit 4-1 Kommando Invalidate
		Byte 12 Dateistatus Bit 8-2 RFU 1=0 üngültig 1=1 gültig
		Byte 13 Länge der folgenden Daten
		Byte 14 Struktur der Datei 00=transparent,01=feste Länge,03=zyklisch
		Byte 15 Länge eines Eintrages bei transparenten Dateien=00 sonst länge eines Datensatzes.
		Byte 16> RFU
	*/
	//Dateigröße
	uint Puffer;
	Puffer=((uint)K_Kartenantwort.at(2)) <<8 |((uchar)K_Kartenantwort.at(3));
	K_EFAntwort->DateigroesseSetzen(Puffer);	
	//DateiID
	Puffer=((uint)K_Kartenantwort.at(4)) <<8 |((uchar)K_Kartenantwort.at(5));
	K_EFAntwort->DateiIDSetzen(Puffer);
	//Art der Datei
	switch (K_Kartenantwort.at(6))
	{
		case QFrankGSMKarteAntwortbasis::EF:
							K_EFAntwort->DateiartSetzen(QFrankGSMKarteAntwortbasis::EF);
							break;
		default:
#ifndef QT_NO_DEBUG
							qFatal("QFrankGSMKarte Kartenantwort EF: unerwarteter Dateitype 0x%X",
								(uchar)K_Kartenantwort.at(6));
#endif
							break;
	}
	//Dateistruktur
	switch(K_Kartenantwort.at(13))
	{
		case QFrankGSMKarteEFAntwort::Transparent:
								K_EFAntwort->DateiaufbauSetzen(QFrankGSMKarteEFAntwort::Transparent);
								break;
		case QFrankGSMKarteEFAntwort::FesteLaenge:
								K_EFAntwort->DateiaufbauSetzen(QFrankGSMKarteEFAntwort::FesteLaenge);
								break;
		case QFrankGSMKarteEFAntwort::Zyklisch:
								K_EFAntwort->DateiaufbauSetzen(QFrankGSMKarteEFAntwort::Zyklisch);
								break;
		default:
#ifndef QT_NO_DEBUG
								qFatal("QFrankGSMKarte Kartenantwort EF: unerwartete Dateistruktur 0x%X",
									(uchar)K_Kartenantwort.at(13));

#endif
								break;
	}
	//nur wichtig wenn es eine zueklischen Datei ist
	if(K_EFAntwort->Dateiaufbau()==QFrankGSMKarteEFAntwort::Zyklisch)
	{
		if (K_Kartenantwort.at(7)==0x40)
			K_EFAntwort->ErhoehenFuerZueklischEFErlaubtSetzen(true);
		else
			K_EFAntwort->ErhoehenFuerZueklischEFErlaubtSetzen(false);
	}
	//Wer darf was mit der Datei
	uchar werDarfWasLinks=(uchar)K_Kartenantwort.at(8) >>4;//linkter Teil
	uchar werDarfWasRechts=(uchar)K_Kartenantwort.at(8) & 0x0f;//rechter Teil
	K_EFAntwort->BerechtigungLesenSuchenSetzen((QFrankGSMKarteEFAntwort::Zugriffsberechtigung)werDarfWasLinks);
	K_EFAntwort->BerechtigungAktualisierenSetzen((QFrankGSMKarteEFAntwort::Zugriffsberechtigung)werDarfWasRechts);
	werDarfWasLinks=(uchar)K_Kartenantwort.at(9) >>4;
	K_EFAntwort->BerechtigungErhoehenSetzen((QFrankGSMKarteEFAntwort::Zugriffsberechtigung)werDarfWasLinks);
	werDarfWasLinks=(uchar)K_Kartenantwort.at(10) >>4;
	werDarfWasRechts=(uchar)K_Kartenantwort.at(10) & 0x0f;
	K_EFAntwort->BerechtigungRehabilitierenSetzen((QFrankGSMKarteEFAntwort::Zugriffsberechtigung)werDarfWasLinks);
	K_EFAntwort->BerechtigungUngueltigErklaerenSetzen((QFrankGSMKarteEFAntwort::Zugriffsberechtigung)werDarfWasRechts);
	//Dateistatus
	K_EFAntwort->DateiGueltigSetzen((bool)(((uchar)K_Kartenantwort.at(11)) &0x01));
	//Datensatzlänge
	K_EFAntwort->DatensatzlaengeSetzen((uchar)K_Kartenantwort.at(14));
#ifndef QT_NO_DEBUG
	qDebug()<<"QFrankGSMKarte Kartenantwort EF:";
	qDebug()<<QString("\tDateigröße: 0x%1").arg(K_EFAntwort->Dateigroesse(),0,16);
	qDebug()<<QString("\tDatei ID: 0x%1").arg(K_EFAntwort->DateiID(),0,16);
	switch (K_EFAntwort->Dateiart())
	{
		case QFrankGSMKarteAntwortbasis::EF:
							qDebug()<<"\tDateitype: EF";
							break;
	}
	switch(K_EFAntwort->Dateiaufbau())
	{
		case QFrankGSMKarteEFAntwort::Transparent:
								qDebug()<<"\tDateiaufbau: Transparent";
								break;
		case QFrankGSMKarteEFAntwort::FesteLaenge:
								qDebug()<<"\tDateiaufbau: Datensatz fester Länge";
								break;
		case QFrankGSMKarteEFAntwort::Zyklisch:
								qDebug()<<"\tDateiaufbau: zyklische Datensätze";
								if(K_EFAntwort->ErhoehenFuerZueklischEFErlaubt())
									qDebug()<<"\t\teröhen erlaubt";
								else
									qDebug()<<"\t\terhöhen verboten";
								break;
	}
	qDebug()<<"\tDateiberechtigung";
	qDebug()<<"\t\tLesen/Suchen:"<<K_Zugrifftext(K_EFAntwort->BerechtigungLesenSuchen());
	qDebug()<<"\t\tAktualisieren:"<<K_Zugrifftext(K_EFAntwort->BerechtigungAktualisieren());
	qDebug()<<"\t\tErhöhen:"<<K_Zugrifftext(K_EFAntwort->BerechtigungErhoehen());
	qDebug()<<"\t\tRehabilitieren:"<<K_Zugrifftext(K_EFAntwort->BerechtigungRehabilitieren());
	qDebug()<<"\t\tUngültig erklären:"<<K_Zugrifftext(K_EFAntwort->BerechtigungUngueltigErklaeren());
	if(K_EFAntwort->DateiGueltig())
		qDebug()<<"\tDatei gültig";
	else
		qDebug()<<"\tDatei ungültig";
	qDebug()<<"\tDatensatzlänge in Bytes"<<K_EFAntwort->Datensatzlaenge();
#endif	
}

const QString QFrankGSMKarte::Fehlertext() const
{
	return K_Fehlertext;
}

ulong QFrankGSMKarte::Version()
{
	return GSMKarteVersion;
}

const QString QFrankGSMKarte::Seriennummer() const
{
	return K_Seriennummer;
}

void QFrankGSMKarte::welchenLeser(QFrankLesegeraet *diesen)
{
	K_Leser=diesen;
	if(diesen->LeserInitialisieren()!=QFrankLesegeraet::CommandSuccessful)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte: Leser initialisieren gescheitert.";
#endif
		K_Leser=0;
		return;
	}	

}

#ifndef QT_NO_DEBUG
QString	QFrankGSMKarte::K_FeldNachHex(const QByteArray &feld) const
{
	QString tmp="";
	uchar low,high;
	for(int x=0;x<feld.size();x++)
	{
		//Byte zerlegen
		high=((feld.at(x) & 0xf0) >>4)+0x30;
		low=(feld.at(x) & 0x0f)+0x30;
		if(high>0x39)
			high=high+0x07;
		if(low>0x39)
			low=low+0x07;
		tmp.append(high);
		tmp.append(low);
		tmp.append("-");
	}
	return tmp.left(tmp.size()-1);
}

QString	QFrankGSMKarte::K_Zugrifftext(const uchar &kodierung) const
{
	switch(kodierung)
	{
		case QFrankGSMKarteEFAntwort::Immer:
							return "immer";
							break;
		case QFrankGSMKarteEFAntwort::PIN1:
							return "PIN1";
							break;
		case QFrankGSMKarteEFAntwort::PIN2:
							return "PIN2";
							break;
		case QFrankGSMKarteEFAntwort::RFU:
							return "reserviert";
							break;
		case QFrankGSMKarteEFAntwort::Admin01:
							return "Admin01";
							break;
		case QFrankGSMKarteEFAntwort::Admin02:
							return "Admin02";
							break;
		case QFrankGSMKarteEFAntwort::Admin03:
							return "Admin03";
							break;
		case QFrankGSMKarteEFAntwort::Admin04:
							return "Admin04";
							break;
		case QFrankGSMKarteEFAntwort::Admin05:
							return "Admin05";
							break;
		case QFrankGSMKarteEFAntwort::Admin06:
							return "Admin06";
							break;
		case QFrankGSMKarteEFAntwort::Admin07:
							return "Admin07";
							break;
		case QFrankGSMKarteEFAntwort::Admin08:
							return "Admin0";
							break;
		case QFrankGSMKarteEFAntwort::Admin09:
							return "Admin09";
							break;
		case QFrankGSMKarteEFAntwort::Admin10:
							return "Admin10";
							break;
		case QFrankGSMKarteEFAntwort::Admin11:
							return "Admin11";
							break;
		case QFrankGSMKarteEFAntwort::Nie:
							return "Nie";
							break;
	}
	return QString();
}
#endif

