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
	setObjectName("QFrankCT_API_Leser");
	Portnummer=1;
	Terminalnummer=1;
	VerbindungZumKartenleser=false;
	Lesersicherheit=QFrankLesegeraet::KlasseUnbekannt;
//Vordefiniert für Reiner SCT Leser
#ifdef Q_WS_WIN
	//Treiberdatei="G:\\Versuch\\cj\\ctFilter.dll";
	Treiberdatei=QString("%1%2").arg(getenv("windir")).arg("\\system32\\ctrsct32.dll");
#else
	Treiberdatei="/usr/lib/readers/libctapi-cyberjack.so";
#endif
}

ulong QFrankCT_API_Leser::Version()
{
	return CT_API_Version;
}

QFrankCT_API_Leser::~QFrankCT_API_Leser()
{
	CT_API_schliessen();
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::LeserInitialisieren()
{
#ifdef MEINDEBUG
	qDebug()<<"Versuche den CT API Leser mit folgenden Werten zu initialisieren.\r\n"\
			<<"Treiber:"<<Treiberdatei<<"Port:"<<Portnummer<<"Terminalnummer:"<<Terminalnummer;
#endif
	MeinCT_init=(pCT_init)QLibrary::resolve(Treiberdatei, "CT_init");
	MeinCT_data=(pCT_data)QLibrary::resolve(Treiberdatei, "CT_data");
	MeinCT_close=(pCT_close)QLibrary::resolve(Treiberdatei, "CT_close");
	//wurden alle Funktionen gefunden??
	if(MeinCT_init==0 || MeinCT_data==0 || MeinCT_close==0)
	{
#ifdef MEINDEBUG
		qDebug("Der CT-API Treiber konnte nicht geladen werden. Die Zeiger haben folgenden Werte:\r\n"\
			   "CT_init: 0x%x CT_data: 0x%x CT_close: 0x%x",MeinCT_init,MeinCT_data,MeinCT_close);
#endif
		return  QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Aufruf von CT-init
	Rueckgabecode=MeinCT_init(Terminalnummer,Portnummer);
	if(Rueckgabecode!=0)
	{
#ifdef MEINDEBUG
		qDebug()<<"CT-init konnte nicht ausgeführt werden. Rückgabe:"<<(int)Rueckgabecode;
#endif
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Sicherheitsklasse ermitteln
	Befehl[0]=0x20;
	Befehl[1]=0x13;
	Befehl[2]=0x00;
	Befehl[3]=0x81;//Was hat der Leser alles
	Befehl[4]=0x00;
	LaengeDesBefehl=5;
	Zieladresse=1;
	Quelladresse=2;
	LaengeDerAntwort=sizeof(Antwort);
	Rueckgabecode=MeinCT_data(Terminalnummer,&Zieladresse,&Quelladresse,LaengeDesBefehl,Befehl,&LaengeDerAntwort,Antwort);
	if(Rueckgabecode!=0)
	{
#ifdef MEINDEBUG
		qDebug()<<"Befehl konnte nicht das das Terminal gesendet werden. Rückgabe:"<<(int)Rueckgabecode;
#endif
		CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswerten des Ergebnisses
	if(Antwort[LaengeDerAntwort-2]!=0x90 || Antwort[LaengeDerAntwort-1]!=0x00)
	{
		uint Ergebnis=(Antwort[LaengeDerAntwort-2] <<8) | Antwort[LaengeDerAntwort-1];
#ifdef MEINDEBUG
		qDebug()<<"Befehl Komponentenermittlung fehlgeschlagen Rückgabe:"<<QString("%1").arg(Ergebnis,0,16);
#endif
		CT_API_schliessen();
		return (QFrankLesegeraet::Rueckgabecodes)Ergebnis;
	}
	/*Block durchsuchen
	Byte 2 Länge
	*/
	bool Tastatur=false;
	bool Display=false;
	bool Karteneinschub1=false;
	bool Karteneinschub2=false;
	for(int x=0;x<(Antwort[1]);x++)
	{
		switch (Antwort[x+2])
		{
			case 0x40:
						Display=true;
#ifdef MEINDEBUG
						qDebug()<<"Der Leser hat ein Display";
#endif
						break;
			case 0x50:
						Tastatur=true;
#ifdef MEINDEBUG
						qDebug()<<"Der Leser hat eine Tastatur";
#endif
						break;
			case 0x01:
						Karteneinschub1=true;
#ifdef MEINDEBUG
						qDebug()<<"Der Leser hat ein Karteneinschub";
#endif
						break;
			case 0x02:
						Karteneinschub2=true;
#ifdef MEINDEBUG
						qDebug()<<"Der Leser hat einen 2. Karteneinschub";
#endif
						break;
		}
	}
	//Sicherheitsklasse ermitteln
	if(Display && Tastatur && Karteneinschub1 && Karteneinschub2)
	{
		Lesersicherheit=QFrankLesegeraet::Klasse4;
	}
	else if(Display && Tastatur && Karteneinschub1)
	{
		Lesersicherheit=QFrankLesegeraet::Klasse3;
	}
	else if(Tastatur && Karteneinschub1)
	{
		Lesersicherheit=QFrankLesegeraet::Klasse2;
	}
	else if(Karteneinschub1)
	{
		Lesersicherheit=QFrankLesegeraet::Klasse1;
	}
	VerbindungZumKartenleser=true;
	return QFrankLesegeraet::CommandSuccessful;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_SelectFile(QByteArray datenfeld)
{
	if(!VerbindungTesten("ISO_SelectFile"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_ReadBinary(QByteArray datenfeld,QByteArray &Zielfeld)
{
	if(!VerbindungTesten("ISO_ReadBinary"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_UpdateBinary(QByteArray datenfeld)
{
	if(!VerbindungTesten("ISO_UpdateBinary"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_Verify(QByteArray datenfeld)
{
	if(!VerbindungTesten("ISO_Verify"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_ChangeReferenceData(QByteArray datenfeld)
{
	if(!VerbindungTesten("ISO_ChangeReferenceData"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_VerifySecure(QByteArray datenfeld)
{
	if(!VerbindungTesten("ISO_VerifySecure"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_ChangeReferenceDataSecure(QByteArray datenfeld)
{
	if(!VerbindungTesten("ISO_ChangeReferenceDataSecure"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::KarteAnfordern(QByteArray &ATR)
{
	if(!VerbindungTesten("KarteAnfordern"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Leserklasse QFrankCT_API_Leser::Sicherheitsklasse()
{
	if(!VerbindungTesten("Sicherheitsklasse"))
		return QFrankLesegeraet::KlasseUnbekannt;
#ifdef MEINDEBUG
	switch(Lesersicherheit)
	{
		case QFrankLesegeraet::Klasse1:
										qDebug()<<"Lesser Klasse 1";
										break;
		case QFrankLesegeraet::Klasse2:
										qDebug()<<"Lesser Klasse 2";
										break;
		case QFrankLesegeraet::Klasse3:
										qDebug()<<"Lesser Klasse 3";
										break;
		case QFrankLesegeraet::Klasse4:
										qDebug()<<"Lesser Klasse 4";
										break;
		case QFrankLesegeraet::KlasseUnbekannt:
										qDebug()<<"Lesser Klasse unbekannt";
										break;
		default:
										qFatal("Variable Lesersicherheit hat einen unbekannte Wert!!");
										break;
	}
#endif
	return Lesersicherheit;
}

uint QFrankCT_API_Leser::PortnummerHohlen()
{
	return Portnummer;
}

void QFrankCT_API_Leser::PortnummerSetzen(uint portnummer)
{
	Portnummer=portnummer;
}

QString QFrankCT_API_Leser::TreiberdateiHohlen()
{
	return Treiberdatei;
}

void QFrankCT_API_Leser::TreiberdateiSetzen(QString treiber)
{
	Treiberdatei=treiber;
}

bool QFrankCT_API_Leser::VerbindungTesten(QString programmteil)
{
	if(!VerbindungZumKartenleser)
	{
#ifdef MEINDEBUG
		qDebug()<<objectName()<<programmteil<<"gescheitert, da der Leser nicht initialisiert.";
#endif
		return false;
	}
	return true;
}

void QFrankCT_API_Leser::CT_API_schliessen()
{
	Rueckgabecode=MeinCT_close(Terminalnummer);
#ifdef MEINDEBUG
	if(Rueckgabecode!=0)
		qDebug()<<"CT-Close mit Fehler beendet. Rückgabe:"<<(int)Rueckgabecode;
#endif
}

#ifdef MEINDEBUG
QString QFrankCT_API_Leser::FeldNachHex(QByteArray feld)
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
#endif
