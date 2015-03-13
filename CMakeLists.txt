cmake_minimum_required(VERSION 2.6)

include(InstallRequiredSystemLibraries) #http://docdb.fnal.gov/ILC/sid/stanitz/ilcsoft-latest/cmake-2.4.8/Modules/InstallRequiredSystemLibraries.cmake

PROJECT(vast)


############################################################
# set install infix for plugin installation path
############################################################
set(VAST_PLUGIN_INFIX lib/vast/plugins)
set(VAST_WIDGET_INFIX lib/vast/widgets)
set(VAST_DOC_INFIX share/vast/doc/html)

include_directories(widgets)

option(HAVE_WEBKIT "Set to off if you dont have QtWebKit (will disable help)" ON)

# collecting files
file (GLOB VIEWER_FILES_CPP  viewer/*.cpp)
file (GLOB WIDGET_FILES_CPP widgets/*.cpp )
file (GLOB UI_FILES forms/*.ui)
file (GLOB RESOURCE_FILES resources/*.qrc)
file (GLOB INTERNAL_FILES_CPP viewer/internal/*.cpp )
file (GLOB VIEWER_DEV_FILES viewer/*.hpp )
file (GLOB WIDGET_DEV_FILES widgets/*.hpp )

if(NOT HAVE_WEBKIT)
	list(REMOVE_ITEM WIDGET_FILES_CPP ${CMAKE_SOURCE_DIR}/widgets/helpdialog.cpp)
	list(REMOVE_ITEM WIDGET_DEV_FILES ${CMAKE_SOURCE_DIR}/widgets/helpdialog.hpp)
endif(NOT HAVE_WEBKIT)

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
	widgets/aboutDialog.hpp
)

if(HAVE_WEBKIT)
	set(VIEWER_FILES_HPP ${VIEWER_FILES_HPP} widgets/helpdialog.hpp)
endif(HAVE_WEBKIT)

##########################################################
# set viewer version number
##########################################################
set(VAST_VERSION_MAJOR "0")
set(VAST_VERSION_MINOR "7")
set(VAST_VERSION_PATCH "3")

add_definitions(-D_VAST_VERSION_MAJOR=${VAST_VERSION_MAJOR})
add_definitions(-D_VAST_VERSION_MINOR=${VAST_VERSION_MINOR})
add_definitions(-D_VAST_VERSION_PATCH=${VAST_VERSION_PATCH})

if(HAVE_WEBKIT)
add_definitions(-DHAVE_WEBKIT)
endif(HAVE_WEBKIT)

###########################################################
# documentation
###########################################################
if(HAVE_WEBKIT)
option(${CMAKE_PROJECT_NAME}_DOCUMENTATION "Build the documentation for vast" OFF)

IF(${CMAKE_PROJECT_NAME}_DOCUMENTATION)
	add_subdirectory(documentation)	
ENDIF(${CMAKE_PROJECT_NAME}_DOCUMENTATION)

set_source_files_properties( "${CMAKE_CURRENT_SOURCE_DIR}/widgets/helpdialog.cpp" PROPERTIES COMPILE_FLAGS
		"-DVAST_DOC_PATH=\\\"${CMAKE_INSTALL_PREFIX}/${VAST_DOC_INFIX}\\\"")
endif(HAVE_WEBKIT)

###########################################################
# isis stuff
###########################################################
SET(ISIS_DIR /usr/share/isis/cmake CACHE PATH 
    "The directory where the isis cmake configuration script is located.")
find_package(ISIS REQUIRED)

option(${CMAKE_PROJECT_NAME}_RUNTIME_LOG "Toggles the vast runtime logging" ON)
option(${CMAKE_PROJECT_NAME}_DEBUG_LOG "Toggles the vast debug logging" OFF)
option(${CMAKE_PROJECT_NAME}_ENABLE_DEV "Toggles vast development logging" ON)

# we use the log definitions of the core
IF(${CMAKE_PROJECT_NAME}_RUNTIME_LOG)
	ADD_DEFINITIONS(-D_ENABLE_LOG=1)
ELSE(${CMAKE_PROJECT_NAME}_RUNTIME_LOG)
	ADD_DEFINITIONS(-D_ENABLE_LOG=0)
ENDIF(${CMAKE_PROJECT_NAME}_RUNTIME_LOG)

# enable DEBUG_LOG
IF(${CMAKE_PROJECT_NAME}_DEBUG_LOG)
	ADD_DEFINITIONS(-D_ENABLE_DEBUG=1)
	MESSAGE(STATUS "Debug code enabled, this will will have a big impact on the performance")
ELSE(${CMAKE_PROJECT_NAME}_DEBUG_LOG)
	ADD_DEFINITIONS(-D_ENABLE_DEBUG=0)
ENDIF(${CMAKE_PROJECT_NAME}_DEBUG_LOG)

IF(${CMAKE_PROJECT_NAME}_ENABLE_DEV)
	add_definitions(-D_ENABLE_DEV=1)
else(${CMAKE_PROJECT_NAME}_ENABLE_DEV)
	add_definitions(-D_ENABLE_DEV=0)
	message(WARNING "vast development logging was turned off. Creating of log will not be possible!")
endif(${CMAKE_PROJECT_NAME}_ENABLE_DEV)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${ISIS_INCLUDE_DIRS})

###########################################################
# qt4 stuff
###########################################################

if(HAVE_WEBKIT)
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtWebKit REQUIRED)
else(HAVE_WEBKIT)
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)
endif(HAVE_WEBKIT)

set(QT_USE_QTUITOOLS TRUE)

INCLUDE(${QT_USE_FILE})

QT4_ADD_RESOURCES(vast_rcc_files ${RESOURCE_FILES})

qt4_wrap_cpp(vast_moc_files ${VIEWER_FILES_HPP}  OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)
QT4_WRAP_UI(vast_UIS_H ${UI_FILES})


###########################################################
# boost and common stuff
###########################################################

FIND_PACKAGE(Boost REQUIRED COMPONENTS thread)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/QImageWidget)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/viewer)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-100 -Wextra -Wall -Wfatal-errors"  )
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wextra -Wall -Wfatal-errors" )


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

set_source_files_properties( "viewer/widgetloader.cpp" PROPERTIES COMPILE_FLAGS
	"-D_VAST_WIDGET_PATH=\\\"${CMAKE_INSTALL_PREFIX}/${VAST_WIDGET_INFIX}\\\" -DDL_SUFFIX=\\\"${CMAKE_SHARED_LIBRARY_SUFFIX}\\\" -DDL_PREFIX=\\\"${CMAKE_SHARED_LIBRARY_PREFIX}\\\""
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
SET(CPACK_PACKAGE_VERSION_MAJOR ${VAST_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VAST_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VAST_VERSION_PATCH})
SET(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE})
#SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${${CMAKE_PROJECT_NAME}_VERSION_MAJOR}.${${CMAKE_PROJECT_NAME}_VERSION_MINOR}")
INCLUDE(CPack)

###########################################################
# Build stuff
###########################################################

add_executable(vast main.cpp ${VIEWER_FILES_CPP} ${WIDGET_FILES_CPP} ${vast_UIS_H} ${vast_moc_files} ${vast_rcc_files} ${INTERNAL_FILES_CPP})
find_library(isisqt4_LIBRARY "isisAdapter_qt4")

message(STATUS "QT libraries: ${QT_LIBRARIES} ")

set(NEEDED_LIBS
	${QT_LIBRARIES}
	${Boost_LIBRARIES}
	${ISIS_LIB}
	${ISIS_LIB_DEPENDS}
	${isisqt4_LIBRARY}
	)

#add the plugin directory
add_subdirectory(plugins)

#add the view_widgets directory
add_subdirectory(view_widgets)

target_link_libraries(vast ${NEEDED_LIBS} ${CMAKE_DL_LIBS})

install(TARGETS vast RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )

#install pixmap
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/vast.xpm DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/vast.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps)

# uninstall target
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake//cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
