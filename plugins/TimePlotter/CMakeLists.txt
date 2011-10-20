
set(FILES_HPP vastPlugin_TimePlotter)
set(FILES_UI forms/plotting.ui)

message(STATUS "Adding TimePlotter plugin")


find_path( QWT5_include qwt.h PATH_SUFFIXES qwt-qt4 )
find_library( QWT5_library qwt-qt4 )

MESSAGE(STATUS "Qwt library: ${QWT5_library}")
include_directories(${QWT5_include})

###########################################################
# qt4 stuff
###########################################################
include_directories(${CMAKE_CURRENT_BINARY_DIR})
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)

INCLUDE(${QT_USE_FILE})

QT4_WRAP_UI(timeplotter_ui_h ${FILES_UI})

add_library(vastPlugin_TimePlotter SHARED vastPlugin_TimePlotter.cpp ${timeplotter_ui_h} )
target_link_libraries(vastPlugin_TimePlotter isis_core  ${ISIS_LIB_DEPENDS} ${QWT5_library} )

install(TARGETS vastPlugin_TimePlotter DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )