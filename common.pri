QT += core gui

debug {
	DEFINES += _ENABLE_DEBUG=1 _ENABLE_DEV=1
} else {
	DEFINES += _ENABLE_DEBUG=0 _ENABLE_DEV=0
}

DEFINES += _ENABLE_LOG=1
DEFINES += DL_PREFIX=\\\"lib\\\" DL_SUFFIX=\\\".so\\\"
DEFINES += _VAST_PLUGIN_PATH=\\\"$$OUT_PWD\\\"
DEFINES += _VAST_WIDGET_PATH=\\\"$$OUT_PWD\\\"

INCLUDEPATH += /tmp/include/isis ..
LIBS += -L/tmp/lib -lisis_core -lisisAdapter_qt4
DESTDIR = ..


#add help if we have WebKit
!no_webkit{
	DEFINES += HAVE_WEBKIT
}
