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
#ifdef Q_WS_WIN
	#include "PCSC-Sicherheit.h"
#else
	#include <reader.h>
#endif

/*!
	\ingroup lesegeraete
	\class QFrankPCSC_Leser
	\brief Die QFrankPCSC_Leser Klasse stellt den Zugriff auf Lesegeräte per PC/SC bereit
	
*/
QFrankPCSC_Leser::QFrankPCSC_Leser(QObject* eltern):QFrankLesegeraet(eltern)
{
	setObjectName("QFrankPCSC_Leser");
	K_Lesersicherheit=QFrankLesegeraet::KlasseUnbekannt;
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
		qWarning(qPrintable(trUtf8("QFrankPCSC_Leser Konstruktor: Konnt keine Verbindung zum PC/SC System erstellen.\r\nGrund: %1","debug")
								  .arg(qPrintable(K_RueckegabePCSCNachText(K_RueckegabePCSC)))));
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
		KarteEntfernen();
		K_RueckegabePCSC=SCardReleaseContext(K_PCSC_Kontext);
#ifndef QT_NO_DEBUG
		if(K_RueckegabePCSC!=SCARD_S_SUCCESS)
			qWarning(qPrintable(trUtf8("QFrankPCSC_Leser Destruktor: Fehler beim Trennen vom PC/SC System. Grund: %1","debug")
										.arg(qPrintable(K_RueckegabePCSCNachText(K_RueckegabePCSC)))));
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
	K_NameDesLesers=Geraete.at(0);
	SCARD_READERSTATE Leserstatus;
	Leserstatus.szReader=strdup(K_NameDesLesers.toLocal8Bit());
	Leserstatus.dwCurrentState = SCARD_STATE_UNAWARE;
	K_RueckegabePCSC=SCardGetStatusChange(K_PCSC_Kontext,1,&Leserstatus,1);
	if(K_RueckegabePCSC!=SCARD_S_SUCCESS)
	{
		K_NameDesLesers="";
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(trUtf8("QFrankPCSC_Leser LeserInitialisieren: konnte den Leser nicht ansprechen. Grund: %1","debug")
									.arg(qPrintable(K_RueckegabePCSCNachText(K_RueckegabePCSC)))));
#endif
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//schauen wir mal ob der Leser bereit ist. Entweder ist eine Karte drin oder nicht:)
	if( !(((Leserstatus.dwEventState&SCARD_STATE_CHANGED) && (Leserstatus.dwEventState&SCARD_STATE_EMPTY)) ||
		((Leserstatus.dwEventState&SCARD_STATE_CHANGED) && (Leserstatus.dwEventState&SCARD_STATE_PRESENT))))
	{
		K_NameDesLesers="";
#ifndef QT_NO_DEBUG
		qCritical(qPrintable(trUtf8("QFrankPCSC_Leser LeserInitialisieren: Der Leser ist nicht bereit. Status: 0x%1","debug").arg(Leserstatus.dwEventState,0,16)));
#endif
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
#ifndef QT_NO_DEBUG
	qDebug("QFrankPCSC_Leser LeserInitialisieren: Leser bereit");
#endif
	/*
		Ermitteln der Sicherheitsklasse ist erst möglich, wenn eine Karte eingelegt wurde.
	*/
	return QFrankLesegeraet::CommandSuccessful;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::ISO_SelectFile(QByteArray datenfeld)
{
	return QFrankLesegeraet::NichtImplementiert;
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
	return QFrankLesegeraet::NichtImplementiert;
	//return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::ISO_UpdateBinary(QByteArray datenfeld)
{
	return QFrankLesegeraet::NichtImplementiert;
#ifndef QT_NO_DEBUG
	//qDebug("ISO_UpdateBinary ergab: %X",Ergebnis);
#endif
	//return (QFrankLesegeraet::Rueckgabecodes) Ergebnis;	
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::ISO_Verify(QByteArray datenfeld)
{	
	return QFrankLesegeraet::NichtImplementiert;
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
	return QFrankLesegeraet::NichtImplementiert;		
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::ISO_VerifySecure(QByteArray datenfeld)
{
	return QFrankLesegeraet::NichtImplementiert;
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
	return QFrankLesegeraet::NichtImplementiert;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::KarteEntfernen()
{
	if(K_Kartenverbindung!=NULL)
	{
		//K_RueckegabePCSC=SCardDisconnect(K_Kartenverbindung,SCARD_RESET_CARD);
		K_RueckegabePCSC=SCardDisconnect(K_Kartenverbindung,SCARD_EJECT_CARD);
		if(K_RueckegabePCSC!=SCARD_S_SUCCESS)
		{
#ifndef QT_NO_DEBUG
			qWarning(qPrintable(trUtf8("QFrankPCSC_Leser KarteEntfernen: Fehler beim Auswerfen der Karte. Grund: %1","debug")
											.arg(qPrintable(K_RueckegabePCSCNachText(K_RueckegabePCSC)))));
#endif
			return QFrankLesegeraet::Error;
		}
		else
		{
#ifndef QT_NO_DEBUG
			qDebug("QFrankPCSC_Leser KarteEntfernen: Karte ausgeworfen");
#endif
			K_Kartenverbindung=NULL;
			return QFrankLesegeraet::CommandSuccessful;
		}
	}
#ifndef QT_NO_DEBUG
		qCritical("%s KarteEntfernen: Keine Verbindung mit einer Karte.",qPrintable(objectName()));
#endif
	return QFrankLesegeraet::ParameterFalsch;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::UniversalIO(const QByteArray &daten, QByteArray &antwort)
{
	return QFrankLesegeraet::NichtImplementiert;
}

QFrankLesegeraet::Rueckgabecodes QFrankPCSC_Leser::KarteAnfordern(QByteArray &ATR)
{
	//Haben wir schon eine aktive Karte??
	if(K_Kartenverbindung!=NULL)
	{
#ifndef QT_NO_DEBUG
		qCritical("%s KarteAnfordern: Es ist bereits eine Karte aktiv.",qPrintable(objectName()));
#endif
		return QFrankLesegeraet::KarteBetreitsAktiv;
	}
	//Ist der Leser bereit??
	if(K_NameDesLesers=="")
	{
#ifndef QT_NO_DEBUG
		qCritical("%s KarteAnfordern: Der Leser ist nicht bereit.",qPrintable(objectName()));
#endif
		return QFrankLesegeraet::LeserNichtInitialisiert;
	}
	//Leser ist bereit:)
	//ist schon eine Karte drinn?
	SCARD_READERSTATE	Leserstatus;
	Leserstatus.szReader = strdup( K_NameDesLesers.toLocal8Bit());
	Leserstatus.dwCurrentState =SCARD_STATE_UNAWARE;
	K_RueckegabePCSC=SCardGetStatusChange(K_PCSC_Kontext,0,&Leserstatus,1);
	if(K_RueckegabePCSC!=SCARD_S_SUCCESS)
	{
#ifndef QT_NO_DEBUG
		qCritical("%s KarteAnfordern: Konnte Zustand des Steckplatzes nicht ermitteln. Grund: %s",qPrintable(objectName()),
					qPrintable(K_RueckegabePCSCNachText(K_RueckegabePCSC)));
#endif
		return QFrankLesegeraet::Error;
	}
	if(Leserstatus.dwEventState&SCARD_STATE_EMPTY)
	{
#ifndef QT_NO_DEBUG
		qDebug("%s KarteAnfordern: Steckplatz ist leer.",qPrintable(objectName()));
#endif
		//keine Karte drin also anfordern
		Leserstatus.dwCurrentState = SCARD_STATE_EMPTY;
		//5 Sekunden auf die Karte warten
		K_RueckegabePCSC=SCardGetStatusChange(K_PCSC_Kontext,5000,&Leserstatus,1);
		if(K_RueckegabePCSC!=SCARD_S_SUCCESS)
		{
#ifndef QT_NO_DEBUG
			qCritical("%s KarteAnfordern: Konnte Karte nicht anfordern. Grund: %s",qPrintable(objectName()),qPrintable(K_RueckegabePCSCNachText(K_RueckegabePCSC)));
#endif
			return QFrankLesegeraet::Error;
		}
		//haben wir jetzte eine Karte?
		if(!((Leserstatus.dwEventState&SCARD_STATE_PRESENT)&&(Leserstatus.dwEventState&SCARD_STATE_CHANGED)))
		{
#ifndef QT_NO_DEBUG
			qCritical("%s KarteAnfordern: Keine Karte im Leser.",qPrintable(objectName()));
#endif
			return QFrankLesegeraet::Error;
		}
	}
	else
	{
		//schon eine Karte drin.
#ifndef QT_NO_DEBUG
		qDebug("%s KarteAnfordern: Steckplatz ist schon besetzt. Status: 0x%X",qPrintable(objectName()),Leserstatus.dwEventState&0x00ffff);
#endif			
	}
	//Karte aktivieren
	K_RueckegabePCSC=SCardConnect(K_PCSC_Kontext,K_NameDesLesers.toLocal8Bit(),SCARD_SHARE_DIRECT,SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1
								  ,&K_Kartenverbindung,&K_Kartenprotokoll);
	if(K_RueckegabePCSC!=SCARD_S_SUCCESS)
	{
#ifndef QT_NO_DEBUG
		qCritical("%s KarteAnfordern: Konnte Karte nicht aktivieren. Grund: %s",qPrintable(objectName()),qPrintable(K_RueckegabePCSCNachText(K_RueckegabePCSC)));
#endif
		return QFrankLesegeraet::Error;
	}
	
#ifndef QT_NO_DEBUG
	qDebug("%s KarteAnfordern: Karte bereit. Hohle ATR.",qPrintable(objectName()));
#endif
	char	KartenleserName[200];
	DWORD   LaengeDesKartenleserName = 200;
	DWORD	Kartenstatus,Kartenprotokoll;
	K_LaengeDerAntwort=sizeof(K_Antwort);
	K_RueckegabePCSC=SCardStatus(K_Kartenverbindung,KartenleserName,&LaengeDesKartenleserName,&Kartenstatus,&Kartenprotokoll,K_Antwort,&K_LaengeDerAntwort);		
	if(K_RueckegabePCSC!=SCARD_S_SUCCESS)
	{
#ifndef QT_NO_DEBUG
		qCritical("%s KarteAnfordern: Konnte den ATR nicht abrufen. Grund: %s",qPrintable(objectName()),qPrintable(K_RueckegabePCSCNachText(K_RueckegabePCSC)));
#endif	
		KarteEntfernen();
		return QFrankLesegeraet::Error;
	}
	//ATR in das QByteArray kopieren
	ATR.resize(K_LaengeDerAntwort);
	memcpy(ATR.data(),K_Antwort,K_LaengeDerAntwort);
#ifndef QT_NO_DEBUG
	qDebug("%s KarteAnfordern: ATR: %s",qPrintable(objectName()),qPrintable(K_FeldNachHex(ATR)));
#endif

	//So dann Versuchen wir mal die Sicherteisklasse zu ermitteln
	K_SicherheitsklasseErmitteln();
	return QFrankLesegeraet::CommandSuccessful;
}

void QFrankPCSC_Leser::K_SicherheitsklasseErmitteln()
{
	return;
	//Sollte eigentlich gehen, aber ist zu riskannt, da es teilweise abstürzt.
	K_RueckegabePCSC=SCardControl(K_Kartenverbindung,CM_IOCTL_GET_FEATURE_REQUEST,NULL,0,K_Antwort,sizeof(K_Antwort),&K_LaengeDerAntwort);
	if(K_RueckegabePCSC!=SCARD_S_SUCCESS)
	{
#ifndef QT_NO_DEBUG
		qCritical("%s K_SicherheitsklasseErmitteln: Konnte die TLV Struktur nicht abrufen. Grund: %s",qPrintable(objectName()),
					qPrintable(K_RueckegabePCSCNachText(K_RueckegabePCSC)));
#endif	
		return;
	}
#ifndef QT_NO_DEBUG
	qDebug("%s K_SicherheitsklasseErmitteln: Struktur decodieren",qPrintable(objectName()));
#endif
}

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
}

QFrankLesegeraet::Leserklasse QFrankPCSC_Leser::Sicherheitsklasse()
{
#ifndef QT_NO_DEBUG
	switch(K_Lesersicherheit)
	{
		case QFrankLesegeraet::Klasse1:
										qDebug("QFrankPCSC_Leser Sicherheitsklasse: Lesser Klasse 1");
										break;
		case QFrankLesegeraet::Klasse2:
										qDebug("QFrankPCSC_Leser Sicherheitsklasse: Lesser Klasse 2");
										break;
		case QFrankLesegeraet::Klasse3:
										qDebug("QFrankPCSC_Leser Sicherheitsklasse: Lesser Klasse 3");
										break;
		case QFrankLesegeraet::Klasse4:
										qDebug("QFrankPCSC_Leser Sicherheitsklasse: Lesser Klasse 4");
										break;
		case QFrankLesegeraet::KlasseUnbekannt:
										qDebug("QFrankPCSC_Leser Sicherheitsklasse: Lesser Klasse unbekannt\r\n"
												"Die Klasse kann erst ermittelt werden, wenn eine Karte aktiviert wurde.");

										break;
		default:
										qFatal("QFrankPCSC_Leser Sicherheitsklasse: Variable Lesersicherheit hat einen unbekanten Wert!!");
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

const QString QFrankPCSC_Leser::K_RueckegabePCSCNachText(const LONG &rueckgabe)const
{
	switch(rueckgabe)
	{
#ifdef Q_WS_WIN
		// Die gibt es nur unter Windows
		case SCARD_E_BAD_SEEK:
											return QString("Fehler beim setzen des Dateiobjektzeigers.");
											break;
		case SCARD_E_COMM_DATA_LOST:
											return trUtf8("Es wurde ein Fehler während der Kommunikation mit der SmartCard festgestellt.","debug");
											break;
		case SCARD_E_DIR_NOT_FOUND:
											return QString("Verzeichnis auf der SmartCard nicht gefunden.");
											break;
		case SCARD_E_FILE_NOT_FOUND:
											return QString("Datei auf der SmarCard nicht vorhanden.");
											break;
		case SCARD_E_ICC_CREATEORDER:
											return trUtf8("Die gewünscht Reihenfolge der Objekterstellung wird nicht unterstützt.","debug");
											break;
		case SCARD_E_ICC_INSTALLATION:
											return QString("Es konnte kein Anbieter für die SmartCard gefunden werden.");
											break;
		case SCARD_E_INVALID_CHV:
											return QString("Der PIN war falsch.");
											break;
		case SCARD_E_NO_ACCESS:
											return QString("Zugriff auf die Datei verboten.");
											break;
		case SCARD_E_NO_DIR:
											return QString("Verzeichnis auf der SmartCard nicht gefunden.");
											break;
		case SCARD_E_NO_FILE:
											return QString("Datei auf der SmartCard nicht gefunden.");
											break;
		case SCARD_E_NO_READERS_AVAILABLE:
											return trUtf8("Kein Leser verfügbar.","debug");
											break;
		case SCARD_E_UNEXPECTED:
											return QString("Unerwarteter SmartCard Fehler aufgetreten.");
											break;
		case SCARD_E_UNKNOWN_RES_MNG:
											return QString("Unbekannter Fehler ist aufgetreten.");
											break;
		case SCARD_E_WRITE_TOO_MANY:
											return QString("Es wurde versucht, mehr Daten an das Objekt zu senden, als reinpassen.");
											break;
		case SCARD_W_CANCELLED_BY_USER:
											return QString("Abbruch durch Nutzer.");
											break;
		case SCARD_W_CHV_BLOCKED:
											return QString("PIN gesperrt.");
											break;
		case SCARD_W_EOF:
											return QString("Dateiende erreicht.");
											break;
		case SCARD_W_SECURITY_VIOLATION:
											return QString("Wegen einer Sicherheitsverletzung wurde der Zugriff verboten.");
											break;
		case SCARD_W_WRONG_CHV:
											return QString("PIN falsch.");
											break;
#endif
		case SCARD_E_CANCELLED:
											return QString("Aktion wurde via SCardCancel abgebrochen.");
											break;
		case SCARD_E_CANT_DISPOSE:
											return trUtf8("Das Medium konnte nicht in der gewünschten weise entfernt werden.","debug");
											break;
		case SCARD_E_CARD_UNSUPPORTED:
											return trUtf8("Die SmarCard wird nicht unterstützt.","debug");
											break;		
		case SCARD_E_DUPLICATE_READER:
											return QString("Leser bereits vorhanden.");
											break;		
		case SCARD_E_INSUFFICIENT_BUFFER:
											return trUtf8("Der Puffer für die Rückgabedaten ist zu klein.","debug");
											break;
		case SCARD_E_INVALID_ATR:
											return trUtf8("Die ATR ist ungültig.","debug");
											break;		
		case SCARD_E_INVALID_HANDLE:
											return trUtf8("Der Objektzeiger ist ungültig.","debug");
											break;
		case SCARD_E_INVALID_PARAMETER:
											return trUtf8("Einer oder mehrerer Parameter sind ungültig.","debug");
											break;
		case SCARD_E_INVALID_TARGET:
											return trUtf8("Ziel ungültig.");
											break;
		case SCARD_E_INVALID_VALUE:
											return trUtf8("Min. ein Wert für min. ein Parameter ist ungültig.","debug");
											break;		
		case SCARD_E_NO_MEMORY:
											return trUtf8("Nicht genug Speicher vorhanden, um den Befehl auszuführen.","debug");
											break;		
		case SCARD_E_NO_SERVICE:
											return trUtf8("PC/SC System läuft nicht.","debug");
											break;
		case SCARD_E_NO_SMARTCARD:
											return QString("Keine SmartCard im Leser.");
											break;
		case SCARD_E_NOT_READY:
											return QString("Leser/SmartCard nicht bereit.");
											break;
		case SCARD_E_NOT_TRANSACTED:
											return QString("Es wurde versucht, eine nicht existierende Transaktion zu beenden.");
											break;
		case SCARD_E_PCI_TOO_SMALL:
											return QString("PCI Empfangspuffer zu klein.");
											break;
		case SCARD_E_PROTO_MISMATCH:
											return QString("Angefordertes Protoll passt nicht zur SmartCard.");
											break;
		case SCARD_E_READER_UNAVAILABLE:
											return trUtf8("Leser zur Zeit nicht verfürbar.","debug");
											break;
		case SCARD_E_READER_UNSUPPORTED:
											return trUtf8("Treiber für den Leser zu alt.","debug");
											break;
		case SCARD_E_SERVICE_STOPPED:
											return QString("PC/SC System angehalten.");
											break;
		case SCARD_E_SHARING_VIOLATION:
											return QString("Auf die SmartCard kann nicht zugegriffen werden, da andere Verbindungen warten.");
											break;
		case SCARD_E_SYSTEM_CANCELLED:
											return QString("Die Aktion wurde vom System abgebrochen.");
											break;
		case SCARD_E_TIMEOUT:
											return QString("Zeit abgelaufen.");
											break;		
		case SCARD_E_UNKNOWN_CARD:
											return QString("Unbekannte SmartCard.");
											break;
		case SCARD_E_UNKNOWN_READER:
											return QString("Unbekannter Leser.");
											break;		
		case SCARD_E_UNSUPPORTED_FEATURE:
											return trUtf8("Die SmartCard unterstützt die Funktion nicht.","debug");
											break;		
		case SCARD_F_COMM_ERROR:
											return QString("Interner Kommunikationsfehler erkannt.");
											break;
		case SCARD_F_INTERNAL_ERROR:
											return trUtf8("Interne Konsistesprüfung fehlgeschlagen.","debug");
											break;
		case SCARD_F_UNKNOWN_ERROR:
											return QString("Interne Fehler aufgetreten, Quelle unbekannt.");
											break;
		case SCARD_F_WAITED_TOO_LONG:
											return QString("Interner Zeitgeber abgelaufen.");
											break;		
		case SCARD_S_SUCCESS:
											return QString("Kein Fehler.");
											break;		
		case SCARD_W_REMOVED_CARD:
											return QString("SmartCard wurde entfernt.");
											break;
		case SCARD_W_RESET_CARD:
											return trUtf8("SmartCard wurde zurückgesetzt.","debug");
											break;		
		case SCARD_W_UNPOWERED_CARD:
											return QString("SmartCard von der Stromversorgung getrennt.");
											break;
		case SCARD_W_UNRESPONSIVE_CARD:
											return QString("SmartCard antwortet nicht auf den Reset.");
											break;
		case SCARD_W_UNSUPPORTED_CARD:
											return QString("Der Leser konnte die SmartCard wegen eines Problems mit dem ATR nicht lesen.");
											break;		
		default:
											return QString("Nicht dekodierter Status: 0x%1").arg(rueckgabe,0,16);
											break;
	};
	return QString();
}

#endif
