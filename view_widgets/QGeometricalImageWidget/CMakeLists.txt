message(STATUS "Adding Qt-Geometrical widget")

###########################################################
# qt4 stuff
###########################################################
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_OPENGL TRUE )

INCLUDE(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(widget_moc_files QGeomWidget.hpp )

add_library(vastImageWidget_QtGeom SHARED QGeomWidget.cpp ${widget_moc_files} )
target_link_libraries(vastImageWidget_QtGeom ${ISIS_LIB}  ${ISIS_LIB_DEPENDS} ${QT_LIBRARIES})

install(TARGETS vastImageWidget_QtGeom DESTINATION ${VAST_WIDGET_INFIX} COMPONENT "vast widgets" )