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

#include "Dummyleser.h"

QFrankDummyleser::QFrankDummyleser(QObject* eltern):QFrankLesegeraet(eltern)
{
	setObjectName("QFrankDummyleser");
	FehlerSF=false;
}

ulong QFrankDummyleser::Version()
{
	return DummyleserVersion;
}

bool QFrankDummyleser::ISO_SelectFileFehler()
{
	return FehlerSF;
}

void QFrankDummyleser::ISO_SelectFileFehlerSetzen(bool fehler)
{
	FehlerSF=fehler;
}

QString QFrankDummyleser::FeldNachHex(QByteArray feld)
{
	QString tmp="";
	uchar low,high;
	for(int x=0;x<feld.size();x++)
	{
		//Byte Zerlegen
		high=((feld.at(x) & 0xf0) >>4)+0x30;
		low=(feld.at(x) & 0x0f)+0x30;
		if(high>0x39)
			high=high+0x07;
		if(low>0x39)
			low=low+0x07;
		tmp.append(high);
		tmp.append(low);
		tmp.append("-");
	}
	return tmp.left(tmp.size()-1);
}

QFrankLesegeraet::Rueckgabecodes QFrankDummyleser::ISO_SelectFile(QByteArray datenfeld)
{
	qDebug()<<"Select File Anfang";
	if (datenfeld.size()<4)
	{
		//Datenfeld muss min. 4 Byte gross sein	
		qCritical()<<"Das Datenfeld ist zu klein Größe:"<<datenfeld.size()<<"\r\nSelect File Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	//Byte darf 00(FID) oder 04(AID) sein
	if(datenfeld.at(0)!=0x00 && datenfeld.at(0)!=0x04)
	{
		qCritical()<<"Ungültiger Wert für P1 Feld SelectFile Wert:"<<(uint)datenfeld.at(0)<<"\r\nSelect File Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	else
	{
		switch(datenfeld.at(0))
		{
			case 0x00:
						qDebug()<<"File Identifier wurde gesetzt";
						break;
			case 0x04:
						qDebug()<<"Application Identifier wurde gesetzt";
						break;
		}
	}
	/* stimmt der Wert des Längenfeldes?
	  P1=00 FID immer 2 Byte lang
	  P1=04 AID Länge nicht festgelegt
	  P2 muss immer 0 sein
	*/
	if(datenfeld.at(0)==0x00 && datenfeld.at(1)==0x00 && datenfeld.at(2)==0x02 && datenfeld.size()==5)
	{
		/* ist die FID gültig??
		   3F00 = ganzer Speicher
		   2F00 = DIR Datenbereich
		   2F01 = ATR Datenbereich
		*/
		//Hight Byte/ Low Byte
		uint FID=0;
		FID=datenfeld.at(3) << 8 | datenfeld.at(4);
		bool ungueltigFID=false;
		switch (FID)
		{
			case 0x3f00:
			case 0x2f00:
			case 0x2f01:
							break;
			default:
							ungueltigFID=true;
							break;
		}
		if(!ungueltigFID)
			qDebug()<<QString("FID ist: %1").arg(FID,0,16);
		else
		{
			qDebug()<<QString("FID ungülitig erkannt wurde:%1 \r\n%2").arg(FID,0,16).arg("Select File Ende");
			return QFrankLesegeraet::ParameterFalsch;
		}
	}
	//AID ??
	else if(datenfeld.at(0)==0x04 && datenfeld.at(1)==0x00 && (datenfeld.at(2)==(datenfeld.size()-3)))
	{
		qDebug()<<"AID ist:"<<FeldNachHex(datenfeld.mid(3,datenfeld.size()-3));
	}
	else
	{
		qDebug()<<"Daten ungültig Dump: "<<FeldNachHex(datenfeld)<<"\r\nSelect File Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	if(FehlerSF)
	{
		qDebug()<<"Select File Fehler wird simmuliert\r\nSelect File Ende";
		return QFrankLesegeraet::FileNotFound;
	}
	qDebug()<<"Selct File Ende";
	return QFrankLesegeraet::CommandSuccessful;
}
