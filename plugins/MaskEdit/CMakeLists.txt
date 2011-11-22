message(STATUS "Adding MaskEdit plugin")

###########################################################
# qt4 stuff
###########################################################
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(plugin_moc_files MaskEdit.hpp CreateMaskDialog.hpp)
QT4_WRAP_UI(orientationcorrection_ui_h forms/maskEdit.ui forms/createMask.ui)
QT4_ADD_RESOURCES(maskedit_rcc_files resources/maskedit.qrc)

add_library(vastPlugin_MaskEdit SHARED vastPlugin_MaskEdit.cpp MaskEdit.cpp CreateMaskDialog.cpp ${orientationcorrection_ui_h} ${plugin_moc_files} ${maskedit_rcc_files})
target_link_libraries(vastPlugin_MaskEdit isis_core  ${ISIS_LIB_DEPENDS} )

install(TARGETS vastPlugin_MaskEdit DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )