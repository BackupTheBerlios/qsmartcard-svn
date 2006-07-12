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

#ifndef QFRANKGSMKARTE_H
#define QFRANKGSMKARTE_H

//XXYYZZ XX=Major YY=Minor ZZ=Patch
#define GSMKarteVersion 0x000100

#include <QtCore>
#include <SmartCard.h>

class QFrankGSMKarte: public QFrankSmartCard
{
	Q_OBJECT

	public:
			QFrankGSMKarte(QObject* eltern);
			ulong			Version();
			void			welchenLeser(QFrankLesegeraet *diesen);

	private:	
			QFrankLesegeraet*	K_Leser;
			bool			K_PIN1gesetzt;
			bool			K_PIN2gesetzt;
#ifndef QT_NO_DEBUG
			QString			FeldNachHex(const QByteArray &feld) const; 
#endif		
};
#endif
