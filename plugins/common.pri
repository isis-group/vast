! include( ../common.pri ) {
	error( Couldn't find the common.pri file! )
}

TEMPLATE    = lib
CONFIG      += plugin

INCLUDEPATH += $$OUT_PWD/../../widgets
INCLUDEPATH += ../..
DESTDIR     = ../../plugins
LIBS += -L../..  -lviewer
PRE_TARGETDEPS = ../../$$qtLibraryTarget(viewer)
