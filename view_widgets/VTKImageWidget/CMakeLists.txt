message(STATUS "Adding VTK rendering widget")

###########################################################
# qt4 stuff
###########################################################
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_OPENGL TRUE )

INCLUDE(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

QT4_WRAP_UI(vtkoption_ui_h forms/option_widget.ui)
qt4_wrap_cpp(widget_moc_files VTKImageWidgetImplementation.hpp OptionWidget.hpp OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)
QT4_ADD_RESOURCES(vtkwidget_rcc_files resources/vtkwidget.qrc)

###########################################################
# vtk stuff
###########################################################

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
set( NEEDED_LIBS QVTK ${VTK_LIBRARIES} )

add_library(vastImageWidget_VTK SHARED VTKImageWidgetImplementation.cpp VolumeHandler.cpp ImageComponents.cpp  OptionWidget.cpp ${widget_moc_files} ${vtkoption_ui_h} ${vtkwidget_rcc_files})
target_link_libraries(vastImageWidget_VTK QVTK ${ISIS_LIB}  ${ISIS_LIB_DEPENDS} ${QT_LIBRARIES} ${VTK_LIBRARIES})

install(TARGETS vastImageWidget_VTK DESTINATION ${VAST_WIDGET_INFIX} COMPONENT "vast widgets" )