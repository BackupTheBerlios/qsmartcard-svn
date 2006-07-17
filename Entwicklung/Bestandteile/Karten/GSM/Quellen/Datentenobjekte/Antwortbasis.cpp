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

#include "Antwortbasis.h"

QFrankGSMKarteAntwortbasis::QFrankGSMKarteAntwortbasis(QObject* eltern):QObject(eltern)
{
	K_FreierSpeicher=0;
	K_DateiID=0;
	K_Dateiart=QFrankGSMKarteAntwortbasis::MF;
}

const uint& QFrankGSMKarteAntwortbasis::FreierSpeicher() const
{
	return K_FreierSpeicher;
}

const uint& QFrankGSMKarteAntwortbasis::DateiID() const
{
	return K_DateiID;
}

const QFrankGSMKarteAntwortbasis::Dateitypen& QFrankGSMKarteAntwortbasis::Dateiart() const
{
	return K_Dateiart;
}

void QFrankGSMKarteAntwortbasis::FreierSpeicherSetzen(const uint &frei)
{
	K_FreierSpeicher=frei;
}

void QFrankGSMKarteAntwortbasis::DateiIDSetzen(const uint &id)
{
	K_DateiID=id;
}

void QFrankGSMKarteAntwortbasis::DateiartSetzen(const QFrankGSMKarteAntwortbasis::Dateitypen &art)
{
	K_Dateiart=art;
}
