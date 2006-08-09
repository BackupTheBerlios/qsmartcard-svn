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

#include "ZeichensatzWandlung.h"

QFrankZeichensatzWandlung::QFrankZeichensatzWandlung(QObject *eltern): QObject(eltern)
{
}

QString const QFrankZeichensatzWandlung::SMSnachUTF8(const QByteArray &datenfeld) const
{
	QString tmp="";
	bool Ende=false;
	for (int Stelle = 0; Stelle < datenfeld.size(); Stelle++)
	{
		uchar Zeichen=(uchar)datenfeld.at(Stelle);
		switch(Zeichen)
		{
			case 0x00:
						tmp.append(QChar('@'));
						break;
			case 0x01:
						tmp.append(QChar(0xa3));//£
						break;
			case 0x02:
						tmp.append(QChar('$'));
						break;
			case 0x03:
						tmp.append(QChar(0xa5));//¥
						break;
			case 0x04:
						tmp.append(QChar(0xe8));//è
						break;
			case 0x05:
						tmp.append(QChar(0xe9));//é
						break;
			case 0x06:
						tmp.append(QChar(0xf9));//ù
						break;
			case 0x07:
						tmp.append(QChar(0xec));//ì
						break;
			case 0x08:
						tmp.append(QChar(0xf2));//ò
						break;
			case 0x09:
						tmp.append(QChar(0xc7));//Ç
						break;
			case 0x0b:
						tmp.append(QChar(0xd8));//Ø
						break;
			case 0x0c:
						tmp.append(QChar(0xf8));//ø
						break;
			case 0x0e:
						tmp.append(QChar(0xc5));//Å
						break;
			case 0x0f:
						tmp.append(QChar(0xe5));//å
						break;
			case 0x10:
						tmp.append(QChar(0x0394));//Δ
						break;
			case 0x11:
						tmp.append(QChar(0x05f));//_
						break;
			case 0x12:
						tmp.append(QChar(0x03a6));//Φ
						break;
			case 0x13:
						tmp.append(QChar(0x0393));//Γ
						break;
			case 0x14:
						tmp.append(QChar(0x039b));//Λ
						break;
			case 0x15:
						tmp.append(QChar(0x03a9));//Ω
						break;
			case 0x16:
						tmp.append(QChar(0x03a0));//Π
						break;
			case 0x17:
						tmp.append(QChar(0x03a8));//Ψ
						break;
			case 0x18:
						tmp.append(QChar(0x03a3));//Σ
						break;
			case 0x19:
						tmp.append(QChar(0x0398));//Θ
						break;
			case 0x1a:
						tmp.append(QChar(0x039e));//Ξ
						break;
			case 0x1b:
						//Escape Zeichen für die Erweiterungs Tabelle
						switch((uchar)datenfeld.at(Stelle+1))
						{
							case 0x40:
										tmp.append(QChar(0x07c));//|
										break;
							case 0x14:
										tmp.append(QChar(0x05e));//^
										break;
							case 0x65:
										tmp.append(QChar(0x20ac));//€
										break;
							case 0x38:
										tmp.append(QChar(0x07b));//{
										break;
							case 0x39:
										tmp.append(QChar(0x07d));//}
										break;
							case 0x0a:
										tmp.append(QChar(0x0c));//Seitenumbruch
										break;
							case 0x1b:
										tmp.append(QChar(0x20));//Reserviert als Escape für weitere Erweiterungen soll SP liefern
										break;
							case 0x3c:
										tmp.append(QChar(0x05b));//[
										break;
							case 0x3d:
										tmp.append(QChar(0x07e));//~
										break;
							case 0x3e:
										tmp.append(QChar(0x05d));//]
										break;
							case 0x2f:
										tmp.append(QChar(0x05c));//\
										break;
						}
						Stelle++;
						break;
			case 0x1c:
						tmp.append(QChar(0xc6));//Æ
						break;
			case 0x1d:
						tmp.append(QChar(0xe6));//æ
						break;
			case 0x1e:
						tmp.append(QChar(0xdf));//ß
						break;
			case 0x1f:
						tmp.append(QChar(0xc9));//É
						break;
			case 0x24:
						tmp.append(QChar(0xa4));//¤
						break;
			case 0x27:
						tmp.append(QChar(0xb4));//'
						break;
			case 0x40:
						tmp.append(QChar(0xa1));//¡
						break;
			case 0x5b:
						tmp.append(QChar(0xc4));//Ä
						break;
			case 0x5c:
						tmp.append(QChar(0xd6));//Ö
						break;
			case 0x5d:
						tmp.append(QChar(0xd1));//Ñ
						break;
			case 0x5e:
						tmp.append(QChar(0xdc));//Ü
						break;
			case 0x5f:
						tmp.append(QChar(0xa7));//§
						break;
			case 0x60:
						tmp.append(QChar(0xbf));//¿
						break;
			case 0x7b:
						tmp.append(QChar(0xe4));//ä
						break;
			case 0x7c:
						tmp.append(QChar(0xf6));//ö
						break;
			case 0x7d:
						tmp.append(QChar(0xf1));//ñ
						break;
			case 0x7e:
						tmp.append(QChar(0xfc));//ü
						break;
			case 0x7f:
						tmp.append(QChar(0xe0));//à
						break;
			case 0xff:
						Ende=true;
						break;
			default:
						//alles was bisher nicht bearbeitet wurde ist ASCII
						tmp.append(Zeichen);
						break;
		}
		if(Ende)
			break;
    }
	return tmp;
}
