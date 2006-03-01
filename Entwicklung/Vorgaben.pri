CONFIG 	      += qt release
QT 		      -= gui
#Mit Debug Infos? ja/nein
Debuginfos	       = ja
#strip unter unix/mingw? ja/nein
Strip              = nein
isEmpty(PFAD) {
	win32 {
			PFAD=$$system(type $$(TMP)"\x.x")
} else {
		PFAD=$$system(cat /tmp/x.x)
}
}
DESTDIR	       = $$PFAD/bin
MOC_DIR	       = $$PFAD/tmp/moc
OBJECTS_DIR    	 = $$PFAD/tmp/obj
UI_HEADERS_DIR 	 = $$PFAD/tmp/ui_headers
RCC_DIR		 = $$PFAD/tmp/resourcen


contains(Debuginfos, ja) {
	message(Erstelle $$PROJEKTTEIL mit Debugmeldungen)
	DEFINES      += MEINDEBUG
	
	win32{
			contains(TEMPLATE, app) {
								CONFIG += console
}
}
}
contains(Strip, ja) {
#M�ll entfernen
QMAKE_POST_LINK =strip -s $$DESTDIR/$(TARGET)
}
#damit die Datei .dll und nicht 0.dll heist.
win32 {
	contains(TEMPLATE, lib) {
		TARGET_EXT = .dll
}
}
QMAKE_TARGET_COMPANY = Frank B�ttner
QMAKE_TARGET_PRODUCT = QSmartCard
QMAKE_TARGET_DESCRIPTION = C++ Bibliothek f�r den Zugriff auf SmartCards unter Qt.
QMAKE_TARGET_COPYRIGHT = Copyright (c) 2005-2006 Frank B�ttner