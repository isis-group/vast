############################################################
# include the vast core header files
############################################################
include_directories(${CMAKE_SOURCE_DIR}/viewer)

#we always want to add the QImageWidget
add_subdirectory(QImageWidget)

############################################################
# add global options
############################################################

option(${CMAKE_PROJECT_NAME}_WIDGET_VTK "Enable VTK rendering widget" OFF)
option(${CMAKE_PROJECT_NAME}_WIDGET_QGEOM "Enable Qt Geometrical widget" OFF)

SET (CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS_INIT} -Wl,-undefined,dynamic_lookup)

############################################################
# VTK widget
############################################################
if(${CMAKE_PROJECT_NAME}_WIDGET_VTK)
	add_subdirectory(VTKImageWidget)
endif(${CMAKE_PROJECT_NAME}_WIDGET_VTK)

############################################################
# QGeom widget
############################################################
if(${CMAKE_PROJECT_NAME}_WIDGET_QGEOM)
	add_subdirectory(QGeometricalImageWidget)
endif(${CMAKE_PROJECT_NAME}_WIDGET_QGEOM)