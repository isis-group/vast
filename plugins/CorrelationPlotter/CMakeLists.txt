message(STATUS "Adding CorrelationPlotter plugin")

###########################################################
# qt4 stuff
###########################################################
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(plugin_moc_files CorrelationPlotter.hpp )
QT4_WRAP_UI(correlationplotter_ui_h forms/correlationPlotter.ui)

add_library(vastPlugin_CorrelationPlotter SHARED vastPlugin_CorrelationPlotter.cpp CorrelationPlotter.cpp ${correlationplotter_ui_h} ${plugin_moc_files})
target_link_libraries(vastPlugin_CorrelationPlotter isis_core  ${ISIS_LIB_DEPENDS} )

install(TARGETS vastPlugin_CorrelationPlotter DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )