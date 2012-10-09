QT       += core gui

TARGET = vast
TEMPLATE = app

debug {
	DEFINES += _ENABLE_DEBUG=1 _ENABLE_DEV=1
} else {
	DEFINES += _ENABLE_DEBUG=0 _ENABLE_DEV=0
}

DEFINES += _ENABLE_LOG=1
DEFINES += DL_PREFIX=\\\"lib\\\" DL_SUFFIX=\\\".so\\\"
DEFINES += _VAST_PLUGIN_PATH=\\\"$$OUT_PWD\\\"
DEFINES += _VAST_WIDGET_PATH=\\\"$$OUT_PWD\\\"

SOURCES = main.cpp $$files(widgets/*.cpp) $$files(viewer/*.cpp)
HEADERS = $$files(widgets/*.hpp) $$files(viewer/*.hpp)
FORMS   = $$files(forms/*.ui)

no_webkit{
	SOURCES -= widgets/helpdialog.cpp
	HEADERS -= widgets/helpdialog.hpp
	FORMS   -= forms/helpDialog.ui
} else {
	DEFINES += HAVE_WEBKIT
}

RESOURCES += $$files(resources/*.qrc)

#message($$SOURCES)

INCLUDEPATH += /tmp/include/isis
LIBS += -L/tmp/lib -lisis_core -lisisAdapter_qt4

