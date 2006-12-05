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

#include "MF_DFAntwort.h"

QFrankGSMKarteMF_DFAntwort::QFrankGSMKarteMF_DFAntwort(QObject* eltern):QFrankGSMKarteAntwortbasis(eltern)
{
	K_PIN1Aktiviert=false;
	K_AnzahlDerUnterverzeichnisse=0;
	K_AnzahlDerDateien=0;
	K_AnzahlDerPINs=0;
	K_PIN1gesetzt=false;
	K_PIN1verbleibeneEingabeversuche=0;
	K_SuperPIN1gesetzt=false;
	K_SuperPIN1verbleibeneEingabeversuche=0;
	K_PIN2gesetzt=false;
	K_PIN2verbleibeneEingabeversuche=0;
	K_SuperPIN2gesetzt=false;
	K_SuperPIN2verbleibeneEingabeversuche=0;
	K_FreierSpeicher=0;
}

const bool& QFrankGSMKarteMF_DFAntwort::PIN1Aktiviert() const
{
	return K_PIN1Aktiviert;
}

void QFrankGSMKarteMF_DFAntwort::PIN1AktiviertSetzen(const bool &pin1aktiv)
{
	K_PIN1Aktiviert=pin1aktiv;
}

const uchar& QFrankGSMKarteMF_DFAntwort::AnzahlDerUnterverzeichnisse() const
{
	return K_AnzahlDerUnterverzeichnisse;
}

void QFrankGSMKarteMF_DFAntwort::AnzahlDerUnterverzeichnisseSetzen(const uchar &verzeichnisse)
{
	K_AnzahlDerUnterverzeichnisse=verzeichnisse;
}

const uchar& QFrankGSMKarteMF_DFAntwort::AnzahlDerDateien()const
{
	return K_AnzahlDerDateien;
}

void QFrankGSMKarteMF_DFAntwort::AnzahlDerDateienSetzen(const uchar &dateien)
{
	K_AnzahlDerDateien=dateien;
}

const uchar& QFrankGSMKarteMF_DFAntwort::AnzahlDerPINs()const
{
	return K_AnzahlDerPINs;
}

void QFrankGSMKarteMF_DFAntwort::AnzahlDerPINsSetzen(const uchar &pinns)
{
	K_AnzahlDerPINs=pinns;
}

const bool& QFrankGSMKarteMF_DFAntwort::PIN1gesetzt()const
{
	return K_PIN1gesetzt;
}

void QFrankGSMKarteMF_DFAntwort::PIN1gesetztSetzen(const bool &pin)
{
	K_PIN1gesetzt=pin;
}

const uchar& QFrankGSMKarteMF_DFAntwort::PIN1verbleibeneEingabeversuche()const
{
	return K_PIN1verbleibeneEingabeversuche;
}

void QFrankGSMKarteMF_DFAntwort::PIN1verbleibeneEingabeversucheSetzen(const uchar &anzahl)
{
	K_PIN1verbleibeneEingabeversuche=anzahl;
}

const bool& QFrankGSMKarteMF_DFAntwort::SuperPIN1gesetzt()const
{
	return K_SuperPIN1gesetzt;
}

void QFrankGSMKarteMF_DFAntwort::SuperPIN1gesetztSetzen(const bool &pin)
{
	K_SuperPIN1gesetzt=pin;
}

const uchar& QFrankGSMKarteMF_DFAntwort::SuperPIN1verbleibeneEingabeversuche()const
{
	return K_SuperPIN1verbleibeneEingabeversuche;
}

void QFrankGSMKarteMF_DFAntwort::SuperPIN1verbleibeneEingabeversucheSetzen(const uchar &anzahl)
{
	K_SuperPIN1verbleibeneEingabeversuche=anzahl;
}

const bool& QFrankGSMKarteMF_DFAntwort::PIN2gesetzt()const
{
	return K_PIN2gesetzt;
}

void QFrankGSMKarteMF_DFAntwort::PIN2gesetztSetzen(const bool &pin)
{
	K_PIN2gesetzt=pin;
}

const uchar& QFrankGSMKarteMF_DFAntwort::PIN2verbleibeneEingabeversuche()const
{
	return K_PIN2verbleibeneEingabeversuche;
}

void QFrankGSMKarteMF_DFAntwort::PIN2verbleibeneEingabeversucheSetzen(const uchar &anzahl)
{
	K_PIN2verbleibeneEingabeversuche=anzahl;
}

const bool& QFrankGSMKarteMF_DFAntwort::SuperPIN2gesetzt()const
{
	return K_SuperPIN2gesetzt;
}

void QFrankGSMKarteMF_DFAntwort::SuperPIN2gesetztSetzen(const bool &pin)
{
	K_SuperPIN2gesetzt=pin;
}

const uchar& QFrankGSMKarteMF_DFAntwort::SuperPIN2verbleibeneEingabeversuche()const
{
	return K_SuperPIN2verbleibeneEingabeversuche;
}

void QFrankGSMKarteMF_DFAntwort::SuperPIN2verbleibeneEingabeversucheSetzen(const uchar &anzahl)
{
	K_SuperPIN2verbleibeneEingabeversuche=anzahl;
}

const uint& QFrankGSMKarteMF_DFAntwort::FreierSpeicher()const
{
	return K_FreierSpeicher;
}

void QFrankGSMKarteMF_DFAntwort::FreierSpeicherSetzen(const uint &frei)
{
	K_FreierSpeicher=frei;
}

