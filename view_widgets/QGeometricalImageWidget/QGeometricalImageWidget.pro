! include( ../common.pri ) {
	error( Couldn't find the common.pri file! )
}


TARGET      = $$qtLibraryTarget(vastVisWidgetGeometrical)

SOURCES += geomhandler.cpp QGeomWidget.cpp
HEADERS += geomhandler.hpp QGeomWidget.hpp

