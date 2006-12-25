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
#define CT_API_Version 0x000400
//Zum übersetzten wird min. Version 0.3.0 des Lesermodells benötigt.
#if LesegeraetAPI_Version < 0x000300
#error Es wird min. Version 0.3.0 des Lesermodells benötigt.
#endif

class QFrankCT_API_Leser: public QFrankLesegeraet
{
	Q_OBJECT
	Q_PROPERTY(uint QFrankCT_API_LeserPortnummer READ PortnummerHohlen WRITE PortnummerSetzen)
	Q_PROPERTY(QString QFrankCT_API_LeserTreiberdatei READ TreiberdateiHohlen WRITE TreiberdateiSetzen)	

	public:
			QFrankCT_API_Leser(QObject* eltern);
			~QFrankCT_API_Leser();
			QFrankLesegeraet::Rueckgabecodes	ISO_SelectFile(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_ReadBinary(QByteArray datenfeld,QByteArray &zielfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_UpdateBinary(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_Verify(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_ChangeReferenceData(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_VerifySecure(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_ChangeReferenceDataSecure(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	UniversalIO(const QByteArray &daten, QByteArray &antwort);
			QFrankLesegeraet::Rueckgabecodes	KarteAnfordern(QByteArray &ATR);
			QFrankLesegeraet::Rueckgabecodes	KarteEntfernen();
			QFrankLesegeraet::Leserklasse		Sicherheitsklasse();
			QFrankLesegeraet::Rueckgabecodes	LeserInitialisieren();
			QFrankLesegeraet::Rueckgabecodes	SicherePineingabe(const QByteArray &kartenbefehl);
			ulong								Version();
			

			uint								PortnummerHohlen();
			void								PortnummerSetzen(uint portnummer);
			QString								TreiberdateiHohlen();
			void								TreiberdateiSetzen(QString treiber);
			
	signals:
			void								TasteGedrueckt(const unsigned short &terminal);
	private:
			typedef char(*K_pCT_init)(unsigned short terminal,unsigned short port);
			typedef	char(*K_pCT_data)(unsigned short terminal, unsigned char *zieladresse,
									unsigned char *quelladresse,unsigned short befehlslaenge,
									unsigned char *befehle,unsigned short *antwortlaenge,
 									unsigned char *antworten);
			typedef	char(*K_pCT_close)(unsigned short terminal);
			typedef char(*K_pCT_setkeycb)(unsigned short terminal,int (*rueckruffunktion)(unsigned short terminal,void*daten),void *data);
			void								K_CT_API_schliessen();
			bool								K_VerbindungTesten(QString programmteil);
			bool								K_DatenSenden(uint terminalnummer, uchar *ziel,uchar *quelle,\
																ushort befehlslaenge,uchar *befehle,ushort *antwortlaenge,\
																uchar *antworten);
			bool								K_DatenfeldZuKlein(int groesse,QByteArray &Feld,QString programmteil);
			uint								K_Portnummer;
			QString								K_Treiberdatei;
			uint								K_Terminalnummer;
			bool								K_VerbindungZumKartenleser;
			bool								K_ISO_VerifySecureOderISO_ChangeReferenceDataSecure;
			bool								K_ISO_VerifyOderISO_ChangeReferenceData;
			unsigned char						K_Befehl[300];
			unsigned short						K_LaengeDesBefehl;
			unsigned char						K_Antwort[65500];
			unsigned short						K_LaengeDerAntwort;
			unsigned char						K_Zieladresse;
			unsigned char						K_Quelladresse;
			K_pCT_init							K_MeinCT_init;
			K_pCT_data							K_MeinCT_data;
			K_pCT_close							K_MeinCT_close;
			K_pCT_setkeycb						K_MeinCT_Tastendruck;
			QFrankLesegeraet::Leserklasse		K_Lesersicherheit;
			static int							K_TasteGerueckt(unsigned short terminal,void *daten);
			static QHash<const uint,QFrankCT_API_Leser*>	K_ListeDerTerminals;
#ifndef QT_NO_DEBUG
			QString								K_FeldNachHex(QByteArray feld);
#endif
};
#endif
