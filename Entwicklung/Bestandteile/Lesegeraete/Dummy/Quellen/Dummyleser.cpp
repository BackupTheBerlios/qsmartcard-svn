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
	Datenfeld=QByteArray(12,0x55);
	RueckgabecodeReadBinary=QFrankLesegeraet::CommandSuccessful;
	RueckgabecodeUpdateBinary=QFrankLesegeraet::CommandSuccessful;
	RueckgabecodeVerify=QFrankLesegeraet::CommandSuccessful;
	RueckgabecodeChangeReferenceData=QFrankLesegeraet::CommandSuccessful;
	RueckgabecodeKarteAnfordern=QFrankLesegeraet::CommandSuccessful;
	SicherheitsklasseWert=QFrankLesegeraet::KlasseUnbekannt;
	PinSicher[0]=0x12;
	PinSicher[1]=0x34;
	PinSicher[2]=0x5f;
	PinSicherNeu[0]=0x67;
	PinSicherNeu[1]=0x89;
	PinSicherNeu[2]=0x0f;
}

ulong QFrankDummyleser::Version()
{
	return DummyleserVersion;
}

QString QFrankDummyleser::FeldNachHex(QByteArray feld)
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

QString QFrankDummyleser::BCDNachDez(QByteArray feld)
{
	uchar low,high;
	QString tmp="";
	for(int x=0;x<feld.size();x++)
	{
		high=(feld.at(x)>>4) & 0x0f;
		low=feld.at(x) &0x0f;
		if(high>0x09)
			break;
		else
			tmp.append(high+0x30);
		if(low>0x09)
			break;
		else
			tmp.append(low+0x30);
	}
	return tmp;
}

bool QFrankDummyleser::ISO_SelectFileFehler()
{
	return FehlerSF;
}

void QFrankDummyleser::ISO_SelectFileFehlerSetzen(bool fehler)
{
	FehlerSF=fehler;
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
		qCritical()<<"Ungültiger Wert für P1 Feld Wert:"<<(uint)datenfeld.at(0)<<"\r\nSelect File Ende";
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
		//Hight Byte/Low Byte
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
			qDebug()<<QString("FID ungültig erkannt wurde:%1 \r\n%2").arg(FID,0,16).arg("Select File Ende");
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
		qDebug()<<"Daten ungültig Dump:"<<FeldNachHex(datenfeld)<<"\r\nSelect File Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	if(FehlerSF)
	{
		qDebug()<<"Select File Fehler wird simmuliert\r\nSelect File Ende";
		return QFrankLesegeraet::FileNotFound;
	}
	qDebug()<<"Select File Ende";
	return QFrankLesegeraet::CommandSuccessful;
}

ulong QFrankDummyleser::ISO_ReadBinaryStatuscode()
{
	return RueckgabecodeReadBinary;
}

void QFrankDummyleser::ISO_ReadBinaryStatuscodeSetzen(ulong status)
{
	RueckgabecodeReadBinary=(QFrankLesegeraet::Rueckgabecodes)status;
}

QByteArray QFrankDummyleser::ISO_ReadBinaryDaten()
{
	return Datenfeld;
}

void QFrankDummyleser::ISO_ReadBinaryDatenSetzen(QByteArray dummydaten)
{
	Datenfeld=dummydaten;
}

QFrankLesegeraet::Rueckgabecodes QFrankDummyleser::ISO_ReadBinary(QByteArray datenfeld,QByteArray &Zielfeld)
{
	qDebug()<<"Read Binary Anfang";
	//P1+P2 Adresse
	//LC=Länge der zu lesenden Daten 00=alles
	if(datenfeld.size()<3)
	{
		//steht nur Müll drin
		qCritical()<<"Das Datenfeld ist zu klein Größe:"<<datenfeld.size()<<"\r\nRead Binary Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	Zielfeld=Datenfeld;
	qDebug()<<"Es soll ab Adresse"<<FeldNachHex(datenfeld.mid(0,2))<<FeldNachHex(datenfeld.mid(2))<<"Bytes gelesen werden";
	qDebug()<<"Das wurde gelesen:"<<FeldNachHex(Datenfeld);
	// Zu Lesenden Bytes größer als die Dummydaten? Dann EOF vor Le
	qulonglong ZuLesendeDaten=0;
	for (int x=2;x<datenfeld.size();x++)
	{
		ZuLesendeDaten=ZuLesendeDaten <<8 | datenfeld.at(x);
	}
	if(Datenfeld.size()<ZuLesendeDaten)
	{
		qDebug()<<"Warnung EOF vor Le\r\nRead Binary Ende";
		return QFrankLesegeraet::WarningEOFbeforeLeBytes;
	}
	if(RueckgabecodeReadBinary!=QFrankLesegeraet::CommandSuccessful)
	{
		qDebug()<<"Read Binary Fehler wird simmuliert\r\nRead Binary Ende";
		return RueckgabecodeReadBinary;
	}
	qDebug()<<"Read Binary Ende";
	return QFrankLesegeraet::CommandSuccessful;
}

ulong QFrankDummyleser::ISO_UpdateBinaryStatuscode()
{
	return RueckgabecodeUpdateBinary;
}

void QFrankDummyleser::ISO_UpdateBinaryStatuscodeSetzen(ulong status)
{
	RueckgabecodeUpdateBinary=(QFrankLesegeraet::Rueckgabecodes)status;
}

QFrankLesegeraet::Rueckgabecodes QFrankDummyleser::ISO_UpdateBinary(QByteArray datenfeld)
{
	qDebug()<<"Update Binary Anfang";
	/*Datenfeld muss min 4 Byte gross sein
	  1-2 Offset
	  3 Länge der Daten
	  ab 4 Daten
	*/
	if(datenfeld.size()<4)
	{
		qCritical()<<"Das Datenfeld ist zu klein Größe:"<<datenfeld.size()<<"\r\nUpdate Binary Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	if((datenfeld.size()-3)>256)
	{
		qCritical()<<"Das Datenfeld ist zu groß Größe:"<<datenfeld.size()<<"\r\nUpdate Binary Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	if(((uint)datenfeld.at(2))!=(datenfeld.size()-3))
	{
		qCritical()<<"Die angegeben Länge der Daten stimmt nicht mit der Länge des Feldes überein."<<
					"Länge soll:"<<(uint)datenfeld.at(2)<<"ist:"<<(datenfeld.size()-3)<<"\r\nUpdate Binary Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	qDebug()<<"Ab Adresse"<<FeldNachHex(datenfeld.mid(0,2))<<"werden"<<datenfeld.size()-3<<"Bytes geschrieben";
	qDebug()<<"Inhalt:"<<FeldNachHex(datenfeld.mid(3));
	if(RueckgabecodeUpdateBinary!=QFrankLesegeraet::CommandSuccessful)
	{
		qDebug()<<"Update Binary Fehler wird simmuliert\r\nUpdate Binary Ende";
		return RueckgabecodeUpdateBinary;
	}
	qDebug()<<"Update Binary Ende";
	return QFrankLesegeraet::CommandSuccessful;
}

ulong QFrankDummyleser::ISO_VerifyStatuscode()
{
	return RueckgabecodeVerify;
}

void QFrankDummyleser::ISO_VerifyStatuscodeSetzen(ulong status)
{
	RueckgabecodeVerify=(QFrankLesegeraet::Rueckgabecodes)status;
}

QFrankLesegeraet::Rueckgabecodes QFrankDummyleser::ISO_Verify(QByteArray datenfeld)
{
	/* Das Datenfeld muss min 4 Bytes lang sein
		1+2 00-00
		3. Anzahl der Stellen
		4-..n XX jedes der 2 Felder enthält 2 Zahlen des Pin's die Restlichen sind F
	*/
	qDebug()<<"Verify Anfang";
	if(datenfeld.size()<4)
	{
		qCritical()<<"Das Datenfeld ist nicht min. 4 Bytes sondern"<<datenfeld.size()<<"Bytes lang.\r\nVerify Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	if(((uint)datenfeld.at(2))!=(datenfeld.size()-3) || (datenfeld.at(0)!=0x00 && datenfeld.at(1)!=0x00))
	{
		qCritical()<<"Die Struktur des Datenfeldes ist falsch."<<FeldNachHex(datenfeld)<<"\r\nVerify Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	qDebug()<<"Der Pin lautet:"<<BCDNachDez(datenfeld.mid(3))<<".";
	if(RueckgabecodeVerify!=QFrankLesegeraet::CommandSuccessful)
	{
		qDebug()<<"Verify Fehler wird simmuliert\r\nVerify Ende";
		return RueckgabecodeVerify;
	}
	qDebug()<<"Verify Ende";
	return QFrankLesegeraet::CommandSuccessful;
}

ulong QFrankDummyleser::ISO_ChangeReferenceDataStatuscode()
{
	return RueckgabecodeChangeReferenceData;
}

void QFrankDummyleser::ISO_ChangeReferenceDataStatuscodeSetzen(ulong status)
{
	RueckgabecodeChangeReferenceData=(QFrankLesegeraet::Rueckgabecodes)status;
}

QFrankLesegeraet::Rueckgabecodes QFrankDummyleser::ISO_ChangeReferenceData(QByteArray datenfeld)
{
	qDebug()<<"Change Reference Data Anfang";
	//Datenstruktur wie bei Verify jedoch 2. Pinblock für den neuen Pin.
	if(datenfeld.size()<5)
	{
		qCritical()<<"Das Datenfeld ist nicht min. 5 Bytes sondern"<<datenfeld.size()<<"Bytes lang.\r\nChange Reference Data Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	if(((uint)datenfeld.at(2))!=(datenfeld.size()-3) || (datenfeld.at(0)!=0x00 && datenfeld.at(1)!=0x00))
	{
		qCritical()<<"Die Struktur des Datenfeldes ist falsch."<<FeldNachHex(datenfeld)<<"\r\nChange Reference Data Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	qDebug()<<"Der Pin :"<<BCDNachDez(datenfeld.mid(3,((uint)datenfeld.at(2))/2 ))<<"wird in"<<\
			BCDNachDez(datenfeld.mid(3+(((uint)datenfeld.at(2))/2) ))<<	"geändert";
	if(RueckgabecodeChangeReferenceData!=QFrankLesegeraet::CommandSuccessful)
	{
		qDebug()<<"Change Reference Data Fehler wird simmuliert\r\nChange Reference Data Ende";
		return RueckgabecodeChangeReferenceData;
	}
	qDebug()<<"Change Reference Data Ende";
	return QFrankLesegeraet::CommandSuccessful;
}

QByteArray QFrankDummyleser::ISO_VerifySecurePin()
{
	return PinSicher;
}

void QFrankDummyleser::ISO_VerifySecurePinSetzen(QByteArray pinsicher)
{
	PinSicher=pinsicher;
}

QFrankLesegeraet::Rueckgabecodes QFrankDummyleser::ISO_VerifySecure(QByteArray datenfeld)
{
	//Das geht nur bei Lesern größer Klasse 1
	qDebug()<<"Verify Secure Anfang";
	if(SicherheitsklasseWert==QFrankLesegeraet::Klasse1 ||  SicherheitsklasseWert==QFrankLesegeraet::KlasseUnbekannt)
	{
		qDebug()<<"Sichere Pineingabe wird nur von Lesern Klasse 2 oder höher unterstützt\r\nVerify Secure Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	/* Datenfeld
		Byte 1 01-0x0e Terminal/Kartenslot
		Byte 2 00=Pinpad 01= biometrische Eingabe
		Byte 3 Länge des Rests
		Byte 4 ..Byte n Datagram
		Byte 5 muss 0x52 sein.
		Byte 6 Länge des Tags 0x52
		Byte 7 Controlbyte
		Byte 8 Einfügeposition des Pin's
		Byte 9 - bis Pos -2 aus Byte 8 Befehlscode
	*/
	if((datenfeld.at(0)>0x0e || datenfeld.at(0)<0x01) || (datenfeld.at(1)!=0x00 && datenfeld.at(1)!=0x01) 
	   || (((uint)datenfeld.at(2))!=(datenfeld.size()-3)) || datenfeld.at(3)!=0x52 ||
	   (((uint)datenfeld.at(4))!=(datenfeld.size()-5)) || (((uint)datenfeld.at(6))!=(datenfeld.size()-5)))
	{
		qCritical()<<"Die Struktur des Datenfeldes ist falsch."<<FeldNachHex(datenfeld)<<"\r\nVerify Secure Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}	
	qDebug()<<"Es sollte der Befehl ausgeführt werden:"<<FeldNachHex(datenfeld.mid(7));
	//Pin oder Biometrie
	if(datenfeld.at(1)==0x00)
	{
		qDebug()<<"Als Pin wurde vom Leser angefügt(Länge-Pin lehre Stellen wurden weggelassen sonst F):"<<QString("%1").arg(PinSicher.size(),0,16)<<
			   "-"<<BCDNachDez(PinSicher);
	}
	if(RueckgabecodeVerify!=QFrankLesegeraet::CommandSuccessful)
	{
		qDebug()<<"Verify Secure Fehler wird simmuliert\r\nVerify Secure Ende";
		return  RueckgabecodeVerify;
	}
	qDebug()<<"Verify Secure Ende";
	return QFrankLesegeraet::CommandSuccessful;
}


QByteArray QFrankDummyleser::ISO_ChangeReferenceDataSecurePinNeu()
{
	return PinSicherNeu;
}

void QFrankDummyleser::ISO_ChangeReferenceDataSecurePinNeuSetzen(QByteArray pinneu)
{
	PinSicherNeu=pinneu;
}

QFrankLesegeraet::Rueckgabecodes QFrankDummyleser::ISO_ChangeReferenceDataSecure(QByteArray datenfeld)
{
	//Datenstruktur wie ähnlich ISO_VerifySecure nur das es 2 Positionsbytes gibt
	qDebug()<<"Change Reference Data Secure Anfang";
	//Das geht nur bei Lesern größer Klasse 2
	if(SicherheitsklasseWert==QFrankLesegeraet::Klasse1 ||  SicherheitsklasseWert==QFrankLesegeraet::KlasseUnbekannt ||
	   SicherheitsklasseWert==QFrankLesegeraet::Klasse2)
	{
		qDebug()<<"Sichere Pinänderung wird nur von Lesern Klasse 3 oder höher unterstützt\r\nChange Reference Data Secure Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	uint Pinalt=(uint)((datenfeld.at(5) & 0xf0)>>4);
	if((datenfeld.at(0)>0x0e || datenfeld.at(0)<0x01) || (datenfeld.at(1)!=0x00 && datenfeld.at(1)!=0x01) 
	   || (((uint)datenfeld.at(2))!=(datenfeld.size()-3)) || datenfeld.at(3)!=0x52 ||
	   (((uint)datenfeld.at(4))!=(datenfeld.size()-5)) || (((uint)datenfeld.at(6))!=(datenfeld.size()-5))
	   || (((uint)datenfeld.at(7))!=(((uint)datenfeld.at(6))+Pinalt-1)))
	{
		qCritical()<<"Die Struktur des Datenfeldes ist falsch."<<FeldNachHex(datenfeld)<<"\r\nChange Reference Data Secure Ende";
		return QFrankLesegeraet::ParameterFalsch;
	}
	qDebug()<<"Es sollte der Befehel ausgeführt werden:"<<FeldNachHex(datenfeld.mid(8));
	//Pin oder Biometrie
	if(datenfeld.at(1)==0x00)
	{
		qDebug()<<"Als alter Pin wurde vom Leser angefügt(Länge-Pin lehre Stellen wurden weggelassen sonst F):"<<QString("%1").arg(PinSicher.size(),0,16)<<
			   "-"<<BCDNachDez(PinSicher);
		qDebug()<<"Als neuer Pin wurde vom Leser angefügt(Länge-Pin lehre Stellen wurden weggelassen sonst F):"<<QString("%1").arg(PinSicherNeu.size(),0,16)<<
				"-"<<BCDNachDez(PinSicherNeu);
	}
	if(RueckgabecodeChangeReferenceData!=QFrankLesegeraet::CommandSuccessful)
	{
		qDebug()<<"VChange Reference Data Secure Fehler wird simmuliert\r\nChange Reference Data Secure Ende";
		return  RueckgabecodeVerify;
	}
	qDebug()<<"Change Reference Data Secure Ende";
	return QFrankLesegeraet::CommandSuccessful;
}

ulong QFrankDummyleser::KarteAnfordernStatuscode()
{
	return RueckgabecodeKarteAnfordern;
}

void QFrankDummyleser::KarteAnfordernStatuscodeSetzen(ulong status)
{
	RueckgabecodeKarteAnfordern=(QFrankLesegeraet::Rueckgabecodes)status;
}

QFrankLesegeraet::Rueckgabecodes QFrankDummyleser::KarteAnfordern(QByteArray &ATR)
{
	qDebug()<<"Karte Anfordern Anfang";
	ATR=Datenfeld;
	qDebug()<<"Das Liefert die Karte als ATR zurück:"<<FeldNachHex(Datenfeld);
	if(RueckgabecodeKarteAnfordern!=QFrankLesegeraet::CommandSuccessful)
	{
		qDebug()<<"Karte Anfordern Fehler wird simmuliert\r\nKarte Anfordern Ende";
		return RueckgabecodeKarteAnfordern;
	}	
	qDebug()<<"Karte Anfordern Ende";
	return QFrankLesegeraet::CommandSuccessful;
}

QFrankLesegeraet::Leserklasse QFrankDummyleser::Sicherheitsklasse()
{
	qDebug()<<"Sicherheitsklasse Annfang";
	switch(SicherheitsklasseWert)
	{
		case QFrankLesegeraet::Klasse1:
										qDebug()<<"Ich bin ein Leser der Klasse 1, d.h. ohne alles.";
										break;
		case QFrankLesegeraet::Klasse2:
										qDebug()<<"Ich bin ein Leser der Klasse 2, d.h. mit eigener Tastatur.";
										break;
		case QFrankLesegeraet::Klasse3:
										qDebug()<<"Ich bin ein Leser der Klasse 3, d.h. mit Tastaur und Display.";
										break;
		case QFrankLesegeraet::Klasse4:
										qDebug()<<"Ich bin ein Leser der Klasse 4, d.h. mit Tastatur,Display und Sicherheitsmodul.";
										break;
		case QFrankLesegeraet::KlasseUnbekannt:
										qDebug()<<"Meine Sicherheitsklasse konnte nicht ermittelt werden.:(";
										break;
		default:
										qDebug()<<"Ein unbekannter Wert für die Sicherheitsklasse wurde angegeben.";
										qDebug()<<"Das darf und sollte NIE passieren!!!!";
										break;
	}
	qDebug()<<"Sicherheitsklasse Ende";
	return SicherheitsklasseWert;
}

void QFrankDummyleser::SicherheitsklasseSetzen(ulong klasse)
{
	SicherheitsklasseWert=(QFrankLesegeraet::Leserklasse)klasse;
}
