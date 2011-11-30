cmake_minimum_required(VERSION 2.6)

PROJECT(vast)


############################################################
# set install infix for plugin installation path
############################################################
set(VAST_PLUGIN_INFIX lib/vast/plugins)

include_directories(widgets)

# collecting files
file (GLOB VIEWER_FILES_CPP  viewer/*.cpp)
file (GLOB WIDGET_FILES_CPP widgets/*.cpp )
file (GLOB QWIDGET_FILES_CPP QImageWidget/*.cpp )
file (GLOB UI_FILES forms/*.ui)
file (GLOB RESOURCE_FILES resources/*.qrc)

file (GLOB VIEWER_DEV_FILES viewer/*.hpp )
file (GLOB WIDGET_DEV_FILES widgets/*.hpp )

set(VIEWER_FILES_HPP
	viewer/qviewercore.hpp
	widgets/mainwindow.hpp
	widgets/preferenceDialog.hpp
	widgets/voxelInformationWidget.hpp
	widgets/imageStackWidget.hpp
	widgets/sliderwidget.hpp
	widgets/scalingWidget.hpp
	widgets/loggingDialog.hpp
	widgets/filedialog.hpp
	widgets/startwidget.hpp
)

set(GLWIDGET_FILES_HPP
	QImageWidget/QImageWidgetImplementation.hpp)

##########################################################
# set viewer version number
##########################################################
set(ISIS_VIEWER_VERSION_MAJOR "0")
set(ISIS_VIEWER_VERSION_MINOR "1")
set(ISIS_VIEWER_VERSION_PATCH "8")

add_definitions(-D_ISIS_VIEWER_VERSION_MAJOR=${ISIS_VIEWER_VERSION_MAJOR})
add_definitions(-D_ISIS_VIEWER_VERSION_MINOR=${ISIS_VIEWER_VERSION_MINOR})
add_definitions(-D_ISIS_VIEWER_VERSION_PATCH=${ISIS_VIEWER_VERSION_PATCH})

###########################################################
# isis stuff
###########################################################
SET(ISIS_DIR /usr/share/isis/cmake CACHE PATH 
    "The directory where the isis cmake configuration script is located.")
find_package(ISIS REQUIRED)

option(ISIS_RUNTIME_LOG "Toggles the isis runtime logging" ON)
option(ISIS_DEBUG_LOG "Toggles the isis debug logging" OFF)

# we use the log definitions of the core
IF(ISIS_RUNTIME_LOG)
	ADD_DEFINITIONS(-D_ENABLE_LOG=1)
ELSE(ISIS_RUNTIME_LOG)
	ADD_DEFINITIONS(-D_ENABLE_LOG=0)
ENDIF(ISIS_RUNTIME_LOG)

# enable DEBUG_LOG
IF(ISIS_DEBUG_LOG)
	ADD_DEFINITIONS(-D_ENABLE_DEBUG=1)
	MESSAGE(STATUS "Debug code enabled, this will will have a big impact on the performance")
ELSE(ISIS_DEBUG_LOG)
	ADD_DEFINITIONS(-D_ENABLE_DEBUG=0)
ENDIF(ISIS_DEBUG_LOG)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${ISIS_INCLUDE_DIRS})

###########################################################
# qt4 stuff
###########################################################

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_OPENGL TRUE )

INCLUDE(${QT_USE_FILE})

QT4_ADD_RESOURCES(vast_rcc_files ${RESOURCE_FILES})

qt4_wrap_cpp(vast_moc_files ${VIEWER_FILES_HPP} ${GLWIDGET_FILES_HPP})
QT4_WRAP_UI(vast_UIS_H ${UI_FILES})


###########################################################
# boost and common stuff
###########################################################

FIND_PACKAGE(Boost REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/QImageWidget)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/viewer)

#this flags has to be set due to use parallel with boos::signal

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-100 -Wextra -Wfatal-errors" )
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wextra -Wfatal-errors " )
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-100")

############################################################
# set the plugin directory and the system dependend shared
# library naming conventions
############################################################
set_source_files_properties( "viewer/pluginloader.cpp" PROPERTIES COMPILE_FLAGS
	"-DPLUGIN_PATH=\\\"${CMAKE_INSTALL_PREFIX}/${VAST_PLUGIN_INFIX}\\\" -DDL_SUFFIX=\\\"${CMAKE_SHARED_LIBRARY_SUFFIX}\\\" -DDL_PREFIX=\\\"${CMAKE_SHARED_LIBRARY_PREFIX}\\\""
)

###########################################################
# CPack config
###########################################################
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "vast lipsia viewer")
SET(CPACK_PACKAGE_VENDOR "the friendly lipsia team")
SET(CPACK_PACKAGE_CONTACT "tuerke@cbs.mpg.de")
SET(CPACK_PACKAGE_NAME "vast")
SET(CPACK_BUNDLE_NAME ${CPACK_PACKAGE_NAME})
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR ${ISIS_VIEWER_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${ISIS_VIEWER_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${ISIS_VIEWER_VERSION_PATCH})
SET(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE})
#SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${${CMAKE_PROJECT_NAME}_VERSION_MAJOR}.${${CMAKE_PROJECT_NAME}_VERSION_MINOR}")
INCLUDE(CPack)

###########################################################
# OPenMP config
###########################################################
option(VAST_OPENMP "Toggles the omp" OFF)

IF(VAST_OPENMP)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp ")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp ")
ENDIF(VAST_OPENMP)



add_executable(vast main.cpp ${QWIDGET_FILES_CPP} ${GLWIDGET_FILES_CPP} ${VIEWER_FILES_CPP} ${WIDGET_FILES_CPP} ${vast_UIS_H} ${vast_moc_files} ${vast_rcc_files})
find_library(isisqt4_LIBRARY "isisAdapter_qt4")

set(NEEDED_LIBS
	${QT_LIBRARIES}
	${Boost_LIBRARIES}
	${ISIS_LIB}
	${ISIS_LIB_DEPENDS}
	${isisqt4_LIBRARY}
	${QWT5_library} 
	)

#add the plugin directory
add_subdirectory(plugins)

target_link_libraries(vast ${NEEDED_LIBS})

install(TARGETS vast RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )

# install header files
install(FILES ${VIEWER_DEV_FILES} DESTINATION include/vast COMPONENT Development)
install(FILES ${WIDGET_DEV_FILES} DESTINATION include/vast COMPONENT Development)

#install pixmap
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/vast.xpm DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps)
