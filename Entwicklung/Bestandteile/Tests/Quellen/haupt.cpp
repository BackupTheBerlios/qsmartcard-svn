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
	
	
	//Mit Extras suchen mit dem QtProperty System
	foreach(QString Karte,Terminal->ListeDerKarten())
	{
		qDebug()<<"Karte"<<Karte<<"hat folgende"<< ((QObject *)Terminal->KarteHohlen(Karte))->metaObject()->propertyCount() <<"Extras:";
		for (int i=0;i<((QObject *)Terminal->KarteHohlen(Karte))->metaObject()->propertyCount();i++)
		{
			qDebug()<<"Name:"<<((QObject *)Terminal->KarteHohlen(Karte))->metaObject()->property(i).name();
			qDebug()<<"Type"<<((QObject *)Terminal->KarteHohlen(Karte))->metaObject()->property(i).typeName();
			if(((QObject *)Terminal->KarteHohlen(Karte))->metaObject()->property(i).isReadable())
			{
				qDebug()<<"Der Wert ist lesbar und beinhaltet:"<<((QObject *)Terminal->KarteHohlen(Karte))->property(((QObject *)Terminal->KarteHohlen(Karte))->metaObject()->property(i).name());
			}
			
		}
	}

	
	delete Terminal;
	return 0;
	return Programm.exec();
}
