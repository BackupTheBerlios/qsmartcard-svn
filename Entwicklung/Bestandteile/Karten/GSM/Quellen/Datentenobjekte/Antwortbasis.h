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

#ifndef QFRANKGSMKARTEANTWORTBASIS
#define QFRANKGSMKARTEANTWORTBASIS

#include <QtCore>

class QFrankGSMKarteAntwortbasis:public QObject
{
	Q_OBJECT
	
	public:
				QFrankGSMKarteAntwortbasis(QObject* eltern);
				enum						Dateityp{RFU=0x00,MF=0x01,DF=0x02,EF=0x04};
				Q_DECLARE_FLAGS(Dateitypen,Dateityp)
				const uint&					DateiID() const;
				const QFrankGSMKarteAntwortbasis::Dateitypen&	Dateiart() const;
				void						DateiIDSetzen(const uint &id);
				void						DateiartSetzen(const QFrankGSMKarteAntwortbasis::Dateitypen &art);

	protected:
				uint						K_DateiID;
				QFrankGSMKarteAntwortbasis::Dateitypen		K_Dateiart;	
};

#endif
