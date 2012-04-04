message(STATUS "Adding QtDefault widget")

###########################################################
# qt4 stuff
###########################################################
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_OPENGL TRUE )

INCLUDE(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(widget_moc_files QImageWidgetImplementation.hpp OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)

add_library(vastImageWidget_QtDefault SHARED QImageWidgetImplementation.cpp QOrientationHandler.cpp ${widget_moc_files} )
target_link_libraries(vastImageWidget_QtDefault ${ISIS_LIB}  ${ISIS_LIB_DEPENDS} ${QT_LIBRARIES})

install(TARGETS vastImageWidget_QtDefault DESTINATION ${VAST_WIDGET_INFIX} COMPONENT "vast widgets" )