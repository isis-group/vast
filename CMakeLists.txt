cmake_minimum_required(VERSION 2.6)

PROJECT(vast)

# collecting files
file (GLOB VIEWER_FILES_CPP viewer/*.c viewer/*.cpp)
file (GLOB GLWIDGET_FILES_CPP GLWidget/*.cpp GLWidget/*.c)
file (GLOB UI_FILES forms/*.ui)
file (GLOB RESOURCE_FILES resources/*.qrc)

set(VIEWER_FILES_HPP
	viewer/MainWindowBase.hpp
	viewer/MainWindow.hpp
	viewer/MainWindowUIInterface.hpp
	viewer/QViewerCore.hpp
	viewer/QPreferencesDialog.hpp
	viewer/QPlottingDialog.hpp
)
set(GLWIDGET_FILES_HPP
	GLWidget/QGLWidgetImplementation.hpp)

##########################################################
# set viewer version number
##########################################################
set(ISIS_VIEWER_VERSION_MAJOR "0")
set(ISIS_VIEWER_VERSION_MINOR "1")
set(ISIS_VIEWER_VERSION_PATCH "0")

add_definitions(-D_ISIS_VIEWER_VERSION_MAJOR=${ISIS_VIEWER_VERSION_MAJOR})
add_definitions(-D_ISIS_VIEWER_VERSION_MINOR=${ISIS_VIEWER_VERSION_MINOR})
add_definitions(-D_ISIS_VIEWER_VERSION_PATCH=${ISIS_VIEWER_VERSION_PATCH})

###########################################################
# isis stuff
###########################################################
SET(ISIS_DIR /usr/share/isis/cmake CACHE PATH 
    "The directory where the isis cmake configuration skript script is located.")
find_package(ISIS REQUIRED)
include_directories(${ISIS_INCLUDE_DIRS})

option(ISIS_RUNTIME_LOG "Toggles the isis runtime logging" ON)
option(ISIS_DEBUG_LOG "Toggles the isis debug logging" ON) 

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

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtOpenGL Qwt REQUIRED)

set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_OPENGL TRUE )

INCLUDE(${QT_USE_FILE})

QT4_ADD_RESOURCES(vast_rcc_files ${RESOURCE_FILES})

qt4_wrap_cpp(vast_moc_files ${VIEWER_FILES_HPP} ${GLWIDGET_FILES_HPP})
QT4_WRAP_UI(vast_UIS_H ${UI_FILES})

###########################################################
# qwt stuff
###########################################################

find_path( QWT5_include qwt.h PATH_SUFFIXES qwt-qt4 )
find_library( QWT5_library qwt-qt4 )

MESSAGE(STATUS "Qwt library: ${QWT5_library}")

include_directories(${QWT5_include})


###########################################################
# opengl stuff
###########################################################

find_package(OpenGL REQUIRED)

if(NOT OpenGL)
	find_library(OpenGL "GL" PATHS "/usr/lib" "/usr/local/lib" "/usr/lib/mesa" "/usr/local/lib/mesa")
endif(NOT OpenGL)

message(STATUS "openGL library: ${OPENGL_LIBRARY}")

include_directories(${OPENGL_INCLUDE_DIR})

###########################################################
# boost and common stuff
###########################################################

FIND_PACKAGE(Boost REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GLWidget)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/viewer)

#this flags has to be set due to use parallel with boos::signal

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-100 -Wextra -Wfatal-errors" )
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wextra -Wfatal-errors " )
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-100")

add_executable(vast main.cpp ${GLWIDGET_FILES_CPP} ${VIEWER_FILES_CPP} ${vast_UIS_H} ${vast_moc_files} ${vast_rcc_files})
find_library(isisqt4_LIBRARY "isisAdapter_qt4")

set(NEEDED_LIBS
	${OPENGL_LIBRARY} 
	${QT_LIBRARIES}
	${Boost_LIBRARIES}
	${ISIS_LIB}
	${ISIS_LIB_DEPENDS}
	${isisqt4_LIBRARY}
	${QWT5_library} 
	)

target_link_libraries(vast ${NEEDED_LIBS})

install(TARGETS vast RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
