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

#include <QtCore>
#include <SmartCard.h>

class QFrankKVK: public QFrankSmartCard
{
	Q_OBJECT
	Q_PROPERTY(QString Krankenkassenname READ Krankenkassenname)
	Q_PROPERTY(ulong Krankenkassennummer READ Krankenkassennummer)
	Q_PROPERTY(uint WOP READ WOP)
	Q_PROPERTY(qulonglong Versichertennummer READ Versichertennummer)
	Q_PROPERTY(uint Versichertenstatus READ Versichertenstatus)
	Q_PROPERTY(QChar Statusergaenzung READ Statusergaenzung)
	Q_PROPERTY(QString Titel READ Titel)
	Q_PROPERTY(QString Vorname READ Vorname)
	Q_PROPERTY(QString Namenszusatz READ Namenszusatz)
	Q_PROPERTY(QString Nachname READ Nachname)
	Q_PROPERTY(QDate Geburtsdatum READ Geburtsdatum)
	Q_PROPERTY(QString Strasse READ Strasse)
	Q_PROPERTY(QString Land READ Land)
	Q_PROPERTY(QString PLZ READ PLZ)
	Q_PROPERTY(QString Ort READ Ort)
	Q_PROPERTY(QDate GueltigBis READ GueltigBis)
	Q_PROPERTY(bool KVKLeser READ KVKLeser WRITE KVKLeserSetzen)

	public:
			QFrankKVK(QObject* eltern);
			QString		Krankenkassenname();
			ulong		Krankenkassennummer();
			uint		WOP();
			qulonglong	Versichertennummer();
			uint		Versichertenstatus();
			QChar		Statusergaenzung();
			QString		Titel();
			QString		Vorname();
			QString		Namenszusatz();
			QString		Nachname();
			QDate		Geburtsdatum();
			QString		Strasse();
			QString		Land();
			QString		PLZ();
			QString		Ort();
			QDate		GueltigBis();
			bool		KVKLeser();
			void		KVKLeserSetzen(bool kvk);

	private:
			void		VariabelnInitialisieren();
			QString		KrankenkassennameWert;
			ulong		KrankenkassennummerWert;
			uint		WOPWert;
			qulonglong	VersichertennummerWert;
			uint		VersichertenstatusWert;
			QChar		StatusergaenzungWert;
			QString		TitelWert;
			QString		VornameWert;
			QString		NamenszusatzWert;
			QString		NachnameWert;
			QDate		GeburtsdatumWert;
			QString		StrasseWert;
			QString		LandWert;
			QString		PLZWert;
			QString		OrtWert;
			QDate		GueltigBisWert;
			bool		KVKLeserWert;
};
#endif
