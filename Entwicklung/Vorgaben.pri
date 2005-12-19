CONFIG 	      += qt release
#Mit Debug Infos??
Debuginfos	       = ja
#Müll entfernen
#QMAKE_POST_LINK =strip -s $(TARGET)
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



contains(Debuginfos, ja) {
	message(Erstelle $$PROJEKTTEIL mit Debugmeldungen)
	DEFINES      += MEINDEBUG
	
	win32{
			contains(TEMPLATE, app) {
								CONFIG += console
}
}
}