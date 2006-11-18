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

/*!
	\ingroup lesegeraete
	\class QFrankCT_API_Leser
	\brief Die QFrankCT_API_Leser Klasse stellt den Zugriff auf Lesegeräte per CT-API bereit
	
*/
QFrankCT_API_Leser::QFrankCT_API_Leser(QObject* eltern):QFrankLesegeraet(eltern)
{
	setObjectName("QFrankCT_API_Leser");
	K_Portnummer=1;
	K_Terminalnummer=1;
	K_VerbindungZumKartenleser=false;
	K_ISO_VerifyOderISO_ChangeReferenceData=false;
	K_ISO_VerifySecureOderISO_ChangeReferenceDataSecure=false;
	K_Lesersicherheit=QFrankLesegeraet::KlasseUnbekannt;
//Vordefiniert für Reiner SCT Leser
#ifdef Q_WS_WIN
	//K_Treiberdatei="G:\\Versuch\\cj\\ctFilter.dll";
	K_Treiberdatei=QString("%1%2").arg(getenv("windir")).arg("\\system32\\ctrsct32.dll");
#else
	K_Treiberdatei="/usr/lib/readers/libctapi-cyberjack.so";
#endif
	//Warnung bei DEBUG
#ifndef QT_NO_DEBUG
	qWarning(qPrintable(trUtf8("QFrankCT_API_Leser Konstuktor: WARNUNG Debugversion wird benutzt.\r\nEs könnten sicherheitsrelevante Daten ausgegeben werden!!!!!",
								"debug")));
#endif
}

ulong QFrankCT_API_Leser::Version()
{
	return CT_API_Version;
}

QFrankCT_API_Leser::~QFrankCT_API_Leser()
{
	K_CT_API_schliessen();
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::LeserInitialisieren()
{
#ifndef QT_NO_DEBUG
	qDebug(qPrintable(QString("QFrankCT_API_Leser LeserInitialisieren: Versuche den CT API Leser mit folgenden Werten zu initialisieren.\r\n"\
							  "Treiber: %1 Port: %2 Terminalnummer: %3").arg(K_Treiberdatei).arg(K_Portnummer).arg(K_Terminalnummer)));
#endif
	K_MeinCT_init=(K_pCT_init)QLibrary::resolve(K_Treiberdatei, "CT_init");
	K_MeinCT_data=(K_pCT_data)QLibrary::resolve(K_Treiberdatei, "CT_data");
	K_MeinCT_close=(K_pCT_close)QLibrary::resolve(K_Treiberdatei, "CT_close");
	//wurden alle Funktionen gefunden??
	if(K_MeinCT_init==0 || K_MeinCT_data==0 || K_MeinCT_close==0)
	{
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(QString("QFrankCT_API_Leser LeserInitialisieren: Der CT-API Treiber konnte nicht geladen werden. "
									 "Die Zeiger haben folgenden Werte:\r\n CT_init: 0x%1 CT_data: 0x%2 CT_close: 0x%3").arg((qlonglong)K_MeinCT_init,0,16)
																														.arg((qlonglong)K_MeinCT_data,0,16)
																														.arg((qlonglong)K_MeinCT_close,0,16)));
#endif
		return  QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Aufruf von CT-init
	char Rueckgabecode=K_MeinCT_init(K_Terminalnummer,K_Portnummer);
	if(Rueckgabecode!=0)
	{
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(trUtf8("QFrankCT_API_Leser LeserInitialisieren: CT-init konnte nicht ausgeführt werden. Rückgabe: %1","debug")
									.arg((int)Rueckgabecode)));
#endif
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Sicherheitsklasse ermitteln
	K_Befehl[0]=0x20;
	K_Befehl[1]=0x13;
	K_Befehl[2]=0x00;
	K_Befehl[3]=0x81;//Was hat der Leser alles
	K_Befehl[4]=0x00;
	K_LaengeDesBefehl=5;
	K_Zieladresse=1;
	K_Quelladresse=2;
	K_LaengeDerAntwort=sizeof(K_Antwort); //Muss immer sein, da vom der CT DLL verbogen wird
	if(!K_DatenSenden(K_Terminalnummer,&K_Zieladresse,&K_Quelladresse,K_LaengeDesBefehl,K_Befehl,&K_LaengeDerAntwort,K_Antwort ))
	{
		K_CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswerten des Ergebnisses
	uint Ergebnis=(K_Antwort[K_LaengeDerAntwort-2] <<8) | K_Antwort[K_LaengeDerAntwort-1];
	if(Ergebnis!=QFrankLesegeraet::CommandSuccessful)
	{		
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(trUtf8("QFrankCT_API_Leser LeserInitialisieren: Befehl Komponentenermittlung fehlgeschlagen. Rückgabe: 0x%1","debug")
								.arg(Ergebnis,0,16)));
#endif
		K_CT_API_schliessen();
		return (QFrankLesegeraet::Rueckgabecodes)Ergebnis;
	}
	/*Block durchsuchen
	Byte 2 Länge
	*/
	bool Tastatur=false;
	bool Display=false;
	bool Karteneinschub1=false;
	bool Karteneinschub2=false;
	for(int x=0;x<(K_Antwort[1]);x++)
	{
		switch (K_Antwort[x+2])
		{
			case 0x40:
						Display=true;
#ifndef QT_NO_DEBUG
						qDebug("QFrankCT_API_Leser LeserInitialisieren: Der Leser hat ein Display");
#endif
						break;
			case 0x50:
						Tastatur=true;
#ifndef QT_NO_DEBUG
						qDebug("QFrankCT_API_Leser LeserInitialisieren: Der Leser hat eine Tastatur");
#endif
						break;
			case 0x01:
						Karteneinschub1=true;
#ifndef QT_NO_DEBUG
						qDebug("QFrankCT_API_Leser LeserInitialisieren: Der Leser hat ein Karteneinschub");
#endif
						break;
			case 0x02:
						Karteneinschub2=true;
#ifndef QT_NO_DEBUG
						qDebug("QFrankCT_API_Leser LeserInitialisieren: Der Leser hat einen 2. Karteneinschub");
#endif
						break;
		}
	}
	//Sicherheitsklasse ermitteln
	if(Display && Tastatur && Karteneinschub1 && Karteneinschub2)
	{
		K_Lesersicherheit=QFrankLesegeraet::Klasse4;
	}
	else if(Display && Tastatur && Karteneinschub1)
	{
		K_Lesersicherheit=QFrankLesegeraet::Klasse3;
	}
	else if(Tastatur && Karteneinschub1)
	{
		K_Lesersicherheit=QFrankLesegeraet::Klasse2;
	}
	else if(Karteneinschub1)
	{
		K_Lesersicherheit=QFrankLesegeraet::Klasse1;
	}
	K_VerbindungZumKartenleser=true;
	return QFrankLesegeraet::CommandSuccessful;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::SicherePineingabe(const QByteArray &kartenbefehl)
{
	if(!K_VerbindungTesten("SicherePineingabe"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	//mindestes ein Klasse 2 Leser(SicherePineingabe)
	if(!(K_Lesersicherheit>QFrankLesegeraet::Klasse1 && K_Lesersicherheit<=QFrankLesegeraet::Klasse4))
	{
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(trUtf8("QFrankCT_API_Leser SicherePineingabe: nur mit Geräten der Klasse 2 oder höher möglich!","debug")));		
#endif
	return QFrankLesegeraet::ParameterFalsch;
	}
	//Datfeld darf nicht größer als 255 Bytes sein
	if(kartenbefehl.size()>255)
	{
#ifndef QT_NO_DEBUG
		qCritical("QFrankCT_API_Leser SicherePineingabe: Kartenbefehl zu lang");		
#endif
		return QFrankLesegeraet::ParameterFalsch;
	}
	K_Befehl[0]=0x20;
	K_Befehl[1]=0x18;
	K_Befehl[2]=0x01;//Karte im Slot 1
	K_Befehl[3]=0x00;//0x00=Pin,0x01=biometische Daten
	K_Befehl[4]=kartenbefehl.size();//Länge des Datenfeldes
	K_LaengeDesBefehl=kartenbefehl.size()+5;
	K_LaengeDerAntwort=sizeof(K_Antwort);
	K_Zieladresse=1; // 0=Slot1  1=Termimal 2=Slot2
	K_Quelladresse=2;
	//Kartenbefehl kopieren
	memcpy(K_Befehl+5,kartenbefehl.data(),kartenbefehl.size());
#ifndef QT_NO_DEBUG
	qDebug(qPrintable(QString("QFrankCT_API_Leser SicherePineingabe: Kartenbefehl: %1").arg(K_FeldNachHex(QByteArray((char*)K_Befehl,K_LaengeDesBefehl)))));
#endif
	if(!K_DatenSenden(K_Terminalnummer,&K_Zieladresse,&K_Quelladresse,K_LaengeDesBefehl,K_Befehl,&K_LaengeDerAntwort,K_Antwort))
	{
		K_CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(K_Antwort[K_LaengeDerAntwort-2] <<8) | K_Antwort[K_LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	qDebug("QFrankCT_API_Leser SicherePineingabe: Statuskode vom Leser: 0x%X",Ergebnis);
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;	
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_SelectFile(QByteArray datenfeld)
{
	if(!K_VerbindungTesten("ISO_SelectFile"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	if(K_DatenfeldZuKlein(sizeof(K_Befehl)-2,datenfeld,"ISO_SelectFile"))
	{
		//Datenfeld passt nicht in das Befehlsfeld:(
		return QFrankLesegeraet::ParameterFalsch;
	}
	K_Befehl[0]=0x00;
	K_Befehl[1]=0xa4;
	K_LaengeDesBefehl=datenfeld.size()+2;
	K_Zieladresse=0; // 0=Slot1  1=Termimal 2=Slot2
	K_Quelladresse=2;
	K_LaengeDerAntwort=sizeof(K_Antwort);
	memcpy(K_Befehl+2,datenfeld.data(),datenfeld.size());//Kopieren des QByteArray in's Befehlsfeld
#ifndef QT_NO_DEBUG
	qDebug(qPrintable(QString("QFrankCT_API_Leser ISO_SelectFile: Datenfeld: %1").arg(K_FeldNachHex(QByteArray((char*)K_Befehl,K_LaengeDesBefehl)))));
#endif
	if(!K_DatenSenden(K_Terminalnummer,&K_Zieladresse,&K_Quelladresse,K_LaengeDesBefehl,K_Befehl,&K_LaengeDerAntwort,K_Antwort))
	{
		K_CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(K_Antwort[K_LaengeDerAntwort-2] <<8) | K_Antwort[K_LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	qDebug("QFrankCT_API_Leser ISO_SelectFile: Statuskode vom Leser: 0x%X",Ergebnis);
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::UniversalIO(const QByteArray &daten, QByteArray &antwort)
{
	if(!K_VerbindungTesten("UniversalIO"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	//alles wird blind an den Leser übergeben
#ifndef QT_NO_DEBUG
	qDebug(qPrintable(QString("QFrankCT_API_Leser UniversalIO: Senden: %1").arg(K_FeldNachHex(daten))));
#endif
	K_Zieladresse=0; // 0=Slot1  1=Termimal 2=Slot2
	K_Quelladresse=2;
	K_LaengeDerAntwort=sizeof(K_Antwort);
	K_LaengeDesBefehl=daten.size();
	//Kopieren des QByteArray in's Befehlsfeld
	memcpy(K_Befehl,daten.data(),daten.size());

	if(!K_DatenSenden(K_Terminalnummer,&K_Zieladresse,&K_Quelladresse,K_LaengeDesBefehl,K_Befehl,&K_LaengeDerAntwort,K_Antwort))
	{
		K_CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(K_Antwort[K_LaengeDerAntwort-2] <<8) | K_Antwort[K_LaengeDerAntwort-1];
	antwort.resize(K_LaengeDerAntwort);
	memcpy(antwort.data(),K_Antwort,K_LaengeDerAntwort);//In das Rückgabearray kopieren

#ifndef QT_NO_DEBUG
	qDebug("QFrankCT_API_Leser UniversalIO: Statuskode vom Leser: 0x%X",Ergebnis);
	qDebug(qPrintable(QString("QFrankCT_API_Leser UniversalIO: Empfangen: %1").arg(K_FeldNachHex(antwort))));
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_ReadBinary(QByteArray datenfeld,QByteArray &zielfeld)
{
	if(!K_VerbindungTesten("ISO_ReadBinary"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	if(K_DatenfeldZuKlein(sizeof(K_Befehl)-2,datenfeld,"ISO_ReadBinary"))
	{
		//Datenfeld passt nicht in das Befehlsfeld:(
		return QFrankLesegeraet::ParameterFalsch;
	}
	K_Befehl[0]=0x00;
	K_Befehl[1]=0xb0;
	K_LaengeDesBefehl=datenfeld.size()+2;
	K_Zieladresse=0; // 0=Slot1  1=Termimal 2=Slot2
	K_Quelladresse=2;
	K_LaengeDerAntwort=sizeof(K_Antwort);
	memcpy(K_Befehl+2,datenfeld.data(),datenfeld.size());//Kopieren des QByteArray in's Befehlsfeld
#ifndef QT_NO_DEBUG
	qDebug(qPrintable(QString("QFrankCT_API_Leser ISO_ReadBinary: Datenfeld(Quelle): %1").arg(K_FeldNachHex(QByteArray((char*)K_Befehl,K_LaengeDesBefehl)))));
#endif
	if(!K_DatenSenden(K_Terminalnummer,&K_Zieladresse,&K_Quelladresse,K_LaengeDesBefehl,K_Befehl,&K_LaengeDerAntwort,K_Antwort))
	{
		K_CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(K_Antwort[K_LaengeDerAntwort-2] <<8) | K_Antwort[K_LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	qDebug("QFrankCT_API_Leser ISO_ReadBinary: Statuskode vom Leser: 0x%X",Ergebnis);
#endif
	if(Ergebnis==QFrankLesegeraet::CommandSuccessful || Ergebnis==QFrankLesegeraet::WarningEOFbeforeLeBytes)
	{
		//Daten konnten gelesen werden
		zielfeld.resize(K_LaengeDerAntwort-2);// Die 2 Bytes Status werden nicht mit übergben.
		memcpy(zielfeld.data(),K_Antwort,K_LaengeDerAntwort-2);//In das Rückgabeaary kopieren
#ifndef QT_NO_DEBUG
	qDebug(qPrintable(QString("QFrankCT_API_Leser ISO_ReadBinary: Datenfeld(Ziel): %1").arg(K_FeldNachHex(zielfeld))));
#endif
	}
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_UpdateBinary(QByteArray datenfeld)
{
	if(!K_VerbindungTesten("ISO_UpdateBinary"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	if(K_DatenfeldZuKlein(sizeof(K_Befehl)-2,datenfeld,"ISO_UpdateBinary"))
	{
		//Datenfeld passt nicht in das Befehlsfeld:(
		return QFrankLesegeraet::ParameterFalsch;
	}
	//Datenlänge richtig angegeben??
	if(datenfeld.at(2)!=(datenfeld.size()-3))
	{
		//stimmt nicht
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(trUtf8("QFrankCT_API_Leser ISO_UpdateBinary: Datenlänge falsch","debug")));
#endif
	return QFrankLesegeraet::ParameterFalsch;
	}
	K_Befehl[0]=0x00;
	K_Befehl[1]=0xd6;
	K_LaengeDesBefehl=datenfeld.size()+2;
	K_LaengeDerAntwort=sizeof(K_Antwort);
	K_Zieladresse=0; // 0=Slot1  1=Termimal 2=Slot2
	K_Quelladresse=2;
	memcpy(K_Befehl+2,datenfeld.data(),datenfeld.size());//Kopieren des QByteArray in's Befehlsfeld
#ifndef QT_NO_DEBUG
	qDebug(qPrintable(QString("QFrankCT_API_Leser ISO_UpdateBinary: Datenfeld: %1").arg(K_FeldNachHex(QByteArray((char*)K_Befehl,K_LaengeDesBefehl)))));
#endif
	if(!K_DatenSenden(K_Terminalnummer,&K_Zieladresse,&K_Quelladresse,K_LaengeDesBefehl,K_Befehl,&K_LaengeDerAntwort,K_Antwort))
	{
		K_CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(K_Antwort[K_LaengeDerAntwort-2] <<8) | K_Antwort[K_LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	qDebug("QFrankCT_API_Leser ISO_UpdateBinary: Statuskode vom Leser: 0x%X",Ergebnis);
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;	
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_Verify(QByteArray datenfeld)
{
	QString Funktionsname;
	if(!K_ISO_VerifyOderISO_ChangeReferenceData)
	{
		//false=ISO_Verify true=ISO_ChangeReferenceData
		Funktionsname="ISO_Verify";
		K_Befehl[1]=0x20;
	}
	else
	{
		Funktionsname="ISO_ChangeReferenceData";
		K_Befehl[1]=0x24;
	}
	if(!K_VerbindungTesten(Funktionsname))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	//Bei Geräten mit Tastatur sollte besser die Secure Ausführung benutzt werden
	if(K_Lesersicherheit>QFrankLesegeraet::Klasse1 && K_Lesersicherheit<QFrankLesegeraet::KlasseUnbekannt)
	{
		qWarning(qPrintable(trUtf8("QFrankCT_API_Leser %1: Das Lesegerät kennt die sichere Pineingabe.\r\nDiese sollte auch benutzt werden!!","debug")
									.arg(Funktionsname)));
	}
	if(K_DatenfeldZuKlein(sizeof(K_Befehl)-4,datenfeld,Funktionsname))
	{
		//Datenfeld passt nicht in das Befehlsfeld:(
		return QFrankLesegeraet::ParameterFalsch;
	}
	//hat das Datenfeld mehr als 256 Stellen??
	if (datenfeld.size()>256)
	{
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(QString("QFrankCT_API_Leser %1: Die PIN ist zu lang.").arg(Funktionsname)));
#endif
		return QFrankLesegeraet::ParameterFalsch;
	}
	K_Befehl[0]=0x00;
	K_Befehl[2]=0x00;
	K_Befehl[3]=0x00;
	K_Befehl[4]=datenfeld.size();
	K_LaengeDesBefehl=datenfeld.size()+5;
	K_LaengeDerAntwort=sizeof(K_Antwort);
	K_Zieladresse=0; // 0=Slot1  1=Termimal 2=Slot2
	K_Quelladresse=2;
	memcpy(K_Befehl+5,datenfeld.data(),datenfeld.size());//Kopieren des QByteArray in's Befehlsfeld
#ifndef QT_NO_DEBUG
	qDebug(qPrintable(QString("QFrankCT_API_Leser %1: Datenfeld: %2").arg(Funktionsname).arg(K_FeldNachHex(QByteArray((char*)K_Befehl,K_LaengeDesBefehl)))));
#endif
	if(!K_DatenSenden(K_Terminalnummer,&K_Zieladresse,&K_Quelladresse,K_LaengeDesBefehl,K_Befehl,&K_LaengeDerAntwort,K_Antwort))
	{
		K_CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(K_Antwort[K_LaengeDerAntwort-2] <<8) | K_Antwort[K_LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	
	qDebug("QFrankCT_API_Leser %s: Statuskode vom Leser: 0x%X",Funktionsname.toAscii().data(),Ergebnis);
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;	
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_ChangeReferenceData(QByteArray datenfeld)
{
	K_ISO_VerifyOderISO_ChangeReferenceData=true;
	QFrankLesegeraet::Rueckgabecodes Ergebnis=ISO_Verify(datenfeld);
	K_ISO_VerifyOderISO_ChangeReferenceData=false;
	return Ergebnis;		
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_VerifySecure(QByteArray datenfeld)
{
	QString Funktionsname;
	if(!K_ISO_VerifySecureOderISO_ChangeReferenceDataSecure)
	{
		//false=SO_VerifySecure true=ISO_ChangeReferenceDataSecure
		Funktionsname="ISO_VerifySecure";
		K_Befehl[1]=0x18;
	}
	else
	{
		Funktionsname="ISO_ChangeReferenceDataSecure";
		K_Befehl[1]=0x19;
	}
	
	if(!K_VerbindungTesten(Funktionsname))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	if(!K_ISO_VerifySecureOderISO_ChangeReferenceDataSecure)
	{
		//mindestes ein Klasse 2 Leser(VerifySecure)
		if(!(K_Lesersicherheit>QFrankLesegeraet::Klasse1 && K_Lesersicherheit<=QFrankLesegeraet::Klasse4))
		{
#ifndef QT_NO_DEBUG
			qCritical(qPrintable(trUtf8("QFrankCT_API_Leser %1: Sichere Pineingabe nur mit Geräten der Klasse 2 oder höher möglich.","debug").arg(Funktionsname)));	
#endif
		return QFrankLesegeraet::ParameterFalsch;
		}
	}
	else
	{
		//mindestes ein Klasse 3 Leser(Update..Sec..)
		if(!(K_Lesersicherheit>QFrankLesegeraet::Klasse2 && K_Lesersicherheit<=QFrankLesegeraet::Klasse4))
		{
#ifndef QT_NO_DEBUG
			qCritical(qPrintable(trUtf8("QFrankCT_API_Leser %1: Sicherer Pinwechsel nur mit Geräten der Klasse 3 oder höher möglich","debug").arg(Funktionsname)));	
#endif
		return QFrankLesegeraet::ParameterFalsch;
		}
	}
	
	
	//hat das Datenfeld mehr als 256 Stellen??
	if (datenfeld.size()>256 || datenfeld.at(0)!=0x52)
	{
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(QString("QFrankCT_API_Leser %1: Format des Datenfeldes ist falsch.\r\n\t Datenfeld: %2")
									.arg(Funktionsname).arg(K_FeldNachHex(QByteArray((char*)K_Befehl,K_LaengeDesBefehl)))));
#endif
		return QFrankLesegeraet::ParameterFalsch;
	}
	K_Befehl[0]=0x20;
	K_Befehl[2]=0x01; //Karte1
	K_Befehl[3]=0x00; //Pinpad
	K_Befehl[4]=datenfeld.size();
	K_LaengeDesBefehl=datenfeld.size()+5;
	K_LaengeDerAntwort=sizeof(K_Antwort);
	K_Zieladresse=0; // 0=Slot1  1=Termimal 2=Slot2
	K_Quelladresse=2;
	memcpy(K_Befehl+5,datenfeld.data(),datenfeld.size());//Kopieren des QByteArray in's Befehlsfeld
#ifndef QT_NO_DEBUG
	qDebug(qPrintable(QString("QFrankCT_API_Leser %1: Datenfeld: %2").arg(Funktionsname).arg(K_FeldNachHex(QByteArray((char*)K_Befehl,K_LaengeDesBefehl)))));
#endif
	if(!K_DatenSenden(K_Terminalnummer,&K_Zieladresse,&K_Quelladresse,K_LaengeDesBefehl,K_Befehl,&K_LaengeDerAntwort,K_Antwort))
	{
		K_CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(K_Antwort[K_LaengeDerAntwort-2] <<8) | K_Antwort[K_LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	qDebug("QFrankCT_API_Leser %s: Statuskode vom Leser: 0x%X",Funktionsname.toAscii().data(),Ergebnis);
#endif
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_ChangeReferenceDataSecure(QByteArray datenfeld)
{
	K_ISO_VerifySecureOderISO_ChangeReferenceDataSecure=true;
	QFrankLesegeraet::Rueckgabecodes Ergebnis=ISO_VerifySecure(datenfeld);
	K_ISO_VerifySecureOderISO_ChangeReferenceDataSecure=false;
	
	return Ergebnis;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::KarteEntfernen()
{
	if(!K_VerbindungTesten("KarteEntfernen"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	K_Befehl[0]=0x20;
	K_Befehl[1]=0x15;
	K_Befehl[2]=0x01; //Slot1
	K_Befehl[3]=0x03; // Text wenn Display da is sowie mit optischen und akustischen Signal
	K_Befehl[4]=0x01; //Länge des Anhangs
	K_Befehl[5]=0x04; //Wartezeit für das Entfernen
	K_LaengeDesBefehl=6;
	K_Zieladresse=1;
	K_Quelladresse=2;
	K_LaengeDerAntwort=sizeof(K_Antwort);
	if(!K_DatenSenden(K_Terminalnummer,&K_Zieladresse,&K_Quelladresse,K_LaengeDesBefehl,K_Befehl,&K_LaengeDerAntwort,K_Antwort))
	{
		K_CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(K_Antwort[K_LaengeDerAntwort-2] <<8) | K_Antwort[K_LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	qDebug("QFrankCT_API_Leser KarteEntfernen: Statuskode vom Leser: 0x%X",Ergebnis);
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::KarteAnfordern(QByteArray &ATR)
{
	if(!K_VerbindungTesten("KarteAnfordern"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	K_Befehl[0]=0x20; 
	K_Befehl[1]=0x12;
	K_Befehl[2]=0x01;//Slot1
	K_Befehl[3]=0x01;// Text wenn Display da is und als K_Antwort den ganzen ATR
	K_Befehl[4]=0x01; //Länge des Anhangs
	K_Befehl[5]=0x04;//Wartezeit auf Karte in Sekunden 00=keine 4=Sekunden
	K_Befehl[6]=0x00;//Alles zurückliefern.
	K_LaengeDesBefehl=7;
	K_Zieladresse=1;
	K_Quelladresse=2;
	K_LaengeDerAntwort=sizeof(K_Antwort);
	if(!K_DatenSenden(K_Terminalnummer,&K_Zieladresse,&K_Quelladresse,K_LaengeDesBefehl,K_Befehl,&K_LaengeDerAntwort,K_Antwort))
	{
		K_CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswerten des Ergebnisses
	uint Ergebnis=(K_Antwort[K_LaengeDerAntwort-2] <<8) | K_Antwort[K_LaengeDerAntwort-1];
	if(Ergebnis==QFrankLesegeraet::CommandSuccessful || Ergebnis==QFrankLesegeraet::CommandSuccessfulAsynchron)
	{
		//Karte wurde gefunden
#ifndef QT_NO_DEBUG
		qDebug(qPrintable(QString("QFrankCT_API_Leser KarteAnfordern: ATR: %1").arg(K_FeldNachHex(QByteArray((char*)K_Antwort,K_LaengeDerAntwort)))));
#endif
		ATR=QByteArray((char*)K_Antwort,K_LaengeDerAntwort-2);
	}
#ifndef QT_NO_DEBUG
	else
	{
		qCritical(qPrintable(QString("QFrankCT_API_Leser KarteAnfordern: gescheitert ATR: %1")
									.arg(K_FeldNachHex(QByteArray((char*)K_Antwort,K_LaengeDerAntwort)))));
	}
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
}

QFrankLesegeraet::Leserklasse QFrankCT_API_Leser::Sicherheitsklasse()
{
	if(!K_VerbindungTesten("Sicherheitsklasse"))
		return QFrankLesegeraet::KlasseUnbekannt;
#ifndef QT_NO_DEBUG
	switch(K_Lesersicherheit)
	{
		case QFrankLesegeraet::Klasse1:
										qDebug("QFrankCT_API_Leser Sicherheitsklasse: Leser Klasse 1");
										break;
		case QFrankLesegeraet::Klasse2:
										qDebug("QFrankCT_API_Leser Sicherheitsklasse: Leser Klasse 2");
										break;
		case QFrankLesegeraet::Klasse3:
										qDebug("QFrankCT_API_Leser Sicherheitsklasse: Leser Klasse 3");
										break;
		case QFrankLesegeraet::Klasse4:
										qDebug("QFrankCT_API_Leser Sicherheitsklasse: Leser Klasse 4");
										break;
		case QFrankLesegeraet::KlasseUnbekannt:
										qDebug("QFrankCT_API_Leser Sicherheitsklasse: Leser Klasse unbekannt");
										break;
		default:
										qFatal("QFrankCT_API_Leser Sicherheitsklasse: Variable Lesersicherheit hat einen unbekanten Wert!!");
										break;
	}
#endif
	return K_Lesersicherheit;
}

uint QFrankCT_API_Leser::PortnummerHohlen()
{
	return K_Portnummer;
}

void QFrankCT_API_Leser::PortnummerSetzen(uint portnummer)
{
	K_Portnummer=portnummer;
}

QString QFrankCT_API_Leser::TreiberdateiHohlen()
{
	return K_Treiberdatei;
}

void QFrankCT_API_Leser::TreiberdateiSetzen(QString treiber)
{
	K_Treiberdatei=treiber;
}

bool QFrankCT_API_Leser::K_VerbindungTesten(QString programmteil)
{
	if(!K_VerbindungZumKartenleser)
	{
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(QString("%1 K_VerbindungTesten: %2 gescheitert, da der Leser nicht initialisiert ist.").arg(objectName()).arg(programmteil)));
#endif
		return false;
	}
	return true;
}

void QFrankCT_API_Leser::K_CT_API_schliessen()
{
	if(!K_VerbindungZumKartenleser)
	{
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(trUtf8("QFrankCT_API_Leser K_CT_API_schliessen: CT-Close nicht möglich, da keine Verbindung zum Leser.","debug")));
#endif
		return;
	}
	char Rueckgabecode=K_MeinCT_close(K_Terminalnummer);
#ifndef QT_NO_DEBUG
	if(Rueckgabecode!=0)
		qCritical(qPrintable(trUtf8("QFrankCT_API_Leser K_CT_API_schliessen: CT-Close mit Fehler beendet. Rückgabe: %1","debug").arg((int)Rueckgabecode)));
#endif
}

bool QFrankCT_API_Leser::K_DatenSenden(uint terminalnummer, uchar *ziel,uchar *quelle,\
									 ushort befehlslaenge,uchar *befehle,ushort *antwortlaenge,\
															uchar *antworten)
{
	char Ergebnis=K_MeinCT_data(terminalnummer,ziel,quelle,befehlslaenge,befehle,antwortlaenge,antworten);
	if(Ergebnis!=0)
	{
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(trUtf8("%1 K_DatenSenden: Befehl konnte nicht das das Terminal gesendet werden. Rückgabe: %2","debug")
									.arg(objectName()).arg((int)Ergebnis)));
#endif
		return false;
	}
	return true;
}

bool QFrankCT_API_Leser::K_DatenfeldZuKlein(int groesse,QByteArray &Feld,QString programmteil)
{
	if(Feld.size()>groesse)
	{
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(trUtf8("%1 K_DatenfeldZuKlein: %2 Das übergebene Datenfeld ist zu gross!!","debug").arg(objectName()).arg(programmteil)));
#endif
		return true;
	}
	return false;
}

#ifndef QT_NO_DEBUG
QString QFrankCT_API_Leser::K_FeldNachHex(QByteArray feld)
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
