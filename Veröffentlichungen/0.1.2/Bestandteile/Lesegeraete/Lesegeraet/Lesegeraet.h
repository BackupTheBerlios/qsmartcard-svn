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

#ifndef QFRANKLESEGERAET_H
#define QFRANKLESEGERAET_H

//XXYYZZ XX=Major YY=Minor ZZ=Patch
#define LesegeraetAPI_Version 0x000100

#include <QtCore>

class QFrankLesegeraet: public QObject
{
	Q_OBJECT
	public:
			enum Rueckgabecode
			{
				Error=0x6200,
				CardPresentAndActivated=0x6201,
				DataCorrupted=0x6281,
				WarningEOFbeforeLeBytes=0x6282,
				VerificationUnsuccessfullOneLeft=0x63c0,
				VerificationUnsuccessfullTwoLeft=0x63c1,
				VerificationUnsuccessfullThreeLeft=0x63c2,
				VerificationUnsuccessfullFourLeft=0x63c3,
				VerificationUnsuccessfullFifeLeft=0x63c4,
				VerificationUnsuccessfullSixLeft=0x63c5,
				VerificationUnsuccessfullSevenLeft=0x63c6,
				VerificationUnsuccessfullEightLeft=0x63c7,
				VerificationUnsuccessfullNineLeft=0x63c8,
				VerificationUnsuccessfullTenLeft=0x63c9,
				VerificationUnsuccessfullElevenLeft=0x63ca,
				VerificationUnsuccessfullTwelveLeft=0x63cb,
				VerificationUnsuccessfullThirteenLeft=0x63cc,
				VerificationUnsuccessfullFourteenLeft=0x63cd,
				VerificationUnsuccessfullFifteenLeft=0x63ce,
				VerificationUnsuccessfullSixteenLeft=0x63cf,
				ResetNotSuccessful=0x6400,
				CancelByCancelKey=0x6401,
				MemoryFailure=0x6501,
				CommandWithTimerNotSupported=0x6900,
				VerificationMethodBlocked=0x6983,
				FileNotFound=0x6a82,
				CommandSuccessful=0x9000,
				CommandSuccessfulAsynchron=0x9001,
				LeserNichtInitialisiert=0xfffe,
				ParameterFalsch=0xffff
			};
			enum Klasse
			{
				Klasse1=0x00,
				Klasse2=0x01,
				Klasse3=0x02,
				Klasse4=0x03,
				KlasseUnbekannt=0xff
			};
			Q_DECLARE_FLAGS(Rueckgabecodes, Rueckgabecode)
			Q_DECLARE_FLAGS(Leserklasse, Klasse)
			QFrankLesegeraet(QObject* eltern);
			virtual ulong								Version()=0;
			virtual QFrankLesegeraet::Rueckgabecodes	KarteAnfordern(QByteArray &ATR)=0;
			virtual QFrankLesegeraet::Rueckgabecodes	KarteEntfernen()=0;
			virtual QFrankLesegeraet::Rueckgabecodes	ISO_SelectFile(QByteArray datenfeld)=0;
			virtual QFrankLesegeraet::Rueckgabecodes	ISO_ReadBinary(QByteArray datenfeld,QByteArray &zielfeld)=0;
			virtual	QFrankLesegeraet::Rueckgabecodes	ISO_UpdateBinary(QByteArray datenfeld)=0;
			virtual	QFrankLesegeraet::Rueckgabecodes	ISO_Verify(QByteArray datenfeld)=0;
			virtual	QFrankLesegeraet::Rueckgabecodes	ISO_ChangeReferenceData(QByteArray datenfeld)=0;
			virtual	QFrankLesegeraet::Rueckgabecodes	ISO_VerifySecure(QByteArray datenfeld)=0;
			virtual	QFrankLesegeraet::Rueckgabecodes	ISO_ChangeReferenceDataSecure(QByteArray datenfeld)=0;
			virtual QFrankLesegeraet::Leserklasse		Sicherheitsklasse()=0;
			virtual QFrankLesegeraet::Rueckgabecodes	LeserInitialisieren()=0;

};
#endif
