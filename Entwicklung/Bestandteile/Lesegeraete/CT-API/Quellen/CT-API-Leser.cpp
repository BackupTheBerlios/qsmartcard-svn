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

#include "CT-API-Leser.h"

QFrankCT_API_Leser::QFrankCT_API_Leser(QObject* eltern):QFrankLesegeraet(eltern)
{
	
}

ulong QFrankCT_API_Leser::Version()
{
	return CT_API_Version;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_SelectFile(QByteArray datenfeld)
{	
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_ReadBinary(QByteArray datenfeld,QByteArray &Zielfeld)
{
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_UpdateBinary(QByteArray datenfeld)
{	
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_Verify(QByteArray datenfeld)
{
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_ChangeReferenceData(QByteArray datenfeld)
{
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_VerifySecure(QByteArray datenfeld)
{
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_ChangeReferenceDataSecure(QByteArray datenfeld)
{
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::KarteAnfordern(QByteArray &ATR)
{	
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Leserklasse QFrankCT_API_Leser::Sicherheitsklasse()
{
	return QFrankLesegeraet::KlasseUnbekannt;
}

