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

#ifndef QFRANKTERMINAL_H
#define QFRANKTERMINAL_H

#include <QtCore>

//der MS Compiler braucht Hilfe beim exportieren
#if defined(_WIN32) && !defined(__GNUC__)
	#ifdef DLL_BAUEN
		#define DLL_EXPORT __declspec(dllexport)
	#else
		#define DLL_EXPORT __declspec(dllimport)
	#endif
#else
	#define DLL_EXPORT
#endif

//class QFrankSmartCard;
//class QFrankLeser;

//Dummy nur zum testen anfang
class  DLL_EXPORT QFrankSmartCard
{
public:
QFrankSmartCard();
};
class DLL_EXPORT QFrankLeser
{
public:
QFrankLeser();
};
//Dummy nur zum testen ende

class DLL_EXPORT QFrankTerminal : public QObject
{
	Q_OBJECT
	public:
			QFrankTerminal(QObject *eltern=0,QString pluginVerzeichnis=QCoreApplication::applicationDirPath());
			QStringList						ListeDerKarten();
			QStringList						ListeDerLeser();
			QFrankSmartCard*				KarteHohlen(QString karte);
			QFrankLeser*					LeserHohlen(QString lesegeraet);

	private:
			QString							Pluginverzeichnis;
			bool							ArgumentLeer(QString &argument);
			QHash<QString,QFrankSmartCard*>	TabelleKarten;
			QHash<QString,QFrankLeser*>		TabelleLeser;
};
#endif
