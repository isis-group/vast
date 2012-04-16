message(STATUS "Adding OrientationCorrection plugin")

###########################################################
# qt4 stuff
###########################################################
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_OPENGL TRUE )

INCLUDE(${QT_USE_FILE})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(plugin_moc_files OrientationCorrection.hpp OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)
QT4_WRAP_UI(orientationcorrection_ui_h forms/orientationcorrection.ui)

add_library(vastPlugin_OrientationCorrection SHARED vastPlugin_OrientationCorrection.cpp OrientationCorrection.cpp ${orientationcorrection_ui_h} ${plugin_moc_files})
target_link_libraries(vastPlugin_OrientationCorrection ${ISIS_LIB}  ${ISIS_LIB_DEPENDS}  ${QT_LIBRARIES} )

install(TARGETS vastPlugin_OrientationCorrection DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )