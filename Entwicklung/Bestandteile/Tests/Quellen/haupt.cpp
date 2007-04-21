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
 *
 */

#include <QtCore>
#include <Terminal.h>

#include <cstdio>

int main(int argc, char *argv[])
{
	QCoreApplication Programm(argc,argv);
		
	QFrankTerminal *Terminal=new QFrankTerminal(&Programm);
	qDebug()<<"Es wurden folgende Geräte Plug-In's gefunden:\r\n"<<Terminal->ListeDerLeser().join("\r\n");
	qDebug()<<"Es wurden folgende SmartCard Plug-In's gefunden:\r\n"<<Terminal->ListeDerKarten().join("\r\n");
	
	
	/*
	qDebug()<<"WICHTIG KEINE KARTE IN DAS LESEGERÄT EINLEGEN AUCH NICHT WENN DAS GERÄT SIE AUFFORDERT!!";
	qDebug()<<"Eingabe drücken, zum fortfahren";
	std::getchar();

	//Was kann die Karte alles??
	foreach(QString Karte,Terminal->ListeDerKarten())
	{
		qDebug()<<"Die Karte"<<Karte<<"stellt folgende Funktionen bereit:";
		//alle Eigenschalften auflisten
		for(int x=1;x<((QObject *)Terminal->KarteHohlen(Karte))->metaObject()->propertyCount();x++)
		{
			QString tmp=((QObject *)Terminal->KarteHohlen(Karte))->metaObject()->property(x).name();
			if(tmp.startsWith(((QObject *)Terminal->KarteHohlen(Karte))->objectName()))
				qDebug()<<tmp<<"mit dem Datentyp"<<
						((QObject *)Terminal->KarteHohlen(Karte))->metaObject()->property(x).typeName();
		}
		
	}

	//Was hat das Lesegerät für Sonderfunktionen
	foreach(QString Leser,Terminal->ListeDerLeser())
	{
		qDebug()<<"Das Lesegerät"<<Leser<<"hat folgende Sonderfunktionen:";
		//Alle Sonderfunktionen auflisten
		for(int x=1;x<((QObject *)Terminal->LeserHohlen(Leser))->metaObject()->propertyCount();x++)
		{
			QString tmp=((QObject *)Terminal->LeserHohlen(Leser))->metaObject()->property(x).name();
			if(tmp.startsWith(((QObject *)Terminal->LeserHohlen(Leser))->objectName()))
				qDebug()<<tmp<<"mit dem Datentyp"<<
						((QObject *)Terminal->LeserHohlen(Leser))->metaObject()->property(x).typeName();
		}
	}

	//tests für den Dummyleser
	qDebug()<<"Tests für den Dummyleser";
	QByteArray Testfeld(5,0x00);
	QByteArray Zielfeld(10,0x77);
	Testfeld[0]=0x04;
	Testfeld[1]=0x00;
	Testfeld[2]=0x00;
	Testfeld[3]=0x00;
	Testfeld[4]=0x06;
	//((QObject*)Terminal->LeserHohlen("Dummyleser1"))->setProperty("QFrankDummyleserISO_SelectFileFehler",true);
	Terminal->LeserHohlen("Dummyleser1")->ISO_SelectFile(Testfeld);
	((QObject*)Terminal->LeserHohlen("Dummyleser1"))->setProperty("QFrankDummyleserISO_ReadBinaryDaten",QByteArray(7,0x55));
	Terminal->LeserHohlen("Dummyleser1")->ISO_ReadBinary(Testfeld,Zielfeld);
	for (int x=0;x<Zielfeld.size();x++)
	{
		qDebug()<<QString("%1").arg((uchar)Zielfeld.at(x),0,16);
	}

	
	QByteArray Schreibfeld(4,0x55);
	Schreibfeld[0]=0x00;
	Schreibfeld[1]=0x01;
	Schreibfeld[2]=0x01;
	Terminal->LeserHohlen("Dummyleser1")->ISO_UpdateBinary(Schreibfeld);

	QByteArray Pinfeld(6,0xff);
	Pinfeld[0]=0x00;
	Pinfeld[1]=0x00;
	Pinfeld[2]=0x03;
	Pinfeld[3]=0x12;
	Pinfeld[4]=0x34;
	Pinfeld[5]=0xf6;
	Terminal->LeserHohlen("Dummyleser1")->ISO_Verify(Pinfeld);

	QByteArray PinAendernfeld(9,0xff);
	PinAendernfeld[0]=0x00;
	PinAendernfeld[1]=0x00;
	PinAendernfeld[2]=0x06;
	PinAendernfeld[3]=0x12;
	PinAendernfeld[4]=0x34;
	PinAendernfeld[5]=0xf6;
	PinAendernfeld[6]=0x6a;
	Terminal->LeserHohlen("Dummyleser1")->ISO_ChangeReferenceData(PinAendernfeld);

	Pinfeld.resize(2);
	Pinfeld[0]=0x67;
	Pinfeld[1]=0x89;
	((QObject*)Terminal->LeserHohlen("Dummyleser1"))->setProperty("QFrankDummyleserSicherheitsklasse",QFrankLesegeraet::Klasse2);
	((QObject*)Terminal->LeserHohlen("Dummyleser1"))->setProperty("QFrankDummyleserISO_VerifySecurePin",Pinfeld);
	Testfeld.resize(11);
	Testfeld[0]=0x02;
	Testfeld[1]=0x00;
	Testfeld[2]=0x08; //Länge Feld
	Testfeld[3]=0x52;
	Testfeld[4]=0x06; //Länge am hier
	Testfeld[5]=0x50; //Pin hat 5 Stellen und BCD Kodiert
	Testfeld[6]=0x06; //Einfügen nach Pos 6 des Befehls
	Testfeld[7]=0x00; //Ab hier Kartenbefehl
	Testfeld[8]=0x20;
	Testfeld[9]=0x00;
	Testfeld[10]=0x00;
	Terminal->LeserHohlen("Dummyleser1")->ISO_VerifySecure(Testfeld);

	Testfeld.resize(12);
	Testfeld[2]=0x09; //Länge Feld
	Testfeld[4]=0x07; //Länge am hier
	Testfeld[6]=0x07; //Positions alte Pin(5 Stellig)
	Testfeld[7]=0x0b; //Postition neue Pin 00 für wahlfreie Länge der neuen Pin
	Testfeld[8]=0x00; //An hier Kartenbefehl
	Testfeld[9]=0x24;
	Testfeld[10]=0x00;
	Testfeld[11]=0x00;
	Pinfeld.resize(3);
	Pinfeld[2]=0x0f;
	QByteArray PinfeldNeu(2,0xff);
	PinfeldNeu[0]=0x12;
	PinfeldNeu[1]=0x3f;
	((QObject*)Terminal->LeserHohlen("Dummyleser1"))->setProperty("QFrankDummyleserSicherheitsklasse",QFrankLesegeraet::Klasse3);
	((QObject*)Terminal->LeserHohlen("Dummyleser1"))->setProperty("QFrankDummyleserISO_ChangeReferenceDataSecurePinAlt",Pinfeld);
	((QObject*)Terminal->LeserHohlen("Dummyleser1"))->setProperty("QFrankDummyleserISO_ChangeReferenceDataSecurePinNeu",PinfeldNeu);
	Terminal->LeserHohlen("Dummyleser1")->ISO_ChangeReferenceDataSecure(Testfeld);

	Terminal->LeserHohlen("Dummyleser1")->KarteAnfordern(Testfeld);
	

	//Verbinden des Dummylesers mit der Dummykarte
	Terminal->KarteHohlen("Dummy Karte1")->welchenLeser(Terminal->LeserHohlen("Dummyleser1"));

	//Test CT-API Treiber
	qDebug()<<QString("0x%1").arg(Terminal->LeserHohlen("CT-API-Leser")->LeserInitialisieren(),0,16);
	qDebug()<<QString("0x%1").arg(Terminal->LeserHohlen("CT-API-Leser")->Sicherheitsklasse(),0,16);
	qDebug()<<QString("0x%1").arg(Terminal->LeserHohlen("CT-API-Leser")->KarteAnfordern(Testfeld),0,16);
	Testfeld.resize(5);
	Testfeld[0]=0x04;
	Testfeld[1]=0x00;
	Testfeld[2]=0x02;
	Testfeld[3]=0x3f;
	Testfeld[4]=0x00;
	qDebug()<<QString("0x%1").arg(Terminal->LeserHohlen("CT-API-Leser")->ISO_SelectFile(Testfeld),0,16);
	Testfeld.resize(3);
	Testfeld[0]=0x00;
	Testfeld[1]=0x00;
	Testfeld[2]=0x00;
	qDebug()<<QString("0x%1").arg(Terminal->LeserHohlen("CT-API-Leser")->ISO_ReadBinary(Testfeld,Testfeld),0,16);
	
	Testfeld.resize(5);
	Testfeld[0]=0x00;
	Testfeld[1]=0x00;
	Testfeld[2]=0x02; //Länge der Daten
	Testfeld[3]=0x0d;
	Testfeld[4]=0x00;
	qDebug()<<QString("0x%1").arg(Terminal->LeserHohlen("CT-API-Leser")->ISO_UpdateBinary(Testfeld),0,16);
	
	Testfeld.resize(2);
	Testfeld[0]=0x12;
	Testfeld[1]=0x34;
	qDebug()<<QString("0x%1").arg(Terminal->LeserHohlen("CT-API-Leser")->ISO_Verify(Testfeld),0,16);
	
	qDebug()<<QString("0x%1").arg(Terminal->LeserHohlen("CT-API-Leser")->ISO_ChangeReferenceData(Testfeld),0,16);

	Testfeld.resize(8);
	Testfeld[0]=0x52;
	Testfeld[1]=0x06;//Länge des ganzens
	Testfeld[2]=0x00; //Pin beligig lang und bcd
	Testfeld[3]=0x06; //Pin ab Postion 6 einfügen 5 wird Lc
	Testfeld[4]=0x00;
	Testfeld[5]=0x20;
	Testfeld[6]=0x00;
	Testfeld[7]=0x00;
	qDebug()<<QString("0x%1").arg(Terminal->LeserHohlen("CT-API-Leser")->ISO_VerifySecure(Testfeld),0,16);

	Testfeld.resize(9);
	Testfeld[0]=0x52;
	Testfeld[1]=0x07;//Länge des ganzens
	Testfeld[2]=0x00; //Pin alt beligig lang und bcd
	Testfeld[3]=0x06; //Ab Position 6  wird Lc
	Testfeld[4]=0x00; //neue Pin bliebig lang
	Testfeld[5]=0x00;
	Testfeld[6]=0x20;
	Testfeld[7]=0x00;
	Testfeld[8]=0x00;
	qDebug()<<QString("0x%1").arg(Terminal->LeserHohlen("CT-API-Leser")->ISO_ChangeReferenceDataSecure(Testfeld),0,16);
	qDebug()<<QString("0x%1").arg(Terminal->LeserHohlen("CT-API-Leser")->KarteEntfernen(),0,16);

	//Test für KVK 
	qDebug()<<"An dieser Stelle kann jetzte sicher eine KVK Karte eingelegt werden. Um diese auszulesen";
	qDebug()<<"Eingabe drücken, zum fortfahren";
	std::getchar();*/
	/*
	//((QObject*)Terminal->KarteHohlen("KVK Karte"))->setProperty("QFrankKVKKVKLeser",true);
		

	/*
	Terminal->KarteHohlen("KVK Karte")->welchenLeser(Terminal->LeserHohlen("CT-API-Leser"));
	if(((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKAuslesen").toBool())
	{
		qDebug()<<"Name der Kasse:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKKrankenkassenname").toString();
		qDebug()<<"Nummer der Kasse:"<<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKKrankenkassennummer").toULongLong();
		qDebug()<<"Nummer des Versicherten:"<<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKVersichertennummer").toULongLong();
		qDebug()<<"Status des Versicherten:"<<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKVersichertenstatus").toUInt();
		qDebug()<<"Titel:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKTitel").toString();
		qDebug()<<"Vorname:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKVorname").toString();
		qDebug()<<"Namenszusatz:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKNamenszusatz").toString();
		qDebug()<<"Nachname:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKNachname").toString();
		qDebug()<<"Geburtsdatum:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKGeburtsdatum").toDate().toString("dd.MM.yyyy");
		qDebug()<<"Straße:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKStrasse").toString();
		qDebug()<<"Postleitzahl:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKPLZ").toString();
		qDebug()<<"Ort:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKOrt").toString();
		qDebug()<<"Land:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKLand").toString();
		qDebug()<<"Karte Gültig bis:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKGueltigBis").toDate().toString("MM.yyyy");
		qDebug()<<"WOP/VKNR:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKWOP").toUInt();
		qDebug()<<"Statusergänzung:" <<((QObject*)Terminal->KarteHohlen("KVK Karte"))->property("QFrankKVKStatusergaenzung").toChar();

	}
	else
	{
		qDebug()<<"KVK konnte nicht ausgelesen werden:(";
	}
	*/


	//test PC/SC
	/*Terminal->LeserHohlen("PCSC-Leser")->LeserInitialisieren();
	QByteArray ATR;
	Terminal->LeserHohlen("PCSC-Leser")->KarteAnfordern(ATR);*/
	
	//test für GSM Karten
	
#ifndef Q_WS_WIN
	Terminal->LeserHohlen("CT-API-Leser")->setProperty("QFrankCT_API_LeserTreiberdatei","/usr/lib/ctapi/libctapi-cyberjack.so");	
#else
	//Terminal->LeserHohlen("CT-API-Leser")->setProperty("QFrankCT_API_LeserSignaturDerBibliothekPruefen",false);
#endif
	Terminal->KarteHohlen("GSM Karte")->welchenLeser(Terminal->LeserHohlen("CT-API-Leser"));
	//richtige Karte im Leser??
	if(!((QObject*)Terminal->KarteHohlen("GSM Karte"))->property("QFrankGSMKarteAktivieren").toBool())
		qDebug()<<Terminal->KarteHohlen("GSM Karte")->property("QFrankGSMKarteFehlertext").toString();
	else
	{
		//Pin1 aktivieren bei Lesern >1 Eingabe über Leser sonst per Programm
		if(Terminal->LeserHohlen("CT-API-Leser")->Sicherheitsklasse()>QFrankLesegeraet::Klasse1)
		{
			((QObject*)Terminal->KarteHohlen("GSM Karte"))->setProperty("QFrankGSMKartePin1Eingabe",true);
			qDebug()<<"Bitte PIN1 an dem Leser eingeben.";
						
		}
		else
		{
			((QObject*)Terminal->KarteHohlen("GSM Karte"))->setProperty("QFrankGSMKartePin1Eingabe",false);
			qDebug()<<"Bitte PIN1 eingeben und die Eingabetaste drücken.";
			 QByteArray Pin;
			 QTextStream Eingabe(stdin);
			Eingabe >> Pin;
			((QObject*)Terminal->KarteHohlen("GSM Karte"))->setProperty("QFrankGSMKartePinUebermitteln",Pin);
			
		}	
		if(!((QObject*)Terminal->KarteHohlen("GSM Karte"))->property("QFrankGSMKartePin1Eingabe").toBool())
				qDebug()<<Terminal->KarteHohlen("GSM Karte")->property("QFrankGSMKarteFehlertext").toString();
		else
		{
			qDebug()<<"Seriennummer:"<<((QObject*)Terminal->KarteHohlen("GSM Karte"))->property("QFrankGSMKarteSeriennummer").toString();
			qDebug()<<"Mobilfunkanbieter:"<<((QObject*)Terminal->KarteHohlen("GSM Karte"))->property("QFrankGSMKarteAnbieter").toString();
			qDebug()<<"Kurzwahlnummern:"<<((QObject*)Terminal->KarteHohlen("GSM Karte"))->property("QFrankGSMKarteKurzwahlnummern").toString();
			qDebug()<<"Festwahlnummern:"<<((QObject*)Terminal->KarteHohlen("GSM Karte"))->property("QFrankGSMKarteFestwahlnummern").toString();
			qDebug()<<"Eigene Rufnummern:"<<((QObject*)Terminal->KarteHohlen("GSM Karte"))->property("QFrankGSMKarteMeineNummern").toString();
			qDebug()<<"Zuletzt gewählte Rufnummern:"<<((QObject*)Terminal->KarteHohlen("GSM Karte"))->property("QFrankGSMKarteZuletztGewaehlteNummern").toString();
		}
	}
	delete Terminal;
	return 0;
}
