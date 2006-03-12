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

#ifndef QFRANKCTAPILESER_H
#define QFRANKCTAPILESER_H

#include <QtCore>
#include <Lesegeraet.h>

//XXYYZZ XX=Major YY=Minor ZZ=Patch
#define CT_API_Version 0x000100

class QFrankCT_API_Leser: public QFrankLesegeraet
{
	Q_OBJECT
	

	public:
			QFrankCT_API_Leser(QObject* eltern);
			QFrankLesegeraet::Rueckgabecodes	ISO_SelectFile(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_ReadBinary(QByteArray datenfeld,QByteArray &Zielfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_UpdateBinary(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_Verify(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_ChangeReferenceData(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_VerifySecure(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_ChangeReferenceDataSecure(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	KarteAnfordern(QByteArray &ATR);
			QFrankLesegeraet::Leserklasse		Sicherheitsklasse();
			ulong								Version();

	private:			
			
};
#endif
