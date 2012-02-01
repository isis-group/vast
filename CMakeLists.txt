cmake_minimum_required(VERSION 2.6)

include(InstallRequiredSystemLibraries) #http://docdb.fnal.gov/ILC/sid/stanitz/ilcsoft-latest/cmake-2.4.8/Modules/InstallRequiredSystemLibraries.cmake

PROJECT(vast)


############################################################
# set install infix for plugin installation path
############################################################
set(VAST_PLUGIN_INFIX lib/vast/plugins)
set(VAST_DOC_INFIX share/vast/doc/html)

include_directories(widgets)

# collecting files
file (GLOB VIEWER_FILES_CPP  viewer/*.cpp)
file (GLOB WIDGET_FILES_CPP widgets/*.cpp )
file (GLOB QWIDGET_FILES_CPP QImageWidget/*.cpp )
file (GLOB UI_FILES forms/*.ui)
file (GLOB RESOURCE_FILES resources/*.qrc)
file (GLOB INTERNAL_FILES_CPP viewer/internal/*.cpp )
file (GLOB VIEWER_DEV_FILES viewer/*.hpp )
file (GLOB WIDGET_DEV_FILES widgets/*.hpp )

set(VIEWER_FILES_HPP
	viewer/qviewercore.hpp
	viewer/uicore.hpp
	widgets/mainwindow.hpp
	widgets/preferenceDialog.hpp
	widgets/voxelInformationWidget.hpp
	widgets/imageStackWidget.hpp
	widgets/sliderwidget.hpp
	widgets/scalingWidget.hpp
	widgets/loggingDialog.hpp
	widgets/filedialog.hpp
	widgets/startwidget.hpp
	widgets/keycommandsdialog.hpp
	widgets/helpdialog.hpp
	widgets/aboutDialog.hpp
)

set(WIDGET_FILES_HPP
	QImageWidget/QImageWidgetImplementation.hpp)

##########################################################
# set viewer version number
##########################################################
set(VAST_VERSION_MAJOR "0")
set(VAST_VERSION_MINOR "3")
set(VAST_VERSION_PATCH "0")

add_definitions(-D_VAST_VERSION_MAJOR=${VAST_VERSION_MAJOR})
add_definitions(-D_VAST_VERSION_MINOR=${VAST_VERSION_MINOR})
add_definitions(-D_VAST_VERSION_PATCH=${VAST_VERSION_PATCH})


###########################################################
# documentation
###########################################################
option(VAST_DOCUMENTATION "Build the documentation for vast" OFF)

IF(VAST_DOCUMENTATION)
	add_subdirectory(documentation)	
ENDIF(VAST_DOCUMENTATION)

set_source_files_properties( "${CMAKE_CURRENT_SOURCE_DIR}/widgets/helpdialog.cpp" PROPERTIES COMPILE_FLAGS
		"-DVAST_DOC_PATH=\\\"${CMAKE_INSTALL_PREFIX}/${VAST_DOC_INFIX}\\\"")

###########################################################
# isis stuff
###########################################################
SET(ISIS_DIR /usr/share/isis/cmake CACHE PATH 
    "The directory where the isis cmake configuration script is located.")
find_package(ISIS REQUIRED)

option(VAST_RUNTIME_LOG "Toggles the vast runtime logging" ON)
option(VAST_DEBUG_LOG "Toggles the vast debug logging" OFF)
option(VAST_ENABLE_DEV "Toggles vast development logging" ON)

# we use the log definitions of the core
IF(VAST_RUNTIME_LOG)
	ADD_DEFINITIONS(-D_ENABLE_LOG=1)
ELSE(VAST_RUNTIME_LOG)
	ADD_DEFINITIONS(-D_ENABLE_LOG=0)
ENDIF(VAST_RUNTIME_LOG)

# enable DEBUG_LOG
IF(VAST_DEBUG_LOG)
	ADD_DEFINITIONS(-D_ENABLE_DEBUG=1)
	MESSAGE(STATUS "Debug code enabled, this will will have a big impact on the performance")
ELSE(VAST_DEBUG_LOG)
	ADD_DEFINITIONS(-D_ENABLE_DEBUG=0)
ENDIF(VAST_DEBUG_LOG)

IF(VAST_ENABLE_DEV)
	add_definitions(-D_ENABLE_DEV=1)
else(VAST_ENABLE_DEV)
	add_definitions(-D_ENABLE_DEV=0)
	message(WARNING "vast development logging was turned off. Creating of log will not be possible!")
endif(VAST_ENABLE_DEV)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${ISIS_INCLUDE_DIRS})

###########################################################
# qt4 stuff
###########################################################

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtWebKit REQUIRED)

set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_OPENGL TRUE )

INCLUDE(${QT_USE_FILE})

QT4_ADD_RESOURCES(vast_rcc_files ${RESOURCE_FILES})

qt4_wrap_cpp(vast_moc_files ${VIEWER_FILES_HPP} ${WIDGET_FILES_HPP})
QT4_WRAP_UI(vast_UIS_H ${UI_FILES})

find_library(QT4_WEBKIT_LIB QtWebKit)

###########################################################
# boost and common stuff
###########################################################

FIND_PACKAGE(Boost REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/QImageWidget)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/viewer)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-100 -Wextra -Wall -Wfatal-errors"  )
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wextra -Wall -Wfatal-errors -Wall" )


############################################################
# RCS revision number
############################################################
# The revision number is based on GIT commit informations.
# It will be stored in GIT_REVISION.
find_program(GIT_COMMAND git DOC "The rcs revision description information tool.")

if (NOT GIT_REVISION)
  if (GIT_COMMAND)
    execute_process(COMMAND ${GIT_COMMAND} describe OUTPUT_VARIABLE OUT WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX MATCH "[0-9]+-[^-]+$" GIT_REVISION "${OUT}") #@todo wont work with more complex revisions/modified sources
    set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${GIT_REVISION}-${CMAKE_SYSTEM_NAME}-${PROCESSOR}-${CMAKE_BUILD_TYPE})
  else(GIT_COMMAND)
    message(STATUS "git not found")
  endif (GIT_COMMAND)
endif (NOT GIT_REVISION)

if (GIT_REVISION)
  message(STATUS "RCS Revision: ${GIT_REVISION}")
  add_definitions(-DVAST_RCS_REVISION=${GIT_REVISION})
  set(ISIS_CORE_REV_STRING ${${CMAKE_PROJECT_NAME}_VERSION_MAJOR}.${${CMAKE_PROJECT_NAME}_VERSION_MINOR}.${${CMAKE_PROJECT_NAME}_VERSION_PATCH}-${GIT_REVISION})
else(GIT_REVISION)
  set(ISIS_CORE_REV_STRING ${${CMAKE_PROJECT_NAME}_VERSION_MAJOR}.${${CMAKE_PROJECT_NAME}_VERSION_MINOR}.${${CMAKE_PROJECT_NAME}_VERSION_PATCH})
endif(GIT_REVISION)



############################################################
# set the plugin directory and the system dependend shared
# library naming conventions
############################################################
set_source_files_properties( "viewer/pluginloader.cpp" PROPERTIES COMPILE_FLAGS
	"-D_VAST_PLUGIN_PATH=\\\"${CMAKE_INSTALL_PREFIX}/${VAST_PLUGIN_INFIX}\\\" -DDL_SUFFIX=\\\"${CMAKE_SHARED_LIBRARY_SUFFIX}\\\" -DDL_PREFIX=\\\"${CMAKE_SHARED_LIBRARY_PREFIX}\\\""
)

############################################################
# set the documentation path
############################################################

###########################################################
# CPack config
###########################################################
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "vast lipsia viewer")
SET(CPACK_PACKAGE_VENDOR "the friendly lipsia team")
SET(CPACK_PACKAGE_CONTACT "tuerke@cbs.mpg.de")
SET(CPACK_PACKAGE_NAME "vast")
SET(CPACK_BUNDLE_NAME ${CPACK_PACKAGE_NAME})
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR ${VAST_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VAST_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VAST_VERSION_PATCH})
SET(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE})
#SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${${CMAKE_PROJECT_NAME}_VERSION_MAJOR}.${${CMAKE_PROJECT_NAME}_VERSION_MINOR}")
INCLUDE(CPack)

###########################################################
# OPenMP config
###########################################################
option(VAST_OPENMP "Toggles the omp support for vast." OFF)

IF(VAST_OPENMP)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp ")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp ")
ENDIF(VAST_OPENMP)

add_executable(vast main.cpp ${QWIDGET_FILES_CPP} ${WIDGET_FILES_CPP} ${VIEWER_FILES_CPP} ${WIDGET_FILES_CPP} ${vast_UIS_H} ${vast_moc_files} ${vast_rcc_files} ${INTERNAL_FILES_CPP})
find_library(isisqt4_LIBRARY "isisAdapter_qt4")

message(STATUS "QT libraries: ${QT_LIBRARIES} ")

set(NEEDED_LIBS
	${QT_LIBRARIES}
    ${QT4_WEBKIT_LIB}
	${Boost_LIBRARIES}
	${ISIS_LIB}
	${ISIS_LIB_DEPENDS}
	${isisqt4_LIBRARY}
	)

#add the plugin directory
add_subdirectory(plugins)

target_link_libraries(vast ${NEEDED_LIBS})

install(TARGETS vast RUNTIME DESTINATION bin )

# install header files
install(FILES ${VIEWER_DEV_FILES} DESTINATION include/vast COMPONENT Development)

#install(FILES ${WIDGET_DEV_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/vast COMPONENT Development)

#install pixmap
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/vast.xpm DESTINATION share/pixmaps)

# uninstall target
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake//cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)