# Copyright (C) 2006 Frank Büttner frank-buettner@gmx.net
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

PROJEKTTEIL   = PC/SC Leseregät
TEMPLATE      = lib
CONFIG		 += plugin
include (../../../Vorgaben.pri)
VERSION       = 0.1.0.0
TARGET	      = PCSC-Leser
QMAKE_TARGET_DESCRIPTION = Zugriff auf Lesegeräte per PC/SC
DEFINES	     += DLL_BAUEN
INCLUDEPATH	 += ../Lesegeraet
unix{
	INCLUDEPATH += /usr/include/PCSC
	LIBS  +="-lpcsclite"
}else{
	LIBS +="-lWinscard"
}

HEADERS	      = Quellen/PCSC-Leser.h\
				../Lesegeraet/Lesegeraet.h\
				../Lesegeraet/LesegeraetPlugin.h
SOURCES	      = Quellen/PCSC-Leser.cpp\
				Quellen/PCSC-LeserPlugin.cpp\
				../Lesegeraet/Lesegeraet.cpp\
				../Lesegeraet/LesegeraetPlugin.cpp
