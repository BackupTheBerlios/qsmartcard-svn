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

#ifndef QFRANKGSMKARTE_H
#define QFRANKGSMKARTE_H

//XXYYZZ XX=Major YY=Minor ZZ=Patch
#define GSMKarteVersion 0x000100

#include <QtCore>
#include <SmartCard.h>

class QFrankGSMKarteMF_DFAntwort;
class QFrankGSMKarteEFAntwort;

class QFrankGSMKarte: public QFrankSmartCard
{
	Q_OBJECT
	Q_PROPERTY(bool QFrankGSMKarteAktivieren READ KarteAktivieren)
	Q_PROPERTY(QString QFrankGSMKarteFehlertext READ Fehlertext)
	Q_PROPERTY(QString QFrankGSMKarteSeriennummer READ Seriennummer)
	Q_PROPERTY(QString QFrankGSMKarteAnbieter READ Anbieter)

	public:
			QFrankGSMKarte(QObject* eltern);
			ulong						Version();
			void						welchenLeser(QFrankLesegeraet *diesen);
			bool						KarteAktivieren();
			const QString				Fehlertext() const;
			const QString				Seriennummer() const;
			const QString				Anbieter();

	private:
			enum						ArtDerAntwort{MF_DF=0x00,EF=0x01};
			Q_DECLARE_FLAGS(Antwort,ArtDerAntwort)
			QFrankLesegeraet*			K_Leser;
			QFrankGSMKarteMF_DFAntwort*	K_MF_DFAntwort;
			QFrankGSMKarteEFAntwort*	K_EFAntwort;
			QString						K_Fehlertext;
			QString						K_Seriennummer;
			QByteArray					K_Kartenbefehl;
			QByteArray					K_Kartenantwort;
			uint						K_Antwortkode;
			bool						K_ReadBinary(const uchar &anzahl,const uint &offset=0);
			bool						K_PIN1gesetzt;
			bool						K_PIN2gesetzt;
			bool						K_KarteAkiviert;
			bool						K_VerbindungZurKarte();
			bool						K_SeriennummerErmitteln();
			bool						K_SelectFile(const uint &was);
			void						K_GetResponse(QFrankGSMKarte::Antwort antwort,const uchar &antwortLaenge);
			void						K_EFAntwortBearbeiten();
			void						K_MF_DFAntwortBearbeiten();
#ifndef QT_NO_DEBUG
			QString						K_FeldNachHex(const QByteArray &feld) const;
			QString						K_Zugrifftext(const uchar &kodierung)const;
#endif
};
#endif
