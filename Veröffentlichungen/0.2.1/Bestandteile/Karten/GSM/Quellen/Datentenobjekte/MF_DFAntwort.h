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

#ifndef QFRANKGSMKARTEMF_DFANTWORT
#define QFRANKGSMKARTEMF_DFANTWORT

#include "Antwortbasis.h"

class QFrankGSMKarteMF_DFAntwort:public QFrankGSMKarteAntwortbasis
{
	Q_OBJECT

	public:
				QFrankGSMKarteMF_DFAntwort(QObject* eltern);
				const bool&		PIN1Aktiviert() const;
				const uchar&	AnzahlDerUnterverzeichnisse() const;
				const uchar&	AnzahlDerDateien()const;
				const uchar&	AnzahlDerPINs()const;
				const bool&		PIN1gesetzt()const;
				const uchar&	PIN1verbleibeneEingabeversuche()const;
				const bool&		SuperPIN1gesetzt()const;
				const uchar&	SuperPIN1verbleibeneEingabeversuche()const;
				const bool&		PIN2gesetzt()const;
				const uchar&	PIN2verbleibeneEingabeversuche()const;
				const bool&		SuperPIN2gesetzt()const;
				const uchar&	SuperPIN2verbleibeneEingabeversuche()const;
				const uint&		FreierSpeicher()const;
				void			PIN1AktiviertSetzen(const bool &pin1aktiv);
				void			AnzahlDerUnterverzeichnisseSetzen(const uchar &verzeichnisse);
				void			AnzahlDerDateienSetzen(const uchar &dateien);
				void			AnzahlDerPINsSetzen(const uchar &pinns);
				void			PIN1gesetztSetzen(const bool &pin);
				void			PIN1verbleibeneEingabeversucheSetzen(const uchar &anzahl);
				void			SuperPIN1gesetztSetzen(const bool &pin);
				void			SuperPIN1verbleibeneEingabeversucheSetzen(const uchar &anzahl);
				void			PIN2gesetztSetzen(const bool &pin);
				void			PIN2verbleibeneEingabeversucheSetzen(const uchar &anzahl);
				void			SuperPIN2gesetztSetzen(const bool &pin);
				void			SuperPIN2verbleibeneEingabeversucheSetzen(const uchar &anzahl);
				void			FreierSpeicherSetzen(const uint &frei);

	private:
				bool			K_PIN1Aktiviert;
				uchar			K_AnzahlDerUnterverzeichnisse;
				uchar			K_AnzahlDerDateien;
				uchar			K_AnzahlDerPINs;
				bool			K_PIN1gesetzt;
				uchar			K_PIN1verbleibeneEingabeversuche;
				bool			K_SuperPIN1gesetzt;
				uchar			K_SuperPIN1verbleibeneEingabeversuche;
				bool			K_PIN2gesetzt;
				uchar			K_PIN2verbleibeneEingabeversuche;
				bool			K_SuperPIN2gesetzt;
				uchar			K_SuperPIN2verbleibeneEingabeversuche;
				uint			K_FreierSpeicher;
};

#endif
