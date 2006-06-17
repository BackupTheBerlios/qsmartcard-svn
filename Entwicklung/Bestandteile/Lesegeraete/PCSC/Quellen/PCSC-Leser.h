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

#ifndef QFRANKPCSCLESER_H
#define QFRANKPCSCLESER_H

#include <QtCore>
#include <Lesegeraet.h>

//XXYYZZ XX=Major YY=Minor ZZ=Patch
#define PCSC_Version 0x000100

class QFrankPCSC_Leser: public QFrankLesegeraet
{
	Q_OBJECT
	
	public:
			QFrankPCSC_Leser(QObject* eltern);
			~QFrankPCSC_Leser();
			QFrankLesegeraet::Rueckgabecodes	ISO_SelectFile(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_ReadBinary(QByteArray datenfeld,QByteArray &zielfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_UpdateBinary(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_Verify(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_ChangeReferenceData(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_VerifySecure(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_ChangeReferenceDataSecure(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	KarteAnfordern(QByteArray &ATR);
			QFrankLesegeraet::Rueckgabecodes	KarteEntfernen();
			QFrankLesegeraet::Leserklasse		Sicherheitsklasse();
			QFrankLesegeraet::Rueckgabecodes	LeserInitialisieren();
			ulong								Version();	
	private:
			QFrankLesegeraet::Leserklasse		Lesersicherheit;
#ifndef QT_NO_DEBUG
			QString								FeldNachHex(QByteArray feld);
#endif			
};
#endif
