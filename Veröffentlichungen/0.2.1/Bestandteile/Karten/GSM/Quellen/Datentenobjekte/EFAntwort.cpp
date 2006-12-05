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

#include "EFAntwort.h"

QFrankGSMKarteEFAntwort::QFrankGSMKarteEFAntwort(QObject* eltern):QFrankGSMKarteAntwortbasis(eltern)
{
	K_ErhoehenFuerZueklischEFErlaubt=false;
	K_BerechtigungLesenSuchen=QFrankGSMKarteEFAntwort::Nie;
	K_BerechtigungAktualisieren=QFrankGSMKarteEFAntwort::Nie;
	K_BerechtigungErhoehen=QFrankGSMKarteEFAntwort::Nie;
	K_BerechtigungUngueltigErklaeren=QFrankGSMKarteEFAntwort::Nie;
	K_BerechtigungRehabilitieren=QFrankGSMKarteEFAntwort::Nie;
	K_DateiGueltig=false;
	K_Dateiaufbau=QFrankGSMKarteEFAntwort::Transparent;
	K_Datensatzlaenge=0;
	K_Dateigroesse=0;
}

const bool& QFrankGSMKarteEFAntwort::ErhoehenFuerZueklischEFErlaubt() const
{
	return K_ErhoehenFuerZueklischEFErlaubt;
}

void QFrankGSMKarteEFAntwort::ErhoehenFuerZueklischEFErlaubtSetzen(const bool &erlaubt)
{
	K_ErhoehenFuerZueklischEFErlaubt=erlaubt;
}

const QFrankGSMKarteEFAntwort::Zugriffsberechtigung& QFrankGSMKarteEFAntwort::BerechtigungLesenSuchen() const
{
	return K_BerechtigungLesenSuchen;
}

void QFrankGSMKarteEFAntwort::BerechtigungLesenSuchenSetzen(const QFrankGSMKarteEFAntwort::Zugriffsberechtigung &suchen)
{
	K_BerechtigungLesenSuchen=suchen;
}

const QFrankGSMKarteEFAntwort::Zugriffsberechtigung& QFrankGSMKarteEFAntwort::BerechtigungAktualisieren() const
{
	return K_BerechtigungAktualisieren;
}

void QFrankGSMKarteEFAntwort::BerechtigungAktualisierenSetzen(const QFrankGSMKarteEFAntwort::Zugriffsberechtigung &aktualisieren)
{
	K_BerechtigungAktualisieren=aktualisieren;
}
const QFrankGSMKarteEFAntwort::Zugriffsberechtigung& QFrankGSMKarteEFAntwort::BerechtigungErhoehen() const
{
	return K_BerechtigungErhoehen;
}

void QFrankGSMKarteEFAntwort::BerechtigungErhoehenSetzen(const QFrankGSMKarteEFAntwort::Zugriffsberechtigung &erhoehen)
{
	K_BerechtigungErhoehen=erhoehen;
}

const QFrankGSMKarteEFAntwort::Zugriffsberechtigung& QFrankGSMKarteEFAntwort::BerechtigungUngueltigErklaeren() const
{
	return K_BerechtigungUngueltigErklaeren;
}

void QFrankGSMKarteEFAntwort::BerechtigungUngueltigErklaerenSetzen(const QFrankGSMKarteEFAntwort::Zugriffsberechtigung &ungueltig)
{
	K_BerechtigungUngueltigErklaeren=ungueltig;
}

const QFrankGSMKarteEFAntwort::Zugriffsberechtigung& QFrankGSMKarteEFAntwort::BerechtigungRehabilitieren() const
{
	return K_BerechtigungRehabilitieren;
}

void QFrankGSMKarteEFAntwort::BerechtigungRehabilitierenSetzen(const QFrankGSMKarteEFAntwort::Zugriffsberechtigung &reha)
{
	K_BerechtigungRehabilitieren=reha;
}

const bool& QFrankGSMKarteEFAntwort::DateiGueltig() const
{
	return K_DateiGueltig;
}

void QFrankGSMKarteEFAntwort::DateiGueltigSetzen(const bool &gueltig)
{
	K_DateiGueltig=gueltig;
}
const QFrankGSMKarteEFAntwort::InternerDateiaufbau& QFrankGSMKarteEFAntwort::Dateiaufbau() const
{
	return K_Dateiaufbau;
}

void QFrankGSMKarteEFAntwort::DateiaufbauSetzen(const QFrankGSMKarteEFAntwort::InternerDateiaufbau &aufbau)
{
	K_Dateiaufbau=aufbau;
}

const uchar& QFrankGSMKarteEFAntwort::Datensatzlaenge() const
{
	return K_Datensatzlaenge;
}

void QFrankGSMKarteEFAntwort::DatensatzlaengeSetzen(const uchar &laenge)
{
	K_Datensatzlaenge=laenge;
}

const uint& QFrankGSMKarteEFAntwort::Dateigroesse() const
{
	return K_Dateigroesse;
}

void QFrankGSMKarteEFAntwort::DateigroesseSetzen(const uint &groesse)
{
	K_Dateigroesse=groesse;
}

