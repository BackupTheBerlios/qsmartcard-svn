/*
 *  Copyright (C) 2005-2007 Frank Büttner frank-buettner@gmx.net
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

#include "Signatur.h"
#include <Windows.h>
#include <Wintrust.h>
#include <Softpub.h>

using namespace QFrank;

bool CT_API_Manipulationsschutz::BibliothekOK(const QString &datei)
{
#ifndef QT_NO_DEBUG
	DWORD Fehlercode;
#endif
	QString Datei=datei;
	Datei.replace("/","\\");
	
	WINTRUST_FILE_INFO Dateiinformationen;
	//alle Felder auf 0 setzen.
	memset(&Dateiinformationen,0,sizeof(Dateiinformationen));
	Dateiinformationen.cbStruct=sizeof(WINTRUST_FILE_INFO);
	Dateiinformationen.pcwszFilePath=(LPCWSTR)Datei.utf16();
	Dateiinformationen.hFile=NULL;
	Dateiinformationen.pgKnownSubject=NULL;

	GUID ArtDerPruefung=WINTRUST_ACTION_GENERIC_VERIFY_V2;
	WINTRUST_DATA ParameterFuerDieUeberpruefung;
	memset(&ParameterFuerDieUeberpruefung,0,sizeof(ParameterFuerDieUeberpruefung));
	ParameterFuerDieUeberpruefung.cbStruct=sizeof(ParameterFuerDieUeberpruefung);

	ParameterFuerDieUeberpruefung.pPolicyCallbackData=NULL;
	ParameterFuerDieUeberpruefung.pSIPClientData=NULL;
	//Kein Dialog anzeigen
	ParameterFuerDieUeberpruefung.dwUIChoice=WTD_UI_NONE;
	//Auf zurückgezogne Zertifikate testen
	ParameterFuerDieUeberpruefung.fdwRevocationChecks=WTD_REVOKE_WHOLECHAIN;
	//ParameterFuerDieUeberpruefung.fdwRevocationChecks=WTD_REVOKE_NONE;
	//Zert ist eingebettet.
	ParameterFuerDieUeberpruefung.dwUnionChoice=WTD_CHOICE_FILE;
	ParameterFuerDieUeberpruefung.dwStateAction=WTD_STATEACTION_IGNORE;
	ParameterFuerDieUeberpruefung.hWVTStateData=NULL;
	ParameterFuerDieUeberpruefung.pwszURLReference=NULL;
	ParameterFuerDieUeberpruefung.pwszURLReference=NULL;
	ParameterFuerDieUeberpruefung.dwUIContext=WTD_UICONTEXT_EXECUTE;
	//Welche Datei??
	ParameterFuerDieUeberpruefung.pFile=&Dateiinformationen;
	LONG Ergebnis=WinVerifyTrust(NULL,&ArtDerPruefung,&ParameterFuerDieUeberpruefung);
	//Ab an die Auswertung
	switch(Ergebnis)
	{
		case ERROR_SUCCESS:
#ifndef QT_NO_DEBUG
							qDebug("Signatur der Datei %s gültig.",qPrintable(Datei));
#endif
							return true;
							break;
		case TRUST_E_NOSIGNATURE:
#ifndef QT_NO_DEBUG
							Fehlercode=GetLastError();
							if(TRUST_E_NOSIGNATURE == Fehlercode || TRUST_E_SUBJECT_FORM_UNKNOWN == Fehlercode || TRUST_E_PROVIDER_UNKNOWN == Fehlercode)
								qDebug("Die Datei %s ist nicht signiert.",qPrintable(Datei));
							else
								qDebug("Die Signatur der Datei %s ist ungültig",qPrintable(Datei));
#endif
							return false;
							break;
		case TRUST_E_EXPLICIT_DISTRUST:
#ifndef QT_NO_DEBUG
							qDebug("Die Signatur von %s ist vorhanden, aber das Zertifikat wurde gesperrt.",qPrintable(Datei));
#endif
							return false;
							break;
		case TRUST_E_SUBJECT_NOT_TRUSTED:
#ifndef QT_NO_DEBUG
							qDebug("In %s ist eine Signatur vorhanden, aber dieser wird nicht vertraut.",qPrintable(Datei));
#endif
							return false;
							break;
		case CRYPT_E_SECURITY_SETTINGS:
#ifndef QT_NO_DEBUG
							qDebug("Dem Herrausgeber des Zertifikates für %s wird nicht vertraut oder der Zeitstemple ist ungültig.",qPrintable(Datei));
#endif
							return false;
							break;
	}
#ifndef QT_NO_DEBUG
	qDebug("Die Datei %s konnte aus unbekannten Gründen nicht verifiziert werden.",qPrintable(Datei));
#endif
	return false;
}
