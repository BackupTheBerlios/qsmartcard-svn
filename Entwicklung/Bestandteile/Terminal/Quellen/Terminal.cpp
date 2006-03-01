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

#include "Terminal.h"
#include <Karteninterface.h>
#include <SmartCard.h>

//Dummy nur zum testen anfang
QFrankLeser::QFrankLeser()
{
}
//Dummy nur zum testen ende

QFrankTerminal::QFrankTerminal(QObject *eltern,QString pluginVerzeichnis):QObject(eltern)
{
	setObjectName("QFrankTerminal");
	PlugInsLaden(QDir(pluginVerzeichnis));

	//Dummy Geräte
	TabelleLeser.insert("Dummy Leser1",new QFrankLeser());
	TabelleLeser.insert("Dumme Leser2",new QFrankLeser());
}

QFrankTerminal::~QFrankTerminal()
{
}

QStringList QFrankTerminal::ListeDerKarten()
{
	return TabelleKarten.keys();
}

QStringList QFrankTerminal::ListeDerLeser()
{
	return TabelleLeser.keys();
}

QFrankSmartCard* QFrankTerminal::KarteHohlen(QString karte)
{
	//Argument leer? oder nicht in der Liste
	if(ArgumentLeer(karte) | !TabelleKarten.contains(karte))
		return 0;
	return TabelleKarten.value(karte);
}

QFrankLeser* QFrankTerminal::LeserHohlen(QString lesegeraet)
{
	//Argument leer? oder nicht in der Liste
	if(ArgumentLeer(lesegeraet) | !TabelleLeser.contains(lesegeraet))
		return 0;
	return TabelleLeser.value(lesegeraet);
}

bool QFrankTerminal::ArgumentLeer(QString &argument)
{
	if(argument.isEmpty() | argument.isNull())
		return true;
	return false;
}

void QFrankTerminal::PlugInsLaden(QDir pfad)
{
	
#ifdef MEINDEBUG
	qDebug()<<"Lade Plugins aus:"<<pfad.absolutePath();
#endif
	foreach (QString Datei, pfad.entryList(QDir::Files))
	{
#ifdef MEINDEBUG
		qDebug()<<"prüfe Datei:"<<Datei;
#endif
		QPluginLoader PlugInLader(pfad.absoluteFilePath(Datei));
		QObject *PlugIn = PlugInLader.instance();
		if (PlugIn)
		{
#ifdef MEINDEBUG
			qDebug()<<Datei<<"geladen";
#endif
			/*
				Datei konnte geladen werden.
				Nun wird geprüft, was es für ein PlugIn ist
			*/
			QFrankKarteninterface *Karte=qobject_cast<QFrankKarteninterface *>(PlugIn);
			if(Karte)
			{
#ifdef MEINDEBUG
				qDebug()<<Datei<<"ist ein Kartenplugin";
#endif
				//Jedes Bibliothek könnte mehrere Plug-Ins bereitstellen
			    foreach (QString Pluginname,Karte->Karten())
				{
					//Neues Objekt anlegen und den Zeiger in die Liste:)
					TabelleKarten.insert(Pluginname,Karte->erstellen(Pluginname));
				}
			}
		}
	}
}
