/*
 *  Copyright (C) 2005-2006 Frank Büttner frank-buettner@gmx.net
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

#include "KVK.h"
#include <Lesegeraet.h>

QFrankKVK::QFrankKVK(QObject* eltern):QFrankSmartCard(eltern)
{
	setObjectName("QFrankKVK");
	VariabelnInitialisieren();
}

void QFrankKVK::VariabelnInitialisieren()
{
	KrankenkassennameWert="";
	KrankenkassennummerWert=0;
	WOPWert=0;
	VersichertennummerWert=0;
	VersichertenstatusWert=0;
	StatusergaenzungWert=QChar();
	TitelWert="";
	VornameWert="";
	NamenszusatzWert="";
	NachnameWert="";
	GeburtsdatumWert=QDate(1800,1,1);
	StrasseWert="";
	LandWert="";
	PLZWert="";
	OrtWert="";
	GueltigBisWert=QDate(1800,1,1);
	KVKLeserWert=false;
	Leser=0;
}

QString	QFrankKVK::Krankenkassenname()
{
	return KrankenkassennameWert;
}

ulong QFrankKVK::Krankenkassennummer()
{
	return KrankenkassennummerWert;
}

uint QFrankKVK::WOP()
{
	return WOPWert;
}

qulonglong	QFrankKVK::Versichertennummer()
{
	return VersichertennummerWert;
}

uint QFrankKVK::Versichertenstatus()
{
	return VersichertenstatusWert;
}

QChar QFrankKVK::Statusergaenzung()
{
	return StatusergaenzungWert;
}

QString	QFrankKVK::Titel()
{
	return TitelWert;
}

QString	QFrankKVK::Vorname()
{
	return VornameWert;
}

QString	QFrankKVK::Namenszusatz()
{
	return NamenszusatzWert;
}

QString	QFrankKVK::Nachname()
{
	return NachnameWert;
}

QDate QFrankKVK::Geburtsdatum()
{
	return GeburtsdatumWert;
}

QString	QFrankKVK::Strasse()
{
	return StrasseWert;
}

QString	QFrankKVK::Land()
{
	return	LandWert;
}

QString	QFrankKVK::PLZ()
{
	return PLZWert;
}

QString	QFrankKVK::Ort()
{
		return OrtWert;
}

QDate QFrankKVK::GueltigBis()
{
	return GueltigBisWert;
}

bool QFrankKVK::KVKLeser()
{
	return KVKLeserWert;
}

void QFrankKVK::KVKLeserSetzen(bool kvk)
{
	KVKLeserWert=kvk;
}

ulong QFrankKVK::Version()
{
	return KVKVersion;
}

bool QFrankKVK::KarteAuslesen()
{
	if(Leser==0)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"KVK Karte: Keine Verbindung mit Lesegerät.";
#endif
		return false;
	}
	QByteArray atr;
	if(Leser->KarteAnfordern(atr)!=QFrankLesegeraet::CommandSuccessful)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"KVK Karte: Karte anfordern gescheitert.";
#endif
		return false;
	}
	//Karte eingeführt wenn es ein KVK geprüftes Gerät ist, kann mit Select File gearbeitet werden.
	//sonst mit read binary
	if(KVKLeserWert)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"KVK Karte: Auslesen nach KVK-Anforderungen";
#endif
		QByteArray Anwendungskennung(9,0x00);
		Anwendungskennung[0]=0x04; //AID setzen da eh alle Felder auf 0x00 stehen, werden nur die geädernt,
									  //die nicht 0x00 sind
		Anwendungskennung[2]=0x06; //Länge der AID 6 Bytes neue AID=D2-76-00-00-01-01
									  //alte AID=D2-80-00-00-01-01
		Anwendungskennung[3]=0xd2;
		Anwendungskennung[4]=0x76; //neue Karte
		Anwendungskennung[7]=0x01;
		Anwendungskennung[8]=0x01;
		if(Leser->ISO_SelectFile(Anwendungskennung)!=QFrankLesegeraet::CommandSuccessful)
		{
#ifndef QT_NO_DEBUG
			qDebug()<<"KVK Karte: Select File gescheitert, versuche es mit der alten Kennung";
#endif
			Anwendungskennung[4]=0x80;
			if(Leser->ISO_SelectFile(Anwendungskennung)!=QFrankLesegeraet::CommandSuccessful)
			{
#ifndef QT_NO_DEBUG
				qDebug()<<"KVK Karte: Select File auch mit alter Kennung gescheitert";
#endif
				Leser->KarteEntfernen();
				return false;
			}
		}		
	}
	//jezt kann gelesen werden
	QByteArray DatenDerKarte(256,0);//Die KVK beinhaltet maximal 256Bytes
	//Die 3 mal 0x00 des 1. Feldes: 0000 Startadresse 00=alles
	QFrankLesegeraet::Rueckgabecodes Ergebnis=Leser->ISO_ReadBinary(QByteArray(3,0x00),DatenDerKarte);
	if(Ergebnis!=QFrankLesegeraet::CommandSuccessful && Ergebnis!=QFrankLesegeraet::WarningEOFbeforeLeBytes)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"KVK Karte: Read Binary gescheitert";
#endif
		Leser->KarteEntfernen();
		return false;
	}
	//So die Karte wird jetzt nicht mehr benötigt.
	Leser->KarteEntfernen();
	//Auswertung
	//Haben wir eine KVK??  Prüfung nur nötig, wenn die Rohdaten gelesen wurden
	if(!KVKLeserWert)
	{
		if(!IstEsEineKVK(DatenDerKarte))
			return false;
	}
	if(!TagFinden(QFrankKVK::TAG_Datenanfang,DatenDerKarte) || !TagFinden(QFrankKVK::TAG_Kassenname,DatenDerKarte) ||
	   !TagFinden(QFrankKVK::TAG_Kassennummer,DatenDerKarte) || !TagFinden(QFrankKVK::TAG_Versichertennummer,DatenDerKarte) ||
	   !TagFinden(QFrankKVK::TAG_Versichertenstatus,DatenDerKarte) || !TagFinden(QFrankKVK::TAG_Titel,DatenDerKarte,true) ||
	   !TagFinden(QFrankKVK::TAG_Vorname,DatenDerKarte,true) || !TagFinden(QFrankKVK::TAG_Namenszusatz,DatenDerKarte,true) ||
	   !TagFinden(QFrankKVK::TAG_Nachname,DatenDerKarte) || !TagFinden(QFrankKVK::TAG_Geburtsdatum,DatenDerKarte) ||
	   !TagFinden(QFrankKVK::TAG_Strasse,DatenDerKarte,true) || !TagFinden(QFrankKVK::TAG_Land,DatenDerKarte,true) ||
	   !TagFinden(QFrankKVK::TAG_PLZ,DatenDerKarte) || !TagFinden(QFrankKVK::TAG_Ort,DatenDerKarte) ||
	   !TagFinden(QFrankKVK::TAG_GueltigBis,DatenDerKarte,true) || !TagFinden(QFrankKVK::TAG_WOP,DatenDerKarte,true) ||
	   !TagFinden(QFrankKVK::TAG_Statusergaenzung,DatenDerKarte,true) || !PruefsummeOK(DatenDerKarte))
		return false;
	return true;
}

bool QFrankKVK::TagFinden(QFrankKVK::TAGS welches,QByteArray &daten,bool optional)
{
	int Position=daten.indexOf(welches);
	if(Position==-1)
	{
		//TAG nicht gefunden wenn Land fehlt, dann D
		//wenn es optional ist, dann macht das nix
		switch(welches)
		{
			case QFrankKVK::TAG_Land:
										LandWert="D";
										return true;
										break;
			default:
										if(optional)
											return true;
#ifndef QT_NO_DEBUG
												qDebug()<<QString("KVK Karte: Ein nicht optionaler Eintrag wurde nicht gefunden\r\n"\
																  "gesucht wurde TAG: 0x%1").arg((int)welches,0,16);
#endif
										return false;
										break;
		}
	}
	QString WertDesFeldes=FeldNachQString(daten.mid(Position+2,(uchar)daten.at(Position+1)));
	QString GueltigBisJahr;
	switch (welches)
	{
		case QFrankKVK::TAG_Datenanfang:
											//Entsorgen der unnützen Infos				
											daten.remove(0,Position);
#ifndef QT_NO_DEBUG
											qDebug()<<"KVK Karte: Patientendaten"<<FeldNachHex(daten);
#endif		
											break;
		case QFrankKVK::TAG_Kassenname:
											KrankenkassennameWert=WertDesFeldes;
											break;
		case QFrankKVK::TAG_Kassennummer:
											KrankenkassennummerWert=WertDesFeldes.toULong();
											break;
		case QFrankKVK::TAG_Versichertennummer:
											VersichertennummerWert=WertDesFeldes.toULongLong();
											break;
		case QFrankKVK::TAG_Versichertenstatus:
											VersichertenstatusWert=WertDesFeldes.toUInt();
											break;
		case QFrankKVK::TAG_Titel:
											TitelWert=WertDesFeldes;
											break;
		case QFrankKVK::TAG_Vorname:
											VornameWert=WertDesFeldes;
											break;
		case QFrankKVK::TAG_Namenszusatz:
											NamenszusatzWert=WertDesFeldes;
											break;
		case QFrankKVK::TAG_Nachname:
											NachnameWert=WertDesFeldes;
											break;
		case QFrankKVK::TAG_Geburtsdatum:
											GeburtsdatumWert=QDate::fromString(QString(
															 "%1%2%3%4-%5%6-%7%8").arg(
															 WertDesFeldes.at(4)).arg(
															 WertDesFeldes.at(5)).arg(
															 WertDesFeldes.at(6)).arg(
															 WertDesFeldes.at(7)).arg(
															 WertDesFeldes.at(2)).arg(
															 WertDesFeldes.at(3)).arg(
															 WertDesFeldes.at(0)).arg(
															 WertDesFeldes.at(1)),Qt::ISODate);
											break;
		case QFrankKVK::TAG_Strasse:
											StrasseWert=WertDesFeldes;
											break;
		case QFrankKVK::TAG_Land:
											LandWert=WertDesFeldes;
											break;
		case QFrankKVK::TAG_PLZ:
											PLZWert=WertDesFeldes;
											break;
		case QFrankKVK::TAG_Ort:
											OrtWert=WertDesFeldes;
											break;
		case QFrankKVK::TAG_GueltigBis:
											//mm <93 ab Jahr 2000
											if(WertDesFeldes.at(2).digitValue()<9)
											{
												GueltigBisJahr="20";
											}
											else
											{
												GueltigBisJahr="19";
											}
											GueltigBisWert=QDate::fromString(
											QString("%1%2%3-%4%5-01").arg(GueltigBisJahr).arg(
											WertDesFeldes.at(2)).arg(WertDesFeldes.at(3))
											.arg(WertDesFeldes.at(0)).arg(WertDesFeldes.at(1)),
											Qt::ISODate);											
											break;
		case QFrankKVK::TAG_WOP:
											WOPWert=WertDesFeldes.toUInt();
											break;
		case QFrankKVK::TAG_Statusergaenzung:
											StatusergaenzungWert=WertDesFeldes.at(0);
											break;
		default:
											qFatal("KVK Karte: unbekannter Wert TagFinden!!!!");
											break;
	}
	return true;
}

QString	QFrankKVK::FeldNachQString(QByteArray feld)
{
	QString alles;
	char Zeichen;
	Q_FOREACH(Zeichen,feld)
	{
		alles.append(Ersetzen(Zeichen));
	}
	return alles;
}

QChar QFrankKVK::Ersetzen(char was)
{
	//Passt die Code an UTF an
	switch(was)
	{
		case 0x5c:
				//Ö
				return QChar(0xd6);
				break;
		case 0x5b:
				//Ä
				return QChar(0xc4);
				break;
		case 0x5d:
				//Ü
				return QChar(0xdc);
				break;
		case 0x7b:
				//ä
				return QChar(0xe4);
				break;
		case 0x7c:
				//ö
				return QChar(0xf6);
				break;
		case 0x7d:
				//ü
				return QChar(0xfc);
				break;
		case 0x7e:
				//ß
				return QChar(0xdf);
				break;
	}
	return QChar(was);
}

bool QFrankKVK::PruefsummeOK(QByteArray daten)
{
	//wo befindet sich die 0x60??
	int Start=daten.indexOf(0x60);
	// Ende des Prüfblocks bei 0x8e -1 vermutlich:)
	int Ende=daten.lastIndexOf(0x8e)+1;
	unsigned char Pruefcode=1;
	for(int x=Start;x<Ende;x++)
	{
		Pruefcode=Pruefcode^daten.at(x);	
	}
	if(daten.at(Ende+1)!=Pruefcode)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"KVK Karte: Prüfsumme der Karte stimmt nicht";
		qDebug()<<QString("Soll: %1 Ist: %2").arg((uchar)daten.at(Ende+1),0,16).arg(Pruefcode,0,16);
#endif
		return false;
	}
	return true;

}

bool QFrankKVK::IstEsEineKVK(QByteArray &daten)
{
	QByteArray KarteAlt(6,0x00);
	KarteAlt[0]=0xd2;
	KarteAlt[1]=0x80;
	KarteAlt[4]=0x01;
	KarteAlt[5]=0x01;
	QByteArray KarteNeu(6,0x00);
	KarteNeu=KarteAlt;
	KarteNeu[1]=0x76;
	//neue Karte??
	if(!daten.contains(KarteNeu))
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"KVK Karte: Prüfung auf neue Karte gescheitert, teste auf alte Karte";
#endif
		if(!daten.contains(KarteAlt))
		{
#ifndef QT_NO_DEBUG
			qDebug()<<"KVK Karte: Prüfung auf alte Karte gescheitert";
#endif
			return false;
		}
	}
	return true;
}

void QFrankKVK::welchenLeser(QFrankLesegeraet *diesen)
{
	Leser=diesen;
	if(diesen->LeserInitialisieren()!=QFrankLesegeraet::CommandSuccessful)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"KVK Karte: Leser initialisieren gescheitert.";
#endif
		Leser=0;
		return;
	}	
}

#ifndef QT_NO_DEBUG
QString	QFrankKVK::FeldNachHex(QByteArray feld)
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
#endif
