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

#include "PCSC-Leser.h"


/*!
	\ingroup lesegeraete
	\class QFrankPCSC_Leser
	\brief Die QFrankPCSC_Leser Klasse stellt den Zugriff auf Lesegeräte per PC/SC bereit
	
*/
QFrankPCSC_Leser::QFrankPCSC_Leser(QObject* eltern):QFrankLesegeraet(eltern)
{
	setObjectName("QFrankPCSC_Leser");
	K_Lesersicherheit=QFrankLesegeraet::KlasseUnbekannt;
	K_VerbindungZumLeser=false;
//Warnung bei DEBUG
#ifndef QT_NO_DEBUG
	qWarning(qPrintable(trUtf8("QFrankPCSC_Leser Konstruktor: WARNUNG Debugversion wird benutzt.\r\nEs könnten sicherheitsrelevante Daten ausgegeben werden!!!!!"
								,"debug")));
#endif
	//Initialisiere Kontext
	K_PCSC_Kontext=NULL;
	K_Kartenverbindung=NULL;
	K_RueckegabePCSC=SCardEstablishContext(SCARD_SCOPE_SYSTEM,NULL,NULL,&K_PCSC_Kontext);
	if (K_RueckegabePCSC!=SCARD_S_SUCCESS)
#ifndef QT_NO_DEBUG
	{
		qWarning(qPrintable(trUtf8("QFrankPCSC_Leser Konstruktor: Konnt keine Verbindung zum PC/SC System erstellen.\r\nRückgabekode: %1","debug")
								  .arg(K_RueckegabePCSC)));
	}
	else
		qDebug("QFrankPCSC_Leser Konstruktor: Verbindung zum PC/SC System hergestellt.");
#else
		return;
#endif	
}

ulong QFrankPCSC_Leser::Version()
{
	return PCSC_Version;
}

QFrankPCSC_Leser::~QFrankPCSC_Leser()
{
	//Vom PC/SC System trennen
	if(K_PCSC_Kontext!=NULL)
	{
		//Karte auswerfen(falls in Benutzung)
		if(K_Kartenverbindung!=NULL)
		{
			K_RueckegabePCSC=SCardDisconnect(K_Kartenverbindung,SCARD_EJECT_CARD);
#ifndef QT_NO_DEBUG
			if(K_RueckegabePCSC!=SCARD_S_SUCCESS)
				qWarning(qPrintable(trUtf8("QFrankPCSC_Leser Destruktor: Fehler beim Auswerfen der Karte. Rückgabekode: %1","debug").arg(K_RueckegabePCSC)));
			else
				qDebug("QFrankPCSC_Leser Destruktor: Karte ausgeworfen");
#endif
		}
		K_RueckegabePCSC=SCardReleaseContext(K_PCSC_Kontext);
#ifndef QT_NO_DEBUG
		if(K_RueckegabePCSC!=SCARD_S_SUCCESS)
			qWarning(qPrintable(trUtf8("QFrankPCSC_Leser Destruktor: Fehler beim Trennen vom PC/SC System. Rückgabekode: %1","debug").arg(K_RueckegabePCSC)));
		else
			qDebug("QFrankPCSC_Leser Destruktor: Verbindung zum PC/SC System getrennt");
#endif
	}
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::LeserInitialisieren()
{
	//Haben wir eine Verbindung zum PC/SC System??
	if (K_PCSC_Kontext==NULL)
		return QFrankLesegeraet::LeserNichtInitialisiert;
#ifndef QT_NO_DEBUG
	qDebug("QFrankPCSC_Leser LeserInitialisieren: Verbinde mit PC/SC Leser");
#endif

	//Liste abhohlen
	//Unix+Windows Unter Windows würder es zwar kürzer gehen aber so ist es einheitlich
	LPSTR Geraeteliste,EinGeraet;          
	//Länge der Liste Ermitteln
	DWORD LaengeDerListe;
	SCardListReaders( K_PCSC_Kontext, NULL, NULL, &LaengeDerListe);
	Geraeteliste=(LPSTR)malloc(sizeof(char) * LaengeDerListe);
	if(SCardListReaders(K_PCSC_Kontext,NULL,Geraeteliste,&LaengeDerListe)!=SCARD_S_SUCCESS)
	{
#ifndef QT_NO_DEBUG
		qCritical("QFrankPCSC_Leser LeserInitialisieren: Kein PC/SC Leser gefunden");
#endif	
		K_VerbindungZumLeser=false;
		free(Geraeteliste);
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	QStringList Geraete;
	//Die Liste bestet aus /0 terminierten Strings und ended mit einer doppelten /0
	EinGeraet = Geraeteliste;
    while (strlen(EinGeraet))
    {
		Geraete<<QString(EinGeraet);
        //auf zum Nächsten:)
        EinGeraet += strlen(EinGeraet);
    }	
	free(Geraeteliste);
	if(Geraete.isEmpty())
#ifndef QT_NO_DEBUG
	{
		qCritical("QFrankPCSC_Leser LeserInitialisieren: Die Liste der Leser ist leer");
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
#else
		return QFrankLesegeraet::LeserNichtInitialisiert;
#endif
#ifndef QT_NO_DEBUG
	qDebug(qPrintable(QString("QFrankPCSC_Leser LeserInitialisieren: gefundene Leser: %1").arg(Geraete.join(";"))));
#endif	
	//Mit dem 1. Leser verbinden
	SCARD_READERSTATE Leserstatus;
	Leserstatus.szReader=strdup(Geraete.at(0).toLocal8Bit());
	Leserstatus.dwCurrentState = SCARD_STATE_UNAWARE;
	K_RueckegabePCSC=SCardGetStatusChange(K_PCSC_Kontext,1,&Leserstatus,1);
	if(K_RueckegabePCSC!=SCARD_S_SUCCESS)
#ifndef QT_NO_DEBUG
	{
		qCritical(qPrintable(trUtf8("QFrankPCSC_Leser LeserInitialisieren: konnte den Leser nicht ansprechen. Rückgabekode: 0x%1","debug")
									.arg(K_RueckegabePCSC,0,16)));
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
#else
		return QFrankLesegeraet::LeserNichtInitialisiert;
#endif
	//schauen wir mal ob der Leser bereit ist. Entweder ist eine Karte drin oder nicht:)
	if( !(((Leserstatus.dwEventState&SCARD_STATE_CHANGED) && (Leserstatus.dwEventState&SCARD_STATE_EMPTY)) ||
		((Leserstatus.dwEventState&SCARD_STATE_CHANGED) && (Leserstatus.dwEventState&SCARD_STATE_PRESENT))))
#ifndef QT_NO_DEBUG
	{
		qCritical(qPrintable(trUtf8("QFrankPCSC_Leser LeserInitialisieren: Der Leser ist nicht bereit. Status: 0x%1","debug").arg(Leserstatus.dwEventState,0,16)));
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
#else
		return QFrankLesegeraet::LeserNichtInitialisiert;
#endif
#ifndef QT_NO_DEBUG
	qDebug("QFrankPCSC_Leser LeserInitialisieren: Leser bereit");
#endif
	/*
		Ermitteln der Sicherheitsklasse nicht möglich, da weder Windows noch Linux den benötigten Teil
		der PC/SC Norm enthalten. Versuche dies Nachzubilden, führen unter Windows teilweise zum einfrieren
		des kompletten PC/SC Systems(SmartCard Dienst reagiert nicht mehr). Unter Linux führen die Befehel zu
		Fehlermeldungen.
	*/
	K_VerbindungZumLeser=true;
	return QFrankLesegeraet::CommandSuccessful;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::ISO_SelectFile(QByteArray datenfeld)
{	
#ifndef QT_NO_DEBUG
	qDebug(qPrintable(QString("Select File Datenfeld:").arg(K_FeldNachHex(datenfeld))));
#endif	
#ifndef QT_NO_DEBUG
	//qDebug("Select File ergab: %X",Ergebnis);
#endif
	//return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::ISO_ReadBinary(QByteArray datenfeld,QByteArray &zielfeld)
{
	//return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::ISO_UpdateBinary(QByteArray datenfeld)
{	
#ifndef QT_NO_DEBUG
	//qDebug("ISO_UpdateBinary ergab: %X",Ergebnis);
#endif
	//return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;	
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::ISO_Verify(QByteArray datenfeld)
{	
	//Bei Geräten mit Tastatur sollte besser die Secure Ausführung benutzt werden
	if(K_Lesersicherheit>QFrankLesegeraet::Klasse1 && K_Lesersicherheit<QFrankLesegeraet::KlasseUnbekannt)
	{
		qWarning("Das Lesegerät kennt die sichere Pineingabe.\r\nDiese sollte auch benutzt werden!!");
	}
	//hat das Datenfeld mehr als 256 Stellen??
	if (datenfeld.size()>256)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"ISO_Verify die PIN ist zu lang.";
#endif
		return QFrankLesegeraet::ParameterFalsch;
	}
	
#ifndef QT_NO_DEBUG
	//qDebug("ISO_Verify ergab: %X",Ergebnis);
#endif
	//return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;	
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::ISO_ChangeReferenceData(QByteArray datenfeld)
{
	return QFrankLesegeraet::ParameterFalsch;		
}

/*
QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::ISO_VerifySecure(QByteArray datenfeld)
{
	//mindestes ein Klasse 2 Leser(VerifySecure)
	if(!(K_Lesersicherheit>QFrankLesegeraet::Klasse1 && K_Lesersicherheit<=QFrankLesegeraet::Klasse4))
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"ISO_VerifySecure sichere Pineingabe nur mit Geräten der Klasse 2 oder höher möglich";		
#endif
		return QFrankLesegeraet::ParameterFalsch;
	}
	//hat das Datenfeld mehr als 256 Stellen??
	if (datenfeld.size()>256 || datenfeld.at(0)!=0x52)
	{
#ifndef QT_NO_DEBUG
		qDebug()<<"ISO_VerifySecure Format des Datenfeldes ist falsch.";
		//qDebug()<<FeldNachHex(QByteArray((char*)Befehl,LaengeDesBefehl));
#endif
		return QFrankLesegeraet::ParameterFalsch;
	}
#ifndef QT_NO_DEBUG
	//qDebug("%ISO_VerifySecure ergab: %X",Ergebnis);
#endif
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::ISO_ChangeReferenceDataSecure(QByteArray datenfeld)
{
	return QFrankLesegeraet::ParameterFalsch;
}*/

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::KarteEntfernen()
{	
#ifndef QT_NO_DEBUG
	//qDebug("Karte auswerfen ergab: %X",Ergebnis);
#endif
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::UniversalIO(const QByteArray &daten, QByteArray &antwort)
{
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::KarteAnfordern(QByteArray &ATR)
{
	return QFrankLesegeraet::ParameterFalsch;
}

/*
QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::SicherePineingabe(const QByteArray &kartenbefehl)
{
	if(!(K_Lesersicherheit>QFrankLesegeraet::Klasse1 && K_Lesersicherheit<=QFrankLesegeraet::Klasse4))
	{
#ifndef QT_NO_DEBUG
		qDebug("QFrankPCSC_Leser SicherePineingabe: nur mit Geräten der Klasse 2 oder höher möglich");		
#endif
	return QFrankLesegeraet::ParameterFalsch;
	}
	//Datfeld darf nicht größer als 255 Bytes sein
	if(kartenbefehl.size()>255)
	{
#ifndef QT_NO_DEBUG
		qDebug("QFrankPCSC_Leser SicherePineingabe: Kartenbefehl zu lang");		
#endif
		return QFrankLesegeraet::ParameterFalsch;
	}
	return QFrankLesegeraet::NichtImplementiert;
}*/

QFrankLesegeraet::Leserklasse QFrankPCSC_Leser::Sicherheitsklasse()
{
#ifndef QT_NO_DEBUG
	switch(K_Lesersicherheit)
	{
		case QFrankLesegeraet::Klasse1:
										qDebug("Lesser Klasse 1");
										break;
		case QFrankLesegeraet::Klasse2:
										qDebug("Lesser Klasse 2");
										break;
		case QFrankLesegeraet::Klasse3:
										qDebug("Lesser Klasse 3");
										break;
		case QFrankLesegeraet::Klasse4:
										qDebug("Lesser Klasse 4");
										break;
		case QFrankLesegeraet::KlasseUnbekannt:
										qDebug("Lesser Klasse unbekannt");
										break;
		default:
										qFatal("Variable Lesersicherheit hat einen unbekanten Wert!!");
										break;
	}
#endif
	return K_Lesersicherheit;
}

#ifndef QT_NO_DEBUG
QString QFrankPCSC_Leser::K_FeldNachHex(QByteArray feld)
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
