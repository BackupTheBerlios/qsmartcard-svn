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

#ifndef QFRANKDUMMYLESER_H
#define QFRANKDUMMYLESER_H

#include <QtCore>
#include <Lesegeraet.h>

//XXYYZZ XX=Major YY=Minor ZZ=Patch
#define DummyleserVersion 0x000100

class QFrankDummyleser: public QFrankLesegeraet
{
	Q_OBJECT
	Q_PROPERTY(bool QFrankDummyleserISO_SelectFileFehler READ ISO_SelectFileFehler WRITE ISO_SelectFileFehlerSetzen)
	Q_PROPERTY(QByteArray QFrankDummyleserISO_ReadBinaryDaten READ ISO_ReadBinaryDaten WRITE ISO_ReadBinaryDatenSetzen)
	Q_PROPERTY(ulong QFrankDummyleserISO_ReadBinaryStatuscode READ ISO_ReadBinaryStatuscode WRITE ISO_ReadBinaryStatuscodeSetzen)
	public:
			QFrankDummyleser(QObject* eltern);
			bool								ISO_SelectFileFehler();
			void								ISO_SelectFileFehlerSetzen(bool fehler);
			ulong								ISO_ReadBinaryStatuscode();
			void								ISO_ReadBinaryStatuscodeSetzen(ulong status);
			QByteArray							ISO_ReadBinaryDaten();
			void								ISO_ReadBinaryDatenSetzen(QByteArray dummydaten);						
			QFrankLesegeraet::Rueckgabecodes	ISO_SelectFile(QByteArray datenfeld);
			QFrankLesegeraet::Rueckgabecodes	ISO_ReadBinary(QByteArray datenfeld,QByteArray &Zielfeld);
			ulong								Version();

	private:
			bool								FehlerSF;
			QString								FeldNachHex(QByteArray feld);
			QFrankLesegeraet::Rueckgabecodes	RueckgabecodeReadBinary;
			QByteArray							Datenfeld;
};
#endif
