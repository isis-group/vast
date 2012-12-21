! include( ../common.pri ) {
	error( Couldn't find the common.pri file! )
}

TEMPLATE = app
TARGET = vast
INCLUDEPATH += $$OUT_PWD/../widgets
RESOURCES = ../resources/viewer.qrc


SOURCES = main.cpp
LIBS += -L..  -lviewer -lwidgets

PRE_TARGETDEPS = ../$$qtLibraryTarget(viewer) ../$$qtLibraryTarget(widgets)
DEFINES += _VAST_WIDGET_PATH=\\\"$$OUT_PWD/../view_widgets\\\"
DEFINES += _VAST_PLUGIN_PATH=\\\"$$OUT_PWD/../view_widgets\\\"

