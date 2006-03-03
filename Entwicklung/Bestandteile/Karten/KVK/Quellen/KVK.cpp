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

#include "KVK.h"

QFrankKVK::QFrankKVK(QObject* eltern):QFrankSmartCard(eltern)
{
	setObjectName("QFrankKVK");
	KVKLeserWert=false;
	VariabelnInitialisieren();
}

void QFrankKVK::VariabelnInitialisieren()
{
	KrankenkassennameWert="";
	KrankenkassennummerWert=0;
	WOPWert=0;
	VersichertennummerWert=0;
	VersichertenstatusWert=0;
	StatusergaenzungWert=QChar();
	TitelWert="";
	VornameWert="";
	NamenszusatzWert="";
	NachnameWert="";
	GeburtsdatumWert=QDate(1800,1,1);
	StrasseWert="";
	LandWert="";
	PLZWert="";
	OrtWert="";
	GueltigBisWert=QDate(1800,1,1);
}

QString	QFrankKVK::Krankenkassenname()
{
	return KrankenkassennameWert;
}

ulong QFrankKVK::Krankenkassennummer()
{
	return KrankenkassennummerWert;
}

uint QFrankKVK::WOP()
{
	return WOPWert;
}

qulonglong	QFrankKVK::Versichertennummer()
{
	return VersichertennummerWert;
}

uint QFrankKVK::Versichertenstatus()
{
	return VersichertenstatusWert;
}

QChar QFrankKVK::Statusergaenzung()
{
	return StatusergaenzungWert;
}

QString	QFrankKVK::Titel()
{
	return TitelWert;
}

QString	QFrankKVK::Vorname()
{
	return VornameWert;
}

QString	QFrankKVK::Namenszusatz()
{
	return NamenszusatzWert;
}

QString	QFrankKVK::Nachname()
{
	return NachnameWert;
}

QDate QFrankKVK::Geburtsdatum()
{
	return GeburtsdatumWert;
}

QString	QFrankKVK::Strasse()
{
	return StrasseWert;
}

QString	QFrankKVK::Land()
{
	return	LandWert;
}

QString	QFrankKVK::PLZ()
{
	return PLZWert;
}

QString	QFrankKVK::Ort()
{
		return OrtWert;
}

QDate QFrankKVK::GueltigBis()
{
	return GueltigBisWert;
}

bool QFrankKVK::KVKLeser()
{
	return KVKLeserWert;
}

void QFrankKVK::KVKLeserSetzen(bool kvk)
{
	KVKLeserWert=kvk;
}

ulong QFrankKVK::Version()
{
	return KVKVersion;
}
