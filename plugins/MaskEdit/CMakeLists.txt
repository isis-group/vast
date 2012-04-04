message(STATUS "Adding MaskEdit plugin")

###########################################################
# qt4 stuff
###########################################################
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_OPENGL TRUE )

INCLUDE(${QT_USE_FILE})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(plugin_moc_files MaskEdit.hpp CreateMaskDialog.hpp OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)
QT4_WRAP_UI(orientationcorrection_ui_h forms/maskEdit.ui forms/createMask.ui)
QT4_ADD_RESOURCES(maskedit_rcc_files resources/maskedit.qrc)

add_library(vastPlugin_MaskEdit SHARED vastPlugin_MaskEdit.cpp MaskEdit.cpp CreateMaskDialog.cpp ${orientationcorrection_ui_h} ${plugin_moc_files} ${maskedit_rcc_files})
target_link_libraries(vastPlugin_MaskEdit ${ISIS_LIB}  ${ISIS_LIB_DEPENDS} ${QT_LIBRARIES})

install(TARGETS vastPlugin_MaskEdit DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )