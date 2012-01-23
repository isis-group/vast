set(FILES_UI forms/plotting.ui)

message(STATUS "Adding TimePlotter plugin")

# add path for new modules
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

find_path( QWT5_include qwt.h PATH_SUFFIXES qwt-qt4 )
find_library( QWT5_library qwt-qt4 )

MESSAGE(STATUS "Qwt library: ${QWT5_library}")
include_directories(${QWT5_include})

###########################################################
# qt4 stuff
###########################################################
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTOPENGL TRUE)

INCLUDE(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

QT4_ADD_RESOURCES(timeplotter_rcc_files resources/timeplotter.qrc)
qt4_wrap_cpp(plugin_moc_files PlotterDialog.hpp)
QT4_WRAP_UI(timeplotter_ui_h ${FILES_UI})

# look for fftw3
FIND_PACKAGE(FFTW3 REQUIRED)

add_library(vastPlugin_TimePlotter SHARED vastPlugin_TimePlotter.cpp PlotterDialog.cpp ${timeplotter_ui_h} ${plugin_moc_files} ${timeplotter_rcc_files})
target_link_libraries(vastPlugin_TimePlotter isis_core  ${ISIS_LIB_DEPENDS} ${QWT5_library} ${QT_LIBRARIES} ${FFTW3_FFTW3_LIBRARY})

install(TARGETS vastPlugin_TimePlotter DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )