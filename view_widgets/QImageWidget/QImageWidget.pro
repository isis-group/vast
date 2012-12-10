! include( ../common.pri ) {
	error( Couldn't find the common.pri file! )
}


TARGET      = $$qtLibraryTarget(vastVisWidgetImage)

SOURCES += QImageWidgetImplementation.cpp QOrientationHandler.cpp
HEADERS += QImageWidgetImplementation.hpp QOrientationHandler.hpp
