# Copyright (C) 2005-2006 Frank B�ttner frank-buettner@gmx.net
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

PROJEKTTEIL   = Krankenversicherungskarte
TEMPLATE      = lib
CONFIG       += plugin
include (../../../Vorgaben.pri)
VERSION       = 0.1.0.0
TARGET	      = KVK
QMAKE_TARGET_DESCRIPTION = Modul f�r die Krankenversicherungskarte
DEFINES	     += DLL_BAUEN
INCLUDEPATH	 += ../SmartCard\
				../../Lesegeraete/Lesegeraet
HEADERS		  = Quellen/KVK.h\
				../SmartCard/SmartCard.h\
				../SmartCard/SmartCardPlugin.h				
SOURCES		  = Quellen/KVK.cpp\
				Quellen/KVKPlugin.cpp\
				../SmartCard/SmartCard.cpp\
				../SmartCard/SmartCardPlugin.cpp
				
				
