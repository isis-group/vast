set(FILES_UI forms/voxelOperation.ui)

message(STATUS "Adding VoxelOperation plugin")

###########################################################
# muparser stuff
###########################################################
find_library(LIB_MUPARSER "muparser" REQUIRED)
find_path(INCPATH_MUPARSER "muParser.h" PATH_SUFFIXES "muParser")
include_directories(${INCPATH_MUPARSER})

###########################################################
# qt4 stuff
###########################################################
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTOPENGL TRUE)

INCLUDE(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_wrap_cpp(plugin_moc_files VoxelOperationDialog.hpp OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)
QT4_WRAP_UI(voxeloperation_ui_h ${FILES_UI})

add_library(vastPlugin_VoxelOperation SHARED vastPlugin_VoxelOperation.cpp VoxelOperationDialog.cpp ${voxeloperation_ui_h} ${plugin_moc_files} )
target_link_libraries(vastPlugin_VoxelOperation ${LIB_MUPARSER} ${ISIS_LIB}  ${ISIS_LIB_DEPENDS} ${QT_LIBRARIES} )

install(TARGETS vastPlugin_VoxelOperation DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )