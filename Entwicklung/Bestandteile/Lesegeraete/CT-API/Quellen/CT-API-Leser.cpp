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
	Portnummer=1;
	Terminalnummer=1;
	VerbindungZumKartenleser=false;
	ISO_VerifyOderISO_ChangeReferenceData=false;
	ISO_VerifySecureOderISO_ChangeReferenceDataSecure=false;
	Lesersicherheit=QFrankLesegeraet::KlasseUnbekannt;
//Vordefiniert für Reiner SCT Leser
#ifdef Q_WS_WIN
	//Treiberdatei="G:\\Versuch\\cj\\ctFilter.dll";
	Treiberdatei=QString("%1%2").arg(getenv("windir")).arg("\\system32\\ctrsct32.dll");
#else
	Treiberdatei="/usr/lib/readers/libctapi-cyberjack.so";
#endif
	//Warnung bei DEBUG
#ifndef QT_NO_DEBUG
	qWarning("WARNUNG Debugversion wird benutzt.\r\nEs könnten sicherheitsrelevante Daten ausgegeben werden!!!!!");
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
#ifndef QT_NO_DEBUG
	qDebug()<<"Versuche den CT API Leser mit folgenden Werten zu initialisieren.\r\n"\
			<<"Treiber:"<<Treiberdatei<<"Port:"<<Portnummer<<"Terminalnummer:"<<Terminalnummer;
#endif
	MeinCT_init=(pCT_init)QLibrary::resolve(Treiberdatei, "CT_init");
	MeinCT_data=(pCT_data)QLibrary::resolve(Treiberdatei, "CT_data");
	MeinCT_close=(pCT_close)QLibrary::resolve(Treiberdatei, "CT_close");
	//wurden alle Funktionen gefunden??
	if(MeinCT_init==0 || MeinCT_data==0 || MeinCT_close==0)
	{
#ifndef QT_NO_DEBUG
		qDebug("Der CT-API Treiber konnte nicht geladen werden. Die Zeiger haben folgenden Werte:\r\n"\
			   "CT_init: 0x%X CT_data: 0x%X CT_close: 0x%X",MeinCT_init,MeinCT_data,MeinCT_close);
#endif
		return  QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Aufruf von CT-init
	char Rueckgabecode=MeinCT_init(Terminalnummer,Portnummer);
	if(Rueckgabecode!=0)
	{
#ifndef QT_NO_DEBUG
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
	LaengeDerAntwort=sizeof(Antwort); //Muss immer sein, da vom der CT DLL verbogen wird
	if(!DatenSenden(Terminalnummer,&Zieladresse,&Quelladresse,LaengeDesBefehl,Befehl,&LaengeDerAntwort,Antwort ))
	{
		CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswerten des Ergebnisses
	uint Ergebnis=(Antwort[LaengeDerAntwort-2] <<8) | Antwort[LaengeDerAntwort-1];
	if(Ergebnis!=QFrankLesegeraet::CommandSuccessful)
	{		
#ifndef QT_NO_DEBUG
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
#ifndef QT_NO_DEBUG
						qDebug()<<"Der Leser hat ein Display";
#endif
						break;
			case 0x50:
						Tastatur=true;
#ifndef QT_NO_DEBUG
						qDebug()<<"Der Leser hat eine Tastatur";
#endif
						break;
			case 0x01:
						Karteneinschub1=true;
#ifndef QT_NO_DEBUG
						qDebug()<<"Der Leser hat ein Karteneinschub";
#endif
						break;
			case 0x02:
						Karteneinschub2=true;
#ifndef QT_NO_DEBUG
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

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::SicherePineingabe(const QByteArray &kartenbefehl)
{
	if(!VerbindungTesten("SicherePineingabe"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	//mindestes ein Klasse 2 Leser(SicherePineingabe)
	if(!(Lesersicherheit>QFrankLesegeraet::Klasse1 && Lesersicherheit<=QFrankLesegeraet::Klasse4))
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankCT_API_Leser SicherePineingabe: nur mit Geräten der Klasse 2 oder höher möglich";		
#endif
	return QFrankLesegeraet::ParameterFalsch;
	}
	//Datfeld darf nicht größer als 255 Bytes sein
	if(kartenbefehl.size()>255)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankCT_API_Leser SicherePineingabe: Kartenbefehl zu lang";		
#endif
		return QFrankLesegeraet::ParameterFalsch;
	}
	Befehl[0]=0x20;
	Befehl[1]=0x18;
	Befehl[2]=0x01;//Karte im Slot 1
	Befehl[3]=0x00;//0x00=Pin,0x01=biometische Daten
	Befehl[4]=kartenbefehl.size();//Länge des Datenfeldes
	LaengeDesBefehl=kartenbefehl.size()+5;
	LaengeDerAntwort=sizeof(Antwort);
	Zieladresse=1; // 0=Slot1  1=Termimal 2=Slot2
	Quelladresse=2;
	//Kartenbefehl kopieren
	memcpy(Befehl+5,kartenbefehl.data(),kartenbefehl.size());
#ifndef QT_NO_DEBUG
	qDebug()<<"QFrankCT_API_Leser SicherePineingabe Kartenbefehl: "<<FeldNachHex(QByteArray((char*)Befehl,LaengeDesBefehl));
#endif
	if(!DatenSenden(Terminalnummer,&Zieladresse,&Quelladresse,LaengeDesBefehl,Befehl,&LaengeDerAntwort,Antwort))
	{
		CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(Antwort[LaengeDerAntwort-2] <<8) | Antwort[LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	qDebug("QFrankCT_API_Leser SicherePineingabe ergab: %X",Ergebnis);
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;	
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_SelectFile(QByteArray datenfeld)
{
	if(!VerbindungTesten("ISO_SelectFile"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	if(DatenfeldZuKlein(sizeof(Befehl)-2,datenfeld,"ISO_SelectFile"))
	{
		//Datenfeld passt nicht in das Befehlsfeld:(
		return QFrankLesegeraet::ParameterFalsch;
	}
	Befehl[0]=0x00;
	Befehl[1]=0xa4;
	LaengeDesBefehl=datenfeld.size()+2;
	Zieladresse=0; // 0=Slot1  1=Termimal 2=Slot2
	Quelladresse=2;
	LaengeDerAntwort=sizeof(Antwort);
	memcpy(Befehl+2,datenfeld.data(),datenfeld.size());//Kopieren des QByteArray in's Befehlsfeld
#ifndef QT_NO_DEBUG
	qDebug()<<"Select File Datenfeld:"<<FeldNachHex(QByteArray((char*)Befehl,LaengeDesBefehl));
#endif
	if(!DatenSenden(Terminalnummer,&Zieladresse,&Quelladresse,LaengeDesBefehl,Befehl,&LaengeDerAntwort,Antwort))
	{
		CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(Antwort[LaengeDerAntwort-2] <<8) | Antwort[LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	qDebug("Select File ergab: %X",Ergebnis);
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::UniversalIO(const QByteArray &daten, QByteArray &antwort)
{
	if(!VerbindungTesten("UniversalIO"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	//alles wird blind an den Leser übergeben
#ifndef QT_NO_DEBUG
	qDebug()<<"UniversalIO Senden:"<<FeldNachHex(daten);
#endif
	Zieladresse=0; // 0=Slot1  1=Termimal 2=Slot2
	Quelladresse=2;
	LaengeDerAntwort=sizeof(Antwort);
	LaengeDesBefehl=daten.size();
	//Kopieren des QByteArray in's Befehlsfeld
	memcpy(Befehl,daten.data(),daten.size());

	if(!DatenSenden(Terminalnummer,&Zieladresse,&Quelladresse,LaengeDesBefehl,Befehl,&LaengeDerAntwort,Antwort))
	{
		CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(Antwort[LaengeDerAntwort-2] <<8) | Antwort[LaengeDerAntwort-1];
	antwort.resize(LaengeDerAntwort);
	memcpy(antwort.data(),Antwort,LaengeDerAntwort);//In das Rückgabearray kopieren

#ifndef QT_NO_DEBUG
	qDebug("UniversalIO Rückgabecode: %X",Ergebnis);
	qDebug()<<"UniversalIO Empfangen"<<FeldNachHex(antwort);
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_ReadBinary(QByteArray datenfeld,QByteArray &zielfeld)
{
	if(!VerbindungTesten("ISO_ReadBinary"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	if(DatenfeldZuKlein(sizeof(Befehl)-2,datenfeld,"ISO_ReadBinary"))
	{
		//Datenfeld passt nicht in das Befehlsfeld:(
		return QFrankLesegeraet::ParameterFalsch;
	}
	Befehl[0]=0x00;
	Befehl[1]=0xb0;
	LaengeDesBefehl=datenfeld.size()+2;
	Zieladresse=0; // 0=Slot1  1=Termimal 2=Slot2
	Quelladresse=2;
	LaengeDerAntwort=sizeof(Antwort);
	memcpy(Befehl+2,datenfeld.data(),datenfeld.size());//Kopieren des QByteArray in's Befehlsfeld
#ifndef QT_NO_DEBUG
	qDebug()<<"Read Binary Datenfeld(Quelle):"<<FeldNachHex(QByteArray((char*)Befehl,LaengeDesBefehl));
#endif
	if(!DatenSenden(Terminalnummer,&Zieladresse,&Quelladresse,LaengeDesBefehl,Befehl,&LaengeDerAntwort,Antwort))
	{
		CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(Antwort[LaengeDerAntwort-2] <<8) | Antwort[LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	qDebug("Read Binary ergab: %X",Ergebnis);
#endif
	if(Ergebnis==QFrankLesegeraet::CommandSuccessful || Ergebnis==QFrankLesegeraet::WarningEOFbeforeLeBytes)
	{
		//Daten konnten gelesen werden
		zielfeld.resize(LaengeDerAntwort-2);// Die 2 Bytes Status werden nicht mit übergben.
		memcpy(zielfeld.data(),Antwort,LaengeDerAntwort-2);//In das Rückgabeaary kopieren
#ifndef QT_NO_DEBUG
	qDebug()<<"Read Binary Datenfeld(Ziel):"<<FeldNachHex(zielfeld);
#endif
	}
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_UpdateBinary(QByteArray datenfeld)
{
	if(!VerbindungTesten("ISO_UpdateBinary"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	if(DatenfeldZuKlein(sizeof(Befehl)-2,datenfeld,"ISO_UpdateBinary"))
	{
		//Datenfeld passt nicht in das Befehlsfeld:(
		return QFrankLesegeraet::ParameterFalsch;
	}
	//Datenlänge richtig angegeben??
	if(datenfeld.at(2)!=(datenfeld.size()-3))
	{
		//stimmt nicht
#ifndef QT_NO_DEBUG
		qDebug()<<"ISO_UpdateBinary Datenlänge falsch";
#endif
	return QFrankLesegeraet::ParameterFalsch;
	}
	Befehl[0]=0x00;
	Befehl[1]=0xd6;
	LaengeDesBefehl=datenfeld.size()+2;
	LaengeDerAntwort=sizeof(Antwort);
	Zieladresse=0; // 0=Slot1  1=Termimal 2=Slot2
	Quelladresse=2;
	memcpy(Befehl+2,datenfeld.data(),datenfeld.size());//Kopieren des QByteArray in's Befehlsfeld
#ifndef QT_NO_DEBUG
	qDebug()<<"ISO_UpdateBinary Datenfeld:"<<FeldNachHex(QByteArray((char*)Befehl,LaengeDesBefehl));
#endif
	if(!DatenSenden(Terminalnummer,&Zieladresse,&Quelladresse,LaengeDesBefehl,Befehl,&LaengeDerAntwort,Antwort))
	{
		CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(Antwort[LaengeDerAntwort-2] <<8) | Antwort[LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	qDebug("ISO_UpdateBinary ergab: %X",Ergebnis);
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;	
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_Verify(QByteArray datenfeld)
{
	QString Funktionsname;
	if(!ISO_VerifyOderISO_ChangeReferenceData)
	{
		//false=ISO_Verify true=ISO_ChangeReferenceData
		Funktionsname="ISO_Verify";
		Befehl[1]=0x20;
	}
	else
	{
		Funktionsname="ISO_ChangeReferenceData";
		Befehl[1]=0x24;
	}
	if(!VerbindungTesten(Funktionsname))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	//Bei Geräten mit Tastatur sollte besser die Secure Ausführung benutzt werden
	if(Lesersicherheit>QFrankLesegeraet::Klasse1 && Lesersicherheit<QFrankLesegeraet::KlasseUnbekannt)
	{
		qWarning("Das Lesegerät kennt die sichere Pineingabe.\r\nDiese sollte auch benutzt werden!!");
	}
	if(DatenfeldZuKlein(sizeof(Befehl)-4,datenfeld,Funktionsname))
	{
		//Datenfeld passt nicht in das Befehlsfeld:(
		return QFrankLesegeraet::ParameterFalsch;
	}
	//hat das Datenfeld mehr als 256 Stellen??
	if (datenfeld.size()>256)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<Funktionsname<< "die PIN ist zu lang.";
#endif
		return QFrankLesegeraet::ParameterFalsch;
	}
	Befehl[0]=0x00;
	Befehl[2]=0x00;
	Befehl[3]=0x00;
	Befehl[4]=datenfeld.size();
	LaengeDesBefehl=datenfeld.size()+5;
	LaengeDerAntwort=sizeof(Antwort);
	Zieladresse=0; // 0=Slot1  1=Termimal 2=Slot2
	Quelladresse=2;
	memcpy(Befehl+5,datenfeld.data(),datenfeld.size());//Kopieren des QByteArray in's Befehlsfeld
#ifndef QT_NO_DEBUG
	qDebug()<<Funktionsname<<"Datenfeld:"<<FeldNachHex(QByteArray((char*)Befehl,LaengeDesBefehl));
#endif
	if(!DatenSenden(Terminalnummer,&Zieladresse,&Quelladresse,LaengeDesBefehl,Befehl,&LaengeDerAntwort,Antwort))
	{
		CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(Antwort[LaengeDerAntwort-2] <<8) | Antwort[LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	
	qDebug("%s ergab: %X",Funktionsname.toAscii().data(),Ergebnis);
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;	
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_ChangeReferenceData(QByteArray datenfeld)
{
	ISO_VerifyOderISO_ChangeReferenceData=true;
	QFrankLesegeraet::Rueckgabecodes Ergebnis=ISO_Verify(datenfeld);
	ISO_VerifyOderISO_ChangeReferenceData=false;
	return Ergebnis;		
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_VerifySecure(QByteArray datenfeld)
{
	QString Funktionsname;
	if(!ISO_VerifySecureOderISO_ChangeReferenceDataSecure)
	{
		//false=SO_VerifySecure true=ISO_ChangeReferenceDataSecure
		Funktionsname="ISO_VerifySecure";
		Befehl[1]=0x18;
	}
	else
	{
		Funktionsname="ISO_ChangeReferenceDataSecure";
		Befehl[1]=0x19;
	}
	
	if(!VerbindungTesten(Funktionsname))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	if(!ISO_VerifySecureOderISO_ChangeReferenceDataSecure)
	{
		//mindestes ein Klasse 2 Leser(VerifySecure)
		if(!(Lesersicherheit>QFrankLesegeraet::Klasse1 && Lesersicherheit<=QFrankLesegeraet::Klasse4))
		{
#ifndef QT_NO_DEBUG
			qDebug()<<"ISO_VerifySecure sichere Pineingabe nur mit Geräten der Klasse 2 oder höher möglich";		
#endif
		return QFrankLesegeraet::ParameterFalsch;
		}
	}
	else
	{
		//mindestes ein Klasse 3 Leser(Update..Sec..)
		if(!(Lesersicherheit>QFrankLesegeraet::Klasse2 && Lesersicherheit<=QFrankLesegeraet::Klasse4))
		{
#ifndef QT_NO_DEBUG
			qDebug()<<"ISO_ChangeReferenceDataSecure sicherer Pinwechsel nur mit Geräten der Klasse 3 oder höher möglich";		
#endif
		return QFrankLesegeraet::ParameterFalsch;
		}
	}
	
	
	//hat das Datenfeld mehr als 256 Stellen??
	if (datenfeld.size()>256 || datenfeld.at(0)!=0x52)
	{
#ifndef QT_NO_DEBUG
		qDebug()<< Funktionsname<<"Format des Datenfeldes ist falsch.";
		qDebug()<<FeldNachHex(QByteArray((char*)Befehl,LaengeDesBefehl));
#endif
		return QFrankLesegeraet::ParameterFalsch;
	}
	Befehl[0]=0x20;
	Befehl[2]=0x01; //Karte1
	Befehl[3]=0x00; //Pinpad
	Befehl[4]=datenfeld.size();
	LaengeDesBefehl=datenfeld.size()+5;
	LaengeDerAntwort=sizeof(Antwort);
	Zieladresse=0; // 0=Slot1  1=Termimal 2=Slot2
	Quelladresse=2;
	memcpy(Befehl+5,datenfeld.data(),datenfeld.size());//Kopieren des QByteArray in's Befehlsfeld
#ifndef QT_NO_DEBUG
	qDebug()<<Funktionsname<<"Datenfeld:"<<FeldNachHex(QByteArray((char*)Befehl,LaengeDesBefehl));
#endif
	if(!DatenSenden(Terminalnummer,&Zieladresse,&Quelladresse,LaengeDesBefehl,Befehl,&LaengeDerAntwort,Antwort))
	{
		CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(Antwort[LaengeDerAntwort-2] <<8) | Antwort[LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	qDebug("%s ergab: %X",Funktionsname.toAscii().data(),Ergebnis);
#endif
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::ISO_ChangeReferenceDataSecure(QByteArray datenfeld)
{
	ISO_VerifySecureOderISO_ChangeReferenceDataSecure=true;
	QFrankLesegeraet::Rueckgabecodes Ergebnis=ISO_VerifySecure(datenfeld);
	ISO_VerifySecureOderISO_ChangeReferenceDataSecure=false;
	
	return Ergebnis;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::KarteEntfernen()
{
	if(!VerbindungTesten("KarteEntfernen"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	Befehl[0]=0x20;
	Befehl[1]=0x15;
	Befehl[2]=0x01; //Slot1
	Befehl[3]=0x03; // Text wenn Display da is sowie mit optischen und akustischen Signal
	Befehl[4]=0x01; //Länge des Anhangs
	Befehl[5]=0x04; //Wartezeit für das Entfernen
	LaengeDesBefehl=6;
	Zieladresse=1;
	Quelladresse=2;
	LaengeDerAntwort=sizeof(Antwort);
	if(!DatenSenden(Terminalnummer,&Zieladresse,&Quelladresse,LaengeDesBefehl,Befehl,&LaengeDerAntwort,Antwort))
	{
		CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswertung
	uint Ergebnis=(Antwort[LaengeDerAntwort-2] <<8) | Antwort[LaengeDerAntwort-1];
#ifndef QT_NO_DEBUG
	qDebug("Karte auswerfen ergab: %X",Ergebnis);
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
}

QFrankLesegeraet::Rueckgabecodes QFrankCT_API_Leser::KarteAnfordern(QByteArray &ATR)
{
	if(!VerbindungTesten("KarteAnfordern"))
		return QFrankLesegeraet::LeserNichtInitialisiert;
	Befehl[0]=0x20; 
	Befehl[1]=0x12;
	Befehl[2]=0x01;//Slot1
	Befehl[3]=0x01;// Text wenn Display da is und als Antwort den ganzen ATR
	Befehl[4]=0x01; //Länge des Anhangs
	Befehl[5]=0x04;//Wartezeit auf Karte in Sekunden 00=keine 4=Sekunden
	Befehl[6]=0x00;//Alles zurückliefern.
	LaengeDesBefehl=7;
	Zieladresse=1;
	Quelladresse=2;
	LaengeDerAntwort=sizeof(Antwort);
	if(!DatenSenden(Terminalnummer,&Zieladresse,&Quelladresse,LaengeDesBefehl,Befehl,&LaengeDerAntwort,Antwort))
	{
		CT_API_schliessen();
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Auswerten des Ergebnisses
	uint Ergebnis=(Antwort[LaengeDerAntwort-2] <<8) | Antwort[LaengeDerAntwort-1];
	if(Ergebnis==QFrankLesegeraet::CommandSuccessful || Ergebnis==QFrankLesegeraet::CommandSuccessfulAsynchron)
	{
		//Karte wurde gefunden
#ifndef QT_NO_DEBUG
		qDebug()<<"Karte anfordern ergab:"<<FeldNachHex(QByteArray((char*)Antwort,LaengeDerAntwort));
#endif
		ATR=QByteArray((char*)Antwort,LaengeDerAntwort-2);
	}
#ifndef QT_NO_DEBUG
	else
	{
		qDebug()<<"Karte anfordern gescheitert"<<FeldNachHex(QByteArray((char*)Antwort,LaengeDerAntwort));
	}
#endif
	return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
}

QFrankLesegeraet::Leserklasse QFrankCT_API_Leser::Sicherheitsklasse()
{
	if(!VerbindungTesten("Sicherheitsklasse"))
		return QFrankLesegeraet::KlasseUnbekannt;
#ifndef QT_NO_DEBUG
	switch(Lesersicherheit)
	{
		case QFrankLesegeraet::Klasse1:
										qDebug()<<"QFrankCT_API_Leser: Leser Klasse 1";
										break;
		case QFrankLesegeraet::Klasse2:
										qDebug()<<"QFrankCT_API_Leser: Leser Klasse 2";
										break;
		case QFrankLesegeraet::Klasse3:
										qDebug()<<"QFrankCT_API_Leser: Leser Klasse 3";
										break;
		case QFrankLesegeraet::Klasse4:
										qDebug()<<"QFrankCT_API_Leser: Leser Klasse 4";
										break;
		case QFrankLesegeraet::KlasseUnbekannt:
										qDebug()<<"QFrankCT_API_Leser: Leser Klasse unbekannt";
										break;
		default:
										qFatal("QFrankCT_API_Leser: Variable Lesersicherheit hat einen unbekanten Wert!!");
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
#ifndef QT_NO_DEBUG
		qDebug()<<objectName()<<programmteil<<"gescheitert, da der Leser nicht initialisiert.";
#endif
		return false;
	}
	return true;
}

void QFrankCT_API_Leser::CT_API_schliessen()
{
	if(!VerbindungZumKartenleser)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"CT-Close nicht möglich, da keine Verbindung zum Leser.";
#endif
		return;
	}
	char Rueckgabecode=MeinCT_close(Terminalnummer);
#ifndef QT_NO_DEBUG
	if(Rueckgabecode!=0)
		qDebug()<<"CT-Close mit Fehler beendet. Rückgabe:"<<(int)Rueckgabecode;
#endif
}

bool QFrankCT_API_Leser::DatenSenden(uint terminalnummer, uchar *ziel,uchar *quelle,\
									 ushort befehlslaenge,uchar *befehle,ushort *antwortlaenge,\
															uchar *antworten)
{
	char Ergebnis=MeinCT_data(terminalnummer,ziel,quelle,befehlslaenge,befehle,antwortlaenge,antworten);
	if(Ergebnis!=0)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<objectName()<<"Befehl konnte nicht das das Terminal gesendet werden. Rückgabe:"<<(int)Ergebnis;
#endif
		return false;
	}
	return true;
}

bool QFrankCT_API_Leser::DatenfeldZuKlein(int groesse,QByteArray &Feld,QString programmteil)
{
	if(Feld.size()>groesse)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<QString("%1 %2: Das übergebene Datenfeld ist zu gross!!").arg(objectName()).arg(programmteil);
#endif
		return true;
	}
	return false;
}

#ifndef QT_NO_DEBUG
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
