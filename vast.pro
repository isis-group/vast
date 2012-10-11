QT += core gui

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

SOURCES = main.cpp \
widgets/aboutDialog.cpp widgets/filedialog.cpp widgets/imageStackWidget.cpp widgets/keycommandsdialog.cpp widgets/loggingDialog.cpp widgets/mainwindow.cpp \
widgets/preferenceDialog.cpp widgets/scalingWidget.cpp widgets/sliderwidget.cpp widgets/startwidget.cpp widgets/voxelInformationWidget.cpp \
viewer/color.cpp viewer/common.cpp viewer/fileinformation.cpp viewer/geometrical.cpp viewer/imageholder.cpp viewer/memoryhandler.cpp viewer/nativeimageops.cpp \
viewer/pluginloader.cpp viewer/qprogressfeedback.cpp viewer/qviewercore.cpp viewer/settings.cpp viewer/style.cpp viewer/uicore.cpp viewer/viewercorebase.cpp \
viewer/widgetensemblecomponent.cpp viewer/widgetensemble.cpp viewer/widgetloader.cpp


HEADERS = widgets/aboutDialog.hpp widgets/filedialog.hpp widgets/imageStackWidget.hpp widgets/keycommandsdialog.hpp widgets/loggingDialog.hpp widgets/mainwindow.hpp \
widgets/preferenceDialog.hpp widgets/scalingWidget.hpp widgets/sliderwidget.hpp widgets/startwidget.hpp widgets/voxelInformationWidget.hpp \
viewer/color.hpp viewer/common.hpp viewer/error.hpp viewer/fileinformation.hpp viewer/geometrical.hpp viewer/imageholder.hpp viewer/memoryhandler.hpp viewer/nativeimageops.hpp \
viewer/pluginloader.hpp viewer/qprogressfeedback.hpp viewer/qviewercore.hpp viewer/settings.hpp viewer/style.hpp viewer/uicore.hpp viewer/viewercorebase.hpp \
viewer/widgetensemblecomponent.hpp viewer/widgetensemble.hpp viewer/widgetloader.hpp

FORMS   = forms/aboutDialog.ui forms/fileOpenWidget.ui forms/imageStackWidget.ui forms/isisViewerMain.ui forms/keyCommands.ui forms/loggingDialog.ui forms/mainwindow.ui \
forms/pluginInformation.ui forms/preferences.ui forms/scalingDialog.ui forms/sliderWidget.ui forms/start.ui forms/voxelInformationWidget.ui

RESOURCES = resources/viewer.qrc

INCLUDEPATH += /tmp/include/isis
LIBS += -L/tmp/lib -lisis_core -lisisAdapter_qt4

#add help if we have WebKit
!no_webkit{
	DEFINES += HAVE_WEBKIT
	SOURCES += widgets/helpdialog.cpp
	HEADERS += widgets/helpdialog.hpp
	FORMS   += forms/helpDialog.ui
}

#QImageWidget
SOURCES += view_widgets/QImageWidget/QImageWidgetImplementation.cpp view_widgets/QImageWidget/QOrientationHandler.cpp
HEADERS += view_widgets/QImageWidget/QImageWidgetImplementation.hpp view_widgets/QImageWidget/QOrientationHandler.hpp

#QGeometricalImageWidget
SOURCES += view_widgets/QGeometricalImageWidget/geomhandler.cpp view_widgets/QGeometricalImageWidget/QGeomWidget.cpp
HEADERS += view_widgets/QGeometricalImageWidget/geomhandler.hpp view_widgets/QGeometricalImageWidget/QGeomWidget.hpp


#message($$SOURCES)


