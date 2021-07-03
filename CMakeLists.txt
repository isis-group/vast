cmake_minimum_required(VERSION 3.17)

include(InstallRequiredSystemLibraries) #http://docdb.fnal.gov/ILC/sid/stanitz/ilcsoft-latest/cmake-2.4.8/Modules/InstallRequiredSystemLibraries.cmake

project(vast)

find_package(ISIS REQUIRED COMPONENTS Qt)

############################################################
# set install infix for plugin installation path
############################################################
#set(VAST_PLUGIN_INFIX lib/vast/plugins)
#set(VAST_WIDGET_INFIX lib/vast/widgets)
#set(VAST_DOC_INFIX share/vast/doc/html)

include_directories(widgets)

option(HAVE_WEBKIT "Set to off if you dont have QtWebKit (will disable help)" OFF)

# collecting files
file (GLOB VIEWER_FILES_CPP  viewer/*.cpp)
file (GLOB WIDGET_FILES_CPP widgets/*.cpp )
file (GLOB UI_FILES forms/*.ui)
file (GLOB RESOURCE_FILES resources/*.qrc)
file (GLOB INTERNAL_FILES_CPP viewer/internal/*.cpp )
#file (GLOB VIEWER_DEV_FILES viewer/*.hpp )
#file (GLOB WIDGET_DEV_FILES widgets/*.hpp )

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
# qt5 stuff
###########################################################

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-100 -Wextra -Wall -Wfatal-errors"  )
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wextra -Wall -Wfatal-errors" )

set(CMAKE_AUTOUIC_SEARCH_PATHS "forms")

############################################################
# RCS revision number
############################################################
find_package(Git QUIET REQUIRED)

execute_process(
		COMMAND "${GIT_EXECUTABLE}" describe --always HEAD
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		RESULT_VARIABLE res
		OUTPUT_VARIABLE MYAPP_VERSION
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)

set_property(GLOBAL APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/.git/index")

string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+).*$" MYAPP_VERSION "${MYAPP_VERSION}")

message(STATUS "version according to git ${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")

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
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "vast lipsia viewer")
set(CPACK_PACKAGE_VENDOR "the friendly lipsia team")
set(CPACK_PACKAGE_CONTACT "isis@cbs.mpg.de")
set(CPACK_PACKAGE_NAME "vast")
set(CPACK_BUNDLE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${VAST_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VAST_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VAST_VERSION_PATCH})
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_BUILD_TYPE})
#SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${${CMAKE_PROJECT_NAME}_VERSION_MAJOR}.${${CMAKE_PROJECT_NAME}_VERSION_MINOR}")
include(CPack)

###########################################################
# Build stuff
###########################################################

add_executable(vast main.cpp ${VIEWER_FILES_CPP} ${WIDGET_FILES_CPP} ${UI_FILES} ${RESOURCE_FILES} ${INTERNAL_FILES_CPP})

#add the plugin directory
#add_subdirectory(plugins)

#add the view_widgets directory
#add_subdirectory(view_widgets)

target_link_libraries(vast ISIS::isis_qt5 ${CMAKE_DL_LIBS})

install(TARGETS vast RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )

#install pixmap
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/vast.xpm DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/vast.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps)

# uninstall target
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake//cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
