set(FILES_UI forms/histogramDialog.ui)

message(STATUS "Adding Histogram plugin")

find_path( QWT5_include qwt.h PATH_SUFFIXES qwt-qt4 )
find_library( QWT5_library qwt-qt4 )

MESSAGE(STATUS "Qwt library: ${QWT5_library}")
include_directories(${QWT5_include})

###########################################################
# qt4 stuff
###########################################################
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)

INCLUDE(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

QT4_ADD_RESOURCES(histogram_rcc_files resources/histogram.qrc)
qt4_wrap_cpp(plugin_moc_files HistogramDialog.hpp OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)
QT4_WRAP_UI(histogram_ui_h ${FILES_UI})

add_library(vastPlugin_Histogram SHARED vastPlugin_Histogram.cpp HistogramDialog.cpp ${histogram_ui_h} ${plugin_moc_files} ${histogram_rcc_files})
target_link_libraries(vastPlugin_Histogram ${ISIS_LIB}  ${ISIS_LIB_DEPENDS} ${QWT5_library} ${QT_LIBRARIES})

install(TARGETS vastPlugin_Histogram DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )