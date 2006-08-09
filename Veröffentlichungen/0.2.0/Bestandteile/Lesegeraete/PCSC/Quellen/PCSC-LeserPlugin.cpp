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

#include "PCSC-Leser.h"
#include <Lesegeraetinterface.h>
#include <LesegeraetPlugin.h>
#include <QtCore>

class QFrankPCSC_LeserPlugin : public QFrankLesegeraetPlugin
{
	public:
			QFrankPCSC_LeserPlugin();
			QFrankLesegeraet*	erstellen(const QString &);
			QStringList			Geraete() const;
};

QFrankPCSC_LeserPlugin::QFrankPCSC_LeserPlugin() : QFrankLesegeraetPlugin()
{
}

QFrankLesegeraet* QFrankPCSC_LeserPlugin::erstellen(const QString &name)
{
	if(name=="PCSC-Leser")
	{
		QFrankPCSC_Leser* Leser=new QFrankPCSC_Leser(0);
		return Leser;
	}
	return 0;
}

QStringList QFrankPCSC_LeserPlugin::Geraete() const
{
	return QStringList()<<"PCSC-Leser";
}

Q_EXPORT_STATIC_PLUGIN(QFrankPCSC_LeserPlugin)
Q_EXPORT_PLUGIN2(pcscleserplugin, QFrankPCSC_LeserPlugin)
