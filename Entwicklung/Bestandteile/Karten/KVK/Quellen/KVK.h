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

#ifndef QFRANKKVK_H
#define QFRANKKVK_H

//XXYYZZ XX=Major YY=Minor ZZ=Patch
#define KVKVersion 0x000100

#include <QtCore>
#include <SmartCard.h>

class QFrankKVK: public QFrankSmartCard
{
	Q_OBJECT
	Q_PROPERTY(QString QFrankKVKKrankenkassenname READ Krankenkassenname)
	Q_PROPERTY(qulonglong QFrankKVKKrankenkassennummer READ Krankenkassennummer)
	Q_PROPERTY(uint QFrankKVKWOP READ WOP)
	Q_PROPERTY(qulonglong QFrankKVKVersichertennummer READ Versichertennummer)
	Q_PROPERTY(uint QFrankKVKVersichertenstatus READ Versichertenstatus)
	Q_PROPERTY(QChar QFrankKVKStatusergaenzung READ Statusergaenzung)
	Q_PROPERTY(QString QFrankKVKTitel READ Titel)
	Q_PROPERTY(QString QFrankKVKVorname READ Vorname)
	Q_PROPERTY(QString QFrankKVKNamenszusatz READ Namenszusatz)
	Q_PROPERTY(QString QFrankKVKNachname READ Nachname)
	Q_PROPERTY(QDate QFrankKVKGeburtsdatum READ Geburtsdatum)
	Q_PROPERTY(QString QFrankKVKStrasse READ Strasse)
	Q_PROPERTY(QString QFrankKVKLand READ Land)
	Q_PROPERTY(QString QFrankKVKPLZ READ PLZ)
	Q_PROPERTY(QString QFrankKVKOrt READ Ort)
	Q_PROPERTY(QDate QFrankKVKGueltigBis READ GueltigBis)
	Q_PROPERTY(bool QFrankKVKKVKLeser READ KVKLeser WRITE KVKLeserSetzen)
	Q_PROPERTY(bool QFrankKVKAuslesen READ KarteAuslesen)

	public:
			QFrankKVK(QObject* eltern);
			QString				Krankenkassenname();
			ulong				Krankenkassennummer();
			uint				WOP();
			qulonglong			Versichertennummer();
			uint				Versichertenstatus();
			QChar				Statusergaenzung();
			QString				Titel();
			QString				Vorname();
			QString				Namenszusatz();
			QString				Nachname();
			QDate				Geburtsdatum();
			QString				Strasse();
			QString				Land();
			QString				PLZ();
			QString				Ort();
			QDate				GueltigBis();
			bool				KarteAuslesen();
			bool				KVKLeser();
			void				KVKLeserSetzen(bool kvk);
			ulong				Version();
			void				welchenLeser(QFrankLesegeraet *diesen);

	private:
			enum				TAG
								{
									TAG_Datenanfang=0x60,
									TAG_Kassenname=0x80,
									TAG_Kassennummer=0x81,
									TAG_Versichertennummer=0x82,
									TAG_Versichertenstatus=0x83,
									TAG_Titel=0x84,
									TAG_Vorname=0x85,
									TAG_Namenszusatz=0x86,
									TAG_Nachname=0x87,
									TAG_Geburtsdatum=0x88,
									TAG_Strasse=0x89,
									TAG_Land=0x8a,
									TAG_PLZ=0x8b,
									TAG_Ort=0x8c,
									TAG_GueltigBis=0x8d,
									TAG_WOP=0x8f,
									TAG_Statusergaenzung=0x90									
								};
			Q_DECLARE_FLAGS(TAGS, TAG)
			QChar				Ersetzen(char was);
			QString				FeldNachQString(QByteArray feld);
			void				VariabelnInitialisieren();
			QString				KrankenkassennameWert;
			ulong				KrankenkassennummerWert;
			uint				WOPWert;
			qulonglong			VersichertennummerWert;
			uint				VersichertenstatusWert;
			QChar				StatusergaenzungWert;
			QString				TitelWert;
			QString				VornameWert;
			QString				NamenszusatzWert;
			QString				NachnameWert;
			QDate				GeburtsdatumWert;
			QString				StrasseWert;
			QString				LandWert;
			QString				PLZWert;
			QString				OrtWert;
			QDate				GueltigBisWert;
			bool				PruefsummeOK(QByteArray daten);
			bool				KVKLeserWert;
			bool				IstEsEineKVK(QByteArray &daten);
			bool 				TagFinden(QFrankKVK::TAGS welches,QByteArray &daten,bool optional=false);
			QFrankLesegeraet	*Leser;
#ifndef QT_NO_DEBUG
			QString				FeldNachHex(QByteArray feld); 
#endif		
};
#endif
