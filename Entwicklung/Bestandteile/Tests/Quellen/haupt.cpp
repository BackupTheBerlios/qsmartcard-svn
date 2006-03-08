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

	//Was hat das Lesegerät füt Sonderfunktionen
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

	//Verbinden des Dummylesers mit der Dummykarte
	Terminal->KarteHohlen("Dummy Karte1")->welchenLeser(Terminal->LeserHohlen("Dummyleser1"));
	delete Terminal;
	return 0;
	return Programm.exec();
}
