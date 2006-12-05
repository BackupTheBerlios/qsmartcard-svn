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

#include "Dummykarte.h"
#include <Karteninterface.h>
#include <SmartCardPlugin.h>
#include <QtCore>

class QFrankDummykartePlugin : public QFrankSmartCardPlugin
{
	public:
			QFrankDummykartePlugin();
			QFrankSmartCard* erstellen(const QString &);
			QStringList		 Karten() const;
};

QFrankDummykartePlugin::QFrankDummykartePlugin() : QFrankSmartCardPlugin()
{
}

QFrankSmartCard* QFrankDummykartePlugin::erstellen(const QString &name)
{
	if(name=="Dummy Karte1")
	{
		QFrankDummykarte* Karte=new QFrankDummykarte(0);
		return Karte;
	}
	return 0;
}

QStringList QFrankDummykartePlugin::Karten() const
{
	return QStringList()<<"Dummy Karte1";
}

Q_EXPORT_STATIC_PLUGIN(QFrankDummykartePlugin)
Q_EXPORT_PLUGIN2(dummykartenplugin, QFrankDummykartePlugin)
