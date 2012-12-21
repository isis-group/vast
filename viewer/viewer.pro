! include( ../common.pri ) {
	error( Couldn't find the common.pri file! )
}

TEMPLATE = lib
CONFIG += staticlib
INCLUDEPATH += $$OUT_PWD/../widgets
SOURCES += color.cpp common.cpp fileinformation.cpp geometrical.cpp imageholder.cpp memoryhandler.cpp nativeimageops.cpp \
qprogressfeedback.cpp qviewercore.cpp settings.cpp style.cpp uicore.cpp viewercorebase.cpp \
widgetensemblecomponent.cpp widgetensemble.cpp widgetloader.cpp

HEADERS += color.hpp common.hpp error.hpp fileinformation.hpp geometrical.hpp imageholder.hpp memoryhandler.hpp nativeimageops.hpp \
pluginloader.hpp qprogressfeedback.hpp qviewercore.hpp settings.hpp style.hpp uicore.hpp viewercorebase.hpp \
widgetensemblecomponent.hpp widgetensemble.hpp widgetloader.hpp
