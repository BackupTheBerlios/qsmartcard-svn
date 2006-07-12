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

#include "GSM.h"
#include <Lesegeraet.h>

QFrankGSMKarte::QFrankGSMKarte(QObject* eltern):QFrankSmartCard(eltern)
{
	setObjectName("QFrankGSMKarte");
	K_PIN1gesetzt=false;
	K_PIN2gesetzt=false;
	K_KarteAkiviert=false;
	K_Leser=0;
	K_Seriennummer="";
}

bool QFrankGSMKarte::KarteAktivieren()
{
	if(K_Leser==0)
	{
		K_Fehlertext=tr("Keine Verbindung zum Kartenleser");
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte: keine Verbindung zum Leser";
#endif
		return false;
	}
	QByteArray ATR;
	if(K_Leser->KarteAnfordern(ATR)!=QFrankLesegeraet::CommandSuccessfulAsynchron)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte: keine/falsche Karte im Leser";
		qDebug()<<FeldNachHex(ATR);
#endif
		K_Fehlertext=trUtf8("Keine oder ungültige Karte eingelegt");
	return false;
	}
	/* haben wir eine SIM Karte???
	   Jede Karte hat eine Seriennummer(10 Bytes)	   
	*/
	if (!SeriennummerErmitteln())
		return false;
	else
	{
		K_KarteAkiviert=true;
		return true;
	}
}

bool QFrankGSMKarte::SeriennummerErmitteln()
{
	
}

const QString QFrankGSMKarte::Fehlertext() const
{
	return K_Fehlertext;
}

ulong QFrankGSMKarte::Version()
{
	return GSMKarteVersion;
}

const QString QFrankGSMKarte::Seriennummer() const
{
	return K_Seriennummer;
}

void QFrankGSMKarte::welchenLeser(QFrankLesegeraet *diesen)
{
	K_Leser=diesen;
	if(diesen->LeserInitialisieren()!=QFrankLesegeraet::CommandSuccessful)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte: Leser initialisieren gescheitert.";
#endif
		K_Leser=0;
		return;
	}	

}

QString	QFrankGSMKarte::FeldNachHex(const QByteArray &feld) const
{
	QString tmp="";
	uchar low,high;
	for(int x=0;x<feld.size();x++)
	{
		//Byte zerlegen
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

