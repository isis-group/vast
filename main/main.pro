! include( ../common.pri ) {
	error( Couldn't find the common.pri file! )
}

TEMPLATE = app
TARGET = vast
INCLUDEPATH += $$OUT_PWD/../widgets
RESOURCES = ../resources/viewer.qrc

SOURCES = main.cpp
LIBS += -L..  -lviewer -lwidgets

