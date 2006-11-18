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
#include <Lesegeraetinterface.h>
#include <SmartCard.h>
#include <Lesegeraet.h>


QFrankTerminal::QFrankTerminal(QObject *eltern,QString pluginVerzeichnis):QObject(eltern)
{
	setObjectName("QFrankTerminal");
	PlugInsLaden(QDir(pluginVerzeichnis));
}

QFrankTerminal::~QFrankTerminal()
{
	//Zu erste die Karten dann die Leser löschen mit sich die Karten trennen können
	QFrankLesegeraet *Leser;
	Q_FOREACH(Leser,TabelleLeser)
	{
		Leser->KarteEntfernen();
		delete Leser;
	}
	QFrankSmartCard *Karte;
	Q_FOREACH(Karte,TabelleKarten)
	{
		delete Karte;
	}
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

QFrankLesegeraet* QFrankTerminal::LeserHohlen(QString lesegeraet)
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
	
#ifndef QT_NO_DEBUG
	qDebug(qPrintable(QString("QFrankTerminal PlugInsLaden: Lade Plugins aus: %1").arg(pfad.absolutePath())));
#endif
	Q_FOREACH(QString Datei, pfad.entryList(QDir::Files))
	{
#ifndef QT_NO_DEBUG
		qDebug(qPrintable(trUtf8("QFrankTerminal PlugInsLaden: prüfe Datei %1","debug").arg(Datei)));
#endif
		QPluginLoader PlugInLader(pfad.absoluteFilePath(Datei));
		QObject *PlugIn = PlugInLader.instance();
		if (PlugIn)
		{
#ifndef QT_NO_DEBUG
			qDebug(qPrintable(QString("QFrankTerminal PlugInsLaden: %1 geladen").arg(Datei)));
#endif
			/*
				Datei konnte geladen werden.
				Nun wird geprüft, was es für ein PlugIn ist
			*/
			QFrankKarteninterface *Karte=qobject_cast<QFrankKarteninterface *>(PlugIn);
			if(Karte)
			{
#ifndef QT_NO_DEBUG
				qDebug(qPrintable(QString("QFrankTerminal PlugInsLaden: %1 ist ein Kartenplugin").arg(Datei)));
#endif
				//Jedes Bibliothek könnte mehrere Plug-Ins bereitstellen
			    Q_FOREACH(QString Pluginname,Karte->Karten())
				{
					//Neues Objekt anlegen und den Zeiger in die Liste:)
					TabelleKarten.insert(Pluginname,Karte->erstellen(Pluginname));
				}
			}
			QFrankLesegeraetinterface *Leser=qobject_cast<QFrankLesegeraetinterface *>(PlugIn);
			if(Leser)
			{
#ifndef QT_NO_DEBUG
				qDebug(qPrintable(trUtf8("QFrankTerminal PlugInsLaden: %1 ist ein Lesegerätplugin","debug").arg(Datei)));
#endif
				Q_FOREACH(QString Pluginname,Leser->Geraete())
				{
					TabelleLeser.insert(Pluginname,Leser->erstellen(Pluginname));
				}
			}
		}
	}
}
