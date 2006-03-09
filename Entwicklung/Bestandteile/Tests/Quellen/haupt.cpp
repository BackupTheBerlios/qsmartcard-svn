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

int main(int argc, char *argv[])
{
	QCoreApplication Programm(argc,argv);
	QFrankTerminal *Terminal=new QFrankTerminal(&Programm);
	qDebug()<<"Es wurden folgende Geräte Plug-In's gefunden:\r\n"<<Terminal->ListeDerLeser().join("\r\n");
	qDebug()<<"Es wurden folgende SmartCard Plug-In's gefunden:\r\n"<<Terminal->ListeDerKarten().join("\r\n");
	
	
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
	delete Terminal;
	return 0;
	return Programm.exec();
}
