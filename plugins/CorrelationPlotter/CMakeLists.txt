message(STATUS "Adding CorrelationPlotter plugin")

###########################################################
# qt4 stuff
###########################################################
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)

INCLUDE(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(plugin_moc_files CorrelationPlotter.hpp OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)
QT4_WRAP_UI(correlationplotter_ui_h forms/correlationPlotter.ui)
QT4_ADD_RESOURCES(correlationplotter_rcc_files resources/correlationplotter.qrc)

add_library(vastPlugin_CorrelationPlotter SHARED vastPlugin_CorrelationPlotter.cpp CorrelationPlotter.cpp ${correlationplotter_ui_h} ${plugin_moc_files} ${correlationplotter_rcc_files})
target_link_libraries(vastPlugin_CorrelationPlotter ${ISIS_LIB}  ${ISIS_LIB_DEPENDS} ${QT_LIBRARIES})

install(TARGETS vastPlugin_CorrelationPlotter DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )