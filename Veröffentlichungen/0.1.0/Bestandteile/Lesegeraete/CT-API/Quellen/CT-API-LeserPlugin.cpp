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

#include "CT-API-Leser.h"
#include <Lesegeraetinterface.h>
#include <LesegeraetPlugin.h>
#include <QtCore>

class QFrankCT_API_LeserPlugin : public QFrankLesegeraetPlugin
{
	public:
			QFrankCT_API_LeserPlugin();
			QFrankLesegeraet*	erstellen(const QString &);
			QStringList			Geraete() const;
};

QFrankCT_API_LeserPlugin::QFrankCT_API_LeserPlugin() : QFrankLesegeraetPlugin()
{
}

QFrankLesegeraet* QFrankCT_API_LeserPlugin::erstellen(const QString &name)
{
	if(name=="CT-API-Leser")
	{
		QFrankCT_API_Leser* Leser=new QFrankCT_API_Leser(0);
		return Leser;
	}
	return 0;
}

QStringList QFrankCT_API_LeserPlugin::Geraete() const
{
	return QStringList()<<"CT-API-Leser";
}

Q_EXPORT_STATIC_PLUGIN(QFrankCT_API_LeserPlugin)
Q_EXPORT_PLUGIN2(ctapileserplugin, QFrankCT_API_LeserPlugin)
