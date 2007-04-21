/*
 *  Copyright (C) 2005-2007 Frank Büttner frank-buettner@gmx.net
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

#include "Signatur.h"
#include <QtCore>

using namespace QFrank;

bool CT_API_Manipulationsschutz::BibliothekOK(const QString &datei)
{
	QProcess MD5;
	MD5.setProcessChannelMode(QProcess::MergedChannels);
	MD5.start("/usr/sbin/prelink",QStringList()<<"--md5"<<"-y"<<datei);
	if((!MD5.waitForFinished()) || (MD5.exitCode()!=0))
	{
#ifndef QT_NO_DEBUG
		qDebug("Die MD5 Prüfsumme für die Datei %s konnte nicht ermittelt werden.",qPrintable(datei));
		qDebug("Ursache: %s",MD5.readAll().data());
#endif
		return false;
	}
	QString Pruefsumme=QString(MD5.readAll().data());
	Pruefsumme=Pruefsumme.left(Pruefsumme.indexOf(" "));
#ifndef QT_NO_DEBUG
	qDebug("Prüfsumme der Datei %s: %s",qPrintable(datei),qPrintable(Pruefsumme));
#endif
	QProcess Pruefung;
	Pruefung.setProcessChannelMode(QProcess::MergedChannels);
	Pruefung.start("rpm",QStringList() <<"-q"<<"--fileid"<<qPrintable(Pruefsumme));
	if(!Pruefung.waitForStarted())
	{
#ifndef QT_NO_DEBUG
		qDebug("Die Überprüfung der Datei %s konnte nicht gestartet werden.",qPrintable(datei));
#endif
		return false;
	}
	// warten bis es fertig ist
	if(!Pruefung.waitForFinished())
	{
#ifndef QT_NO_DEBUG
		qDebug("Die Überprüfung der Datei %s konnte nicht abgeschlossen werden.",qPrintable(datei));
#endif
		return false;
	}
#ifndef QT_NO_DEBUG
	qDebug("Die Überprüfung der Datei %s Ergab: %i Text: %s",qPrintable(datei),Pruefung.exitCode(),
								 Pruefung.readAll().data());
#endif
	if(Pruefung.exitCode()==0)
		return true;
	return false;
}
