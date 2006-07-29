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

#ifndef QFRANKGSMKARTEEFANTWORT
#define QFRANKGSMKARTEEFANTWORT

#include "Antwortbasis.h"

class QFrankGSMKarteEFAntwort: public QFrankGSMKarteAntwortbasis
{
	Q_OBJECT

	public:
				QFrankGSMKarteEFAntwort(QObject* eltern);
				enum													Legitimation{Immer=0x00,PIN1=0x01,PIN2=0x02,RFU=0x03,Admin01=0x04,Admin02=0x05,
							     													Admin03=0x06,Admin04=0x07,Admin05=0x08,Admin06=0x09,Admin07=0x0a,
							     													Admin08=0x0b,Admin09=0x0c,Admin10=0x0d,Admin11=0x0e,Nie=0x0f};
				enum													Struktur{Transparent=0x00,FesteLaenge=0x01,Zyklisch=0x03};
				Q_DECLARE_FLAGS(Zugriffsberechtigung,Legitimation)
				Q_DECLARE_FLAGS(InternerDateiaufbau,Struktur)
				const bool&												ErhoehenFuerZueklischEFErlaubt() const;
				const QFrankGSMKarteEFAntwort::Zugriffsberechtigung&	BerechtigungLesenSuchen() const;
				const QFrankGSMKarteEFAntwort::Zugriffsberechtigung&	BerechtigungAktualisieren() const;
				const QFrankGSMKarteEFAntwort::Zugriffsberechtigung&	BerechtigungErhoehen() const;
				const QFrankGSMKarteEFAntwort::Zugriffsberechtigung&	BerechtigungUngueltigErklaeren() const;
				const QFrankGSMKarteEFAntwort::Zugriffsberechtigung&	BerechtigungRehabilitieren() const;
				const bool&												DateiGueltig() const;
				const QFrankGSMKarteEFAntwort::InternerDateiaufbau&		Dateiaufbau() const;
				const uchar&											Datensatzlaenge() const;
				const uint&												Dateigroesse() const;
				void 													ErhoehenFuerZueklischEFErlaubtSetzen(const bool &erlaubt);
				void													BerechtigungLesenSuchenSetzen(const QFrankGSMKarteEFAntwort::Zugriffsberechtigung &suchen); 
				void													BerechtigungAktualisierenSetzen(const QFrankGSMKarteEFAntwort::Zugriffsberechtigung &
																										aktualisieren);
				void													BerechtigungErhoehenSetzen(const QFrankGSMKarteEFAntwort::Zugriffsberechtigung &erhoehen);
				void													BerechtigungUngueltigErklaerenSetzen(const QFrankGSMKarteEFAntwort::Zugriffsberechtigung &
																											ungueltig);
				void													BerechtigungRehabilitierenSetzen(const QFrankGSMKarteEFAntwort::Zugriffsberechtigung &reha);
				void													DateiGueltigSetzen(const bool &gueltig);
				void													DateiaufbauSetzen(const QFrankGSMKarteEFAntwort::InternerDateiaufbau &aufbau);
				void													DatensatzlaengeSetzen(const uchar &laenge);
				void													DateigroesseSetzen(const uint &groesse);

	private:
				bool													K_ErhoehenFuerZueklischEFErlaubt;
				QFrankGSMKarteEFAntwort::Zugriffsberechtigung			K_BerechtigungLesenSuchen;
				QFrankGSMKarteEFAntwort::Zugriffsberechtigung			K_BerechtigungAktualisieren;
				QFrankGSMKarteEFAntwort::Zugriffsberechtigung			K_BerechtigungErhoehen;
				QFrankGSMKarteEFAntwort::Zugriffsberechtigung			K_BerechtigungUngueltigErklaeren;
				QFrankGSMKarteEFAntwort::Zugriffsberechtigung			K_BerechtigungRehabilitieren;
				bool													K_DateiGueltig;
				QFrankGSMKarteEFAntwort::InternerDateiaufbau			K_Dateiaufbau;
				uchar													K_Datensatzlaenge;
				uint													K_Dateigroesse;
};				

#endif
