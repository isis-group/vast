message(STATUS "Adding PropertyTool plugin")

###########################################################
# qt4 stuff
###########################################################
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_OPENGL TRUE )

INCLUDE(${QT_USE_FILE})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

QT4_ADD_RESOURCES(propertytool_rcc_files resources/propertytool.qrc)
qt4_wrap_cpp(plugin_moc_files PropertyToolDialog.hpp OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)
QT4_WRAP_UI(propertytool_ui_h forms/propertyToolDialog.ui)

add_library(vastPlugin_PropertyTool SHARED vastPlugin_PropertyTool.cpp PropertyToolDialog.cpp  ${propertytool_ui_h} ${plugin_moc_files} ${propertytool_rcc_files})
target_link_libraries(vastPlugin_PropertyTool ${ISIS_LIB} ${ISIS_LIB_DEPENDS} ${QT_LIBRARIES} )

install(TARGETS vastPlugin_PropertyTool DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )