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

#ifndef QFRANKLESEGERAET_H
#define QFRANKLESEGERAET_H

//XXYYZZ XX=Major YY=Minor ZZ=Patch
#define LesegeraetAPI_Version 0x000100

#include <QtCore>

class QFrankLesegeraet: public QObject
{
	Q_OBJECT
	public:
			enum Rueckgabecode
			{
				CommandSuccessful=0x9000,
				DataCorrupted=0x6281,
				WarningEOFbeforeLeBytes=0x6282,
				MemoryFailure=0x6501,
				FileNotFound=0x6a82,
				ParameterFalsch=0xffff
			};
			Q_DECLARE_FLAGS(Rueckgabecodes, Rueckgabecode)
			QFrankLesegeraet(QObject* eltern);
			virtual ulong								Version()=0;
			virtual QFrankLesegeraet::Rueckgabecodes	ISO_SelectFile(QByteArray datenfeld)=0;
			virtual QFrankLesegeraet::Rueckgabecodes	ISO_ReadBinary(QByteArray datenfeld,QByteArray &Zielfeld)=0;

};
Q_DECLARE_OPERATORS_FOR_FLAGS(QFrankLesegeraet::Rueckgabecodes)
#endif
