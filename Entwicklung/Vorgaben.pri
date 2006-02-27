CONFIG 	      += qt release
QT 		      -= gui
#Mit Debug Infos??
Debuginfos	       = ja
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
#Müll entfernen
QMAKE_POST_LINK =strip -s $$DESTDIR/$(TARGET)

QMAKE_TARGET_COMPANY = Frank Büttner
QMAKE_TARGET_PRODUCT = QSmartCard
QMAKE_TARGET_DESCRIPTION = C++ Bibliothek für den Zugriff auf SmartCards unter Qt.
QMAKE_TARGET_COPYRIGHT = Copyright (c) 2005-2006 Frank Büttner