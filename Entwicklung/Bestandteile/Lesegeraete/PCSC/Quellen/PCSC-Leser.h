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
#include <winscard.h>

//XXYYZZ XX=Major YY=Minor ZZ=Patch
#define PCSC_Version 0x000100
//Zum übersetzten wird min. Version 0.3.1 des Lesermodells benötigt.
#if LesegeraetAPI_Version < 0x000301
	#error Es wird min. Version 0.3.0 des Lesermodells benötigt.
#endif

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
			QFrankLesegeraet::Rueckgabecodes	ISO_VerifySecure(QByteArray datenfeld){return QFrankLesegeraet::NichtImplementiert;}
			QFrankLesegeraet::Rueckgabecodes	ISO_ChangeReferenceDataSecure(QByteArray datenfeld){return QFrankLesegeraet::NichtImplementiert;}
			QFrankLesegeraet::Rueckgabecodes	UniversalIO(const QByteArray &daten, QByteArray &antwort);
			QFrankLesegeraet::Rueckgabecodes	KarteAnfordern(QByteArray &ATR);
			QFrankLesegeraet::Rueckgabecodes	KarteEntfernen();
			QFrankLesegeraet::Leserklasse		Sicherheitsklasse();
			QFrankLesegeraet::Rueckgabecodes	LeserInitialisieren();
			QFrankLesegeraet::Rueckgabecodes	SicherePineingabe(const QByteArray &kartenbefehl){return QFrankLesegeraet::NichtImplementiert;}
			ulong								Version();	
	private:
			QFrankLesegeraet::Leserklasse		K_Lesersicherheit;
			SCARDCONTEXT						K_PCSC_Kontext;
			SCARDHANDLE							K_Kartenverbindung; 
			LONG								K_RueckegabePCSC;
			bool								K_VerbindungZumLeser;
#ifndef QT_NO_DEBUG
			QString								K_FeldNachHex(QByteArray feld);
#endif			
};
#endif
