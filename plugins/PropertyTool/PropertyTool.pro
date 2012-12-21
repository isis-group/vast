! include( ../common.pri ) {
	error( Couldn't find the common.pri file! )
}


TARGET      = $$qtLibraryTarget(vastPlugin_PropertyTool)

SOURCES += PropertyToolDialog.cpp
HEADERS += PropertyToolDialog.hpp
RESOURCES = resources/propertytool.qrc
FORMS = forms/propertyToolDialog.ui
