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

#ifndef QFRANKSMARTCARD_H
#define QFRANKSMARTCARD_H

//XXYYZZ XX=Major YY=Minor ZZ=Patch
#define SmartCardAPI_Version 0x000100

#include <QtCore>

class QFrankLesegeraet;
class QFrankSmartCard: public QObject
{
	Q_OBJECT
	public:
			QFrankSmartCard(QObject* eltern);
			virtual ulong Version()=0;
			virtual void  welchenLeser(QFrankLesegeraet *diesen)=0;

};
#endif
