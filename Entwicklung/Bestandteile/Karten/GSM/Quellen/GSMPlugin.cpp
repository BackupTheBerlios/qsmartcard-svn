/*
 *  Copyright (C) 2006 Frank Büttner frank-buettner@gmx.net
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

#include "GSM.h"
#include <Karteninterface.h>
#include <SmartCardPlugin.h>
#include <QtCore>

class QFrankGSMKartePlugin : public QFrankSmartCardPlugin
{
	public:
			QFrankGSMKartePlugin();
			QFrankSmartCard* erstellen(const QString &);
			QStringList	 Karten() const;
};

QFrankGSMKartePlugin::QFrankGSMKartePlugin() : QFrankSmartCardPlugin()
{
}

QFrankSmartCard* QFrankGSMKartePlugin::erstellen(const QString &name)
{
	if(name=="GSM Karte")
	{
		QFrankGSMKarte* Karte=new QFrankGSMKarte(0);
		return Karte;
	}
	return 0;
}

QStringList QFrankGSMKartePlugin::Karten() const
{
	return QStringList()<<"GSM Karte";
}

Q_EXPORT_STATIC_PLUGIN(QFrankGSMKartePlugin)
Q_EXPORT_PLUGIN2(gsmkarteplugin, QFrankGSMKartePlugin)
