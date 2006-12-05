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

#ifndef QFRANKDUMMYLESER_H
#define QFRANKDUMMYLESER_H

#include <QtCore>
#include <Lesegeraet.h>

//XXYYZZ XX=Major YY=Minor ZZ=Patch
#define DummyleserVersion 0x000201
//Zum übersetzten wird min. Version 0.2.0 des Lesermodells benötigt.
#if LesegeraetAPI_Version < 0x000200
#error Es wird min. Version 0.2.0 des Lesermodells benötigt.
#endif

class QFrankDummyleser: public QFrankLesegeraet
{
	Q_OBJECT
	Q_PROPERTY(bool QFrankDummyleserISO_SelectFileFehler READ ISO_SelectFileFehler WRITE ISO_SelectFileFehlerSetzen)
	Q_PROPERTY(QByteArray QFrankDummyleserISO_ReadBinaryDaten READ ISO_ReadBinaryDaten WRITE ISO_ReadBinaryDatenSetzen)
	Q_PROPERTY(ulong QFrankDummyleserISO_ReadBinaryStatuscode READ ISO_ReadBinaryStatuscode WRITE ISO_ReadBinaryStatuscodeSetzen)
	Q_PROPERTY(QByteArray QFrankDummyleserUniversalIODaten READ ISO_ReadBinaryDaten WRITE ISO_ReadBinaryDatenSetzen)
	Q_PROPERTY(ulong QFrankDummyleserUniversalIOStatuscode READ ISO_ReadBinaryStatuscode WRITE ISO_ReadBinaryStatuscodeSetzen)
	Q_PROPERTY(ulong QFrankDummyleserSicherheitsklasse READ Sicherheitsklasse WRITE SicherheitsklasseSetzen)
	Q_PROPERTY(ulong QFrankDummyleserISO_UpdateBinaryStatuscode READ ISO_UpdateBinaryStatuscode WRITE ISO_UpdateBinaryStatuscodeSetzen)
	Q_PROPERTY(ulong QFrankDummyleserISO_VerifyStatuscode READ ISO_VerifyStatuscode WRITE ISO_VerifyStatuscodeSetzen)
	Q_PROPERTY(ulong QFrankDummyleserISO_ChangeReferenceDataStatuscode READ ISO_ChangeReferenceDataStatuscode WRITE ISO_ChangeReferenceDataStatuscodeSetzen)
	Q_PROPERTY(QByteArray QFrankDummyleserISO_VerifySecurePin READ ISO_VerifySecurePin WRITE ISO_VerifySecurePinSetzen)
	Q_PROPERTY(ulong QFrankDummyleserISO_VerifySecureStatuscode READ ISO_VerifyStatuscode WRITE ISO_VerifyStatuscodeSetzen)
	Q_PROPERTY(QByteArray QFrankDummyleserISO_ChangeReferenceDataSecurePinAlt READ ISO_VerifySecurePin WRITE ISO_VerifySecurePinSetzen)
	Q_PROPERTY(QByteArray QFrankDummyleserISO_ChangeReferenceDataSecurePinNeu READ ISO_ChangeReferenceDataSecurePinNeu WRITE
			   ISO_ChangeReferenceDataSecurePinNeuSetzen)
	Q_PROPERTY(ulong QFrankDummyleserISO_ChangeReferenceDataSecureStatuscode READ ISO_ChangeReferenceDataStatuscode WRITE ISO_ChangeReferenceDataStatuscodeSetzen)
	Q_PROPERTY(ulong QFrankDummyleserKarteAnfordernStatuscode READ KarteAnfordernStatuscode WRITE KarteAnfordernStatuscodeSetzen)
	Q_PROPERTY(QByteArray QFrankDummyleserKarteAnfordernDaten READ ISO_ReadBinaryDaten WRITE ISO_ReadBinaryDatenSetzen)
	Q_PROPERTY(ulong QFrankDummyleserKarteEntfernenStatuscode READ KarteAnfordernStatuscode WRITE KarteAnfordernStatuscodeSetzen)

	public:
			QFrankDummyleser(QObject* eltern);
			bool								ISO_SelectFileFehler();
			void								ISO_SelectFileFehlerSetzen(bool fehler);
			QFrankLesegeraet::Rueckgabecodes	ISO_SelectFile(QByteArray datenfeld);

			ulong								ISO_ReadBinaryStatuscode();
			void								ISO_ReadBinaryStatuscodeSetzen(ulong status);
			QByteArray							ISO_ReadBinaryDaten();
			void								ISO_ReadBinaryDatenSetzen(QByteArray dummydaten);						
			QFrankLesegeraet::Rueckgabecodes	ISO_ReadBinary(QByteArray datenfeld,QByteArray &zielfeld);
			
			ulong								ISO_UpdateBinaryStatuscode();
			void								ISO_UpdateBinaryStatuscodeSetzen(ulong status);
			QFrankLesegeraet::Rueckgabecodes	ISO_UpdateBinary(QByteArray datenfeld);

			ulong								ISO_VerifyStatuscode();
			void								ISO_VerifyStatuscodeSetzen(ulong status);
			QFrankLesegeraet::Rueckgabecodes	ISO_Verify(QByteArray datenfeld);

			ulong								ISO_ChangeReferenceDataStatuscode();
			void								ISO_ChangeReferenceDataStatuscodeSetzen(ulong status);
			QFrankLesegeraet::Rueckgabecodes	ISO_ChangeReferenceData(QByteArray datenfeld);

			QByteArray							ISO_VerifySecurePin();
			void								ISO_VerifySecurePinSetzen(QByteArray pinsicher);
			QFrankLesegeraet::Rueckgabecodes	ISO_VerifySecure(QByteArray datenfeld);

			QByteArray							ISO_ChangeReferenceDataSecurePinNeu();
			void								ISO_ChangeReferenceDataSecurePinNeuSetzen(QByteArray pinneu);
			QFrankLesegeraet::Rueckgabecodes	ISO_ChangeReferenceDataSecure(QByteArray datenfeld);
			
			ulong								KarteAnfordernStatuscode();
			void								KarteAnfordernStatuscodeSetzen(ulong status);
			QFrankLesegeraet::Rueckgabecodes	UniversalIO(const QByteArray &daten, QByteArray &antwort);
			QFrankLesegeraet::Rueckgabecodes	KarteAnfordern(QByteArray &ATR);

			QFrankLesegeraet::Rueckgabecodes	KarteEntfernen();

			QFrankLesegeraet::Leserklasse		Sicherheitsklasse();
			void								SicherheitsklasseSetzen(ulong klasse);

			QFrankLesegeraet::Rueckgabecodes	SicherePineingabe(const QByteArray &kartenbefehl=0){return QFrankLesegeraet::ParameterFalsch;}

			QFrankLesegeraet::Rueckgabecodes	LeserInitialisieren();
			ulong								Version();
			
	private:
			bool								FehlerSF;
			QString					FeldNachHex(const QByteArray &feld) const;
			QString					BCDNachDez(QByteArray feld);
			QFrankLesegeraet::Rueckgabecodes	RueckgabecodeReadBinary;
			QFrankLesegeraet::Rueckgabecodes	RueckgabecodeUpdateBinary;
			QFrankLesegeraet::Rueckgabecodes	RueckgabecodeVerify;
			QFrankLesegeraet::Rueckgabecodes	RueckgabecodeChangeReferenceData;
			QFrankLesegeraet::Rueckgabecodes	RueckgabecodeKarteAnfordern;
			QFrankLesegeraet::Leserklasse		SicherheitsklasseWert;
			QByteArray				Datenfeld;
			QByteArray				PinSicher;
			QByteArray				PinSicherNeu;
			
};
#endif
