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
	Q_PROPERTY(uint QFrankCT_API_LeserPortnummer READ PortnummerHohlen WRITE PortnummerSetzen)
	Q_PROPERTY(QString QFrankCT_API_LeserTreiberdatei READ TreiberdateiHohlen WRITE TreiberdateiSetzen)	

	public:
			QFrankCT_API_Leser(QObject* eltern);
			~QFrankCT_API_Leser();
			QFrankLesegeraet::Rueckgabecodes	ISO_SelectFile(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_ReadBinary(QByteArray datenfeld,QByteArray &Zielfeld);
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
			

			uint								PortnummerHohlen();
			void								PortnummerSetzen(uint portnummer);
			QString								TreiberdateiHohlen();
			void								TreiberdateiSetzen(QString treiber);

	private:
			typedef char(*pCT_init)(unsigned short terminal,unsigned short port);
			typedef	char(*pCT_data)(unsigned short terminal, unsigned char *zieladresse,
									unsigned char *quelladresse,unsigned short befehlslaenge,
									unsigned char *befehle,unsigned short *antwortlaenge,
 									unsigned char *antworten);
			typedef	char(*pCT_close)(unsigned short terminal);		
			void								CT_API_schliessen();
			bool								VerbindungTesten(QString programmteil);
			uint								Portnummer;
			QString								Treiberdatei;
			uint								Terminalnummer;
			bool								VerbindungZumKartenleser;
			pCT_init							MeinCT_init;
			char								Rueckgabecode;
			unsigned char						Befehl[300];
			unsigned short						LaengeDesBefehl;
			unsigned char						Antwort[65500];
			unsigned short						LaengeDerAntwort;
			unsigned char						Zieladresse;
			unsigned char						Quelladresse;
			pCT_data							MeinCT_data;
			pCT_close							MeinCT_close;
			QFrankLesegeraet::Leserklasse		Lesersicherheit;
#ifdef MEINDEBUG
			QString								FeldNachHex(QByteArray feld);
#endif
};
#endif
