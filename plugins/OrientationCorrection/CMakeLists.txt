message(STATUS "Adding OrientationCorrection plugin")

###########################################################
# qt4 stuff
###########################################################
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(plugin_moc_files OrientationCorrection.hpp)
QT4_WRAP_UI(orientationcorrection_ui_h forms/orientationcorrection.ui)

add_library(vastPlugin_OrientationCorrection SHARED vastPlugin_OrientationCorrection.cpp OrientationCorrection.cpp ${orientationcorrection_ui_h} ${plugin_moc_files})
target_link_libraries(vastPlugin_OrientationCorrection isis_core  ${ISIS_LIB_DEPENDS} )

install(TARGETS vastPlugin_OrientationCorrection DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )