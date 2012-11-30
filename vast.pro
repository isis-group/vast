
TEMPLATE = subdirs
SUBDIRS = widgets viewer main

#if main must be last:
CONFIG += ordered

#QImageWidget
#SOURCES += view_widgets/QImageWidget/QImageWidgetImplementation.cpp view_widgets/QImageWidget/QOrientationHandler.cpp
#HEADERS += view_widgets/QImageWidget/QImageWidgetImplementation.hpp view_widgets/QImageWidget/QOrientationHandler.hpp

#QGeometricalImageWidget
#SOURCES += view_widgets/QGeometricalImageWidget/geomhandler.cpp view_widgets/QGeometricalImageWidget/QGeomWidget.cpp
#HEADERS += view_widgets/QGeometricalImageWidget/geomhandler.hpp view_widgets/QGeometricalImageWidget/QGeomWidget.hpp


#message($$SOURCES)


