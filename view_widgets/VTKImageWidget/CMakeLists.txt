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

qt4_wrap_cpp(widget_moc_files VTKImageWidgetImplementation.hpp )

###########################################################
# vtk stuff
###########################################################

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
set( NEEDED_LIBS QVTK ${VTK_LIBRARIES} )


add_library(vastImageWidget_VTK SHARED VTKImageWidgetImplementation.cpp VolumeHandler.cpp ${widget_moc_files} )
target_link_libraries(vastImageWidget_VTK QVTK ${ISIS_LIB}  ${ISIS_LIB_DEPENDS} ${QT_LIBRARIES} ${VTK_LIBRARIES})

install(TARGETS vastImageWidget_VTK DESTINATION ${VAST_WIDGET_INFIX} COMPONENT "vast widgets" )