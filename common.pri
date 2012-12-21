debug {
	DEFINES += _ENABLE_DEBUG=1 _ENABLE_DEV=1
} else {
	DEFINES += _ENABLE_DEBUG=0 _ENABLE_DEV=0
}

DEFINES += _ENABLE_LOG=1

INCLUDEPATH += /usr/include/isis ..
LIBS += -L/usr/lib -lisis_core -lisisAdapter_qt4
DESTDIR     = ..


#add help if we have WebKit
!no_webkit{
	DEFINES += HAVE_WEBKIT
}
