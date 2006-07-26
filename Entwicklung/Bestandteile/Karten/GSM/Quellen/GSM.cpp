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
#include <EFAntwort.h>
#include <MF_DFAntwort.h>
#include <Lesegeraet.h>

//Zum übersetzten wird min. Version 0.2.0 des Lesermodells benötigt.
#if LesegeraetAPI_Version < 0x000200
#error Es wird min. Version 0.2.0 des Lesermodells benötigt.
#endif

QFrankGSMKarte::QFrankGSMKarte(QObject* eltern):QFrankSmartCard(eltern)
{
	setObjectName("QFrankGSMKarte");
	K_PIN1gesetzt=false;
	K_PIN2gesetzt=false;
	K_KarteAkiviert=false;
	K_Leser=0;
	K_Seriennummer="";
	K_MF_DFAntwort=new QFrankGSMKarteMF_DFAntwort(this);
	K_EFAntwort=new QFrankGSMKarteEFAntwort(this);
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
		qDebug()<<"QFrankGSMKarte Karte aktivieren: keine/falsche Karte im Leser";
		qDebug()<<K_FeldNachHex(ATR);
#endif
		K_Fehlertext=trUtf8("Keine oder ungültige Karte eingelegt");
		return false;	
	}
	K_KarteAkiviert=true;
	/* haben wir eine SIM Karte???
	   Jede Karte hat eine Seriennummer(10 Bytes)	   
	*/
	if (!K_SeriennummerErmitteln())
	{
		K_KarteAkiviert=false;
		return false;
	}
	else
		return true;
}

bool QFrankGSMKarte::K_SeriennummerErmitteln()
{
	/*  EF für die Seriennummer der Karte ist: 2fe2 unter dem MF
		Befehl: Select file 2fe2
		Kode: 0xA0-A4-00-00-02-2f-e2
	*/
	K_Kartenbefehl.resize(7);
	K_Kartenbefehl[0]=0xA0;
	K_Kartenbefehl[1]=0xA4;
	K_Kartenbefehl[2]=0x00;
	K_Kartenbefehl[3]=0x00;
	K_Kartenbefehl[4]=0x02;
	K_Kartenbefehl[5]=0x2F;
	K_Kartenbefehl[6]=0xE2;
	K_Antwortkode=K_Leser->UniversalIO(K_Kartenbefehl,K_Kartenantwort);
	/*  Wenn es geklappt hat dann bekommen wird eine 0x9FXX
		wobei XX die Länge der abzuhohlen Rückeantwort des Befehls ist.
	*/
	if (K_Antwortkode<0x9f00 || K_Antwortkode>0x9fff)
	{
		K_Fehlertext=tr("Es wurde vermutlich keine GSM Karte eingelegt.");
#ifndef QT_NO_DEBUG
		qDebug()<<QString("QFrankGSMKarte Seriennummer ermitteln: vermutlich keine GSM Karte Rückgabe Code: %1").arg(K_Antwortkode,0,16);
#endif
		K_Leser->KarteEntfernen();
		return false;
	}
	K_Antwortkode=K_Antwortkode&0x00ff;
	K_AntwortLaenge=(uchar)K_Antwortkode;
	K_KartenantwortHohlen(QFrankGSMKarte::EF);
	return false;
}

bool QFrankGSMKarte::K_VerbindungZurKarte()
{
	//Prüft ob eine Verbindung zum Leser besteht.
	if(K_Leser==0)
	{
		K_Fehlertext=tr("Keine Verbindung zum Kartenleser");
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte: keine Verbindung zum Leser";
#endif
		return false;
	}
	else if(!K_KarteAkiviert)
	{
		K_Fehlertext=tr("Keine Verbindung zur Karte");
#ifndef QT_NO_DEBUG
		qDebug()<<"QFrankGSMKarte: keine Verbindung zur Karte";
#endif
		return false;
	}
	return true;
}

void QFrankGSMKarte::K_KartenantwortHohlen(QFrankGSMKarte::Antwort antwort)
{
	if(!K_VerbindungZurKarte())
		return;
	/*  K_AntwortLaenge beinhaltet die Länge der Antwort die die Karte bereit hält
		Befehl: Get Response Antwortlänge
		Kode: 0xA0-C0-00-00-Antwortlänge
	*/
	K_Kartenbefehl.resize(5);
	K_Kartenbefehl[0]=0xA0;
	K_Kartenbefehl[1]=0xC0;
	K_Kartenbefehl[2]=0x00;
	K_Kartenbefehl[3]=0x00;
	K_Kartenbefehl[4]=K_AntwortLaenge;
	K_Antwortkode=K_Leser->UniversalIO(K_Kartenbefehl,K_Kartenantwort);
	//9000 alles OK
	if (K_Antwortkode!=QFrankLesegeraet::CommandSuccessful)
	{
		K_Fehlertext=tr("Es konnte die Antwort eines Befehls nicht empfangen werden.");
#ifndef QT_NO_DEBUG
		qDebug()<<QString("QFrankGSMKarte Kartenantwort hohlen: Fehler Rückgabe Code: %1").arg(K_Antwortkode,0,16);
		qDebug()<<K_FeldNachHex(K_Kartenantwort);
#endif
		K_Leser->KarteEntfernen();
		K_KarteAkiviert=false;
		return;
	}
	//Auswertung
	uint Puffer;
	switch (antwort)
	{
		case QFrankGSMKarte::MF_DF:
#ifndef QT_NO_DEBUG
						qDebug()<<"QFrankGSMKarte Kartenantwort hohlen: werte MF/DF aus";
#endif
						/*  Struktur:
							Byte 1-2 RFU
							Byte 3-4 freier Speicher für EF/DF unter dem DF/MF
							Byte 5-6 Datei ID
							Byte 7 Dateitype 0x00=RFU,0x01=MF,0x02=DF,0x04=EF
							Byte 8-12 RFU
							Byte 13 Länge des folgenden Datenfeldes
							Byte 14-34 GSM spezifisch
							GSM Struktur:
							Byte 14 Datei Beschreibung. interissant nur Bit 8
								ist es 0=PIN1 aktiviert 1=PIN1 deaktiviert
							Byte 15 Anzahl der Unterverzeichnisse unter diesem
							Byte 16 Anzahl der Dateien unter diesem
							Byte 17 Anzahl der PIN's, super PIN's und Admin Codes
							Byte 18 RFU
							Byte 19 PIN1 Status Bit 8=0 PIN nicht gesetzt
								8=1 PIN gesetzt
								Bit 7-5 RFU
								Bit 4-3 Anzahl der verbleibenen Versuche der Pineingabe 0=gesperrt
							Byte 20 Super PIN1 Status siehe Byte 19
							Byte 21 PIN2 Status siehe Byte 19
							Byte 22 Super PIN2 Status siehe Byte 19
							Byte 23 RFU
							Byte 24-34 Reserviert für admin(wenn vorhanden)
							Byte 35> RFU
						*/
						break;
		case QFrankGSMKarte::EF:
#ifndef QT_NO_DEBUG
						qDebug()<<"QFrankGSMKarte Kartenantwort hohlen: werte EF aus";
#endif
						/*  Struktur:
							Byte 1-2 RFU
							Byte 3-4 Dateigröße bei transparenten Bodygröße
								sonst Anzahl der Einträge mal Länge eines einzelnen Datensatzes
							Byte 5-6 Datei ID
							Byte 7 Dateitype siehe Byte 7 MF/DF
							Byte 8 RFU für transparente und Dateien fixer Länge.
								Für zyklische Dateien alle Bits ausser Bit 7 RFU
								Bit 7= INCREASE Befehl zulässig
							Byte 9-11 Zugriffsmöglichkeiten:
								Wertigkeit der Einträge: 0=immer,1=PIN1(wenn gesetzt),2=PIN2(wenn gesetzt)
								3=RFU, 4-14=admin,15=nie
							Byte 9 Bit 8-5 Kommando Read,Seek
								Bit 4-1 Kommando Update
							Byte 10 Bit 8-5 Kommando INCREASE
								Bit 4-1 RFU
							Byte 11 Bit 8-5 Kommando Rehabilitate
								 Bit 4-1 Kommando Invalidate
							Byte 12 Dateistatus Bit 8-2 RFU 1=0 üngültig 1=1 gültig
							Byte 13 Länge der folgenden Daten
							Byte 14 Struktur der Datei 00=transparent,01=feste Länge,03=zyklisch
							Byte 15 Länge eines Eintrages bei transparenten Dateien=00 sonst länge eines Datensatzes.
							Byte 16> RFU
						*/
						Puffer=((uint)K_Kartenantwort.at(2)) <<8 |((uchar)K_Kartenantwort.at(3));
						qDebug()<<QString("text %1").arg(Puffer,0,16);
						K_EFAntwort->DateigroesseSetzen(Puffer);	
						Puffer=((uint)K_Kartenantwort.at(4)) <<8 |((uchar)K_Kartenantwort.at(5));
						K_EFAntwort->DateiIDSetzen(Puffer);
						XX Hier gehts weiter
#ifndef QT_NO_DEBUG
						qDebug()<<"QFrankGSMKarte Kartenantwort EF:";
						qDebug()<<QString("\tDateigröße: 0x%1").arg(K_EFAntwort->Dateigroesse(),0,16);
						qDebug()<<QString("\tDatei ID: 0x%1").arg(K_EFAntwort->DateiID(),0,16);
#endif
						break;
	}
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

QString	QFrankGSMKarte::K_FeldNachHex(const QByteArray &feld) const
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

