! include( ../common.pri ) {
	error( Couldn't find the common.pri file! )
}


TEMPLATE = lib
CONFIG += staticlib
SOURCES += aboutDialog.cpp filedialog.cpp imageStackWidget.cpp keycommandsdialog.cpp loggingDialog.cpp mainwindow.cpp \
preferenceDialog.cpp scalingWidget.cpp sliderwidget.cpp startwidget.cpp voxelInformationWidget.cpp

HEADERS += aboutDialog.hpp filedialog.hpp imageStackWidget.hpp keycommandsdialog.hpp loggingDialog.hpp mainwindow.hpp \
preferenceDialog.hpp scalingWidget.hpp sliderwidget.hpp startwidget.hpp voxelInformationWidget.hpp

FORMS = aboutDialog.ui preferences.ui imageStackWidget.ui mainwindow.ui fileOpenWidget.ui loggingDialog.ui keyCommands.ui \
voxelInformationWidget.ui sliderWidget.ui scalingDialog.ui isisViewerMain.ui pluginInformation.ui start.ui

#add help if we have WebKit
!no_webkit{
	SOURCES += helpdialog.cpp
	HEADERS += helpdialog.hpp
}
