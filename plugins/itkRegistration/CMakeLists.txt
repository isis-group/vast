message(STATUS "Adding itkRegistration plugin")

###########################################################
# itk4 stuff
###########################################################
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth-100")

if( "${ITK_VERSION_MAJOR}" LESS 4 )
  message( WARNING "You are using ITK below version 4. Try to use ITKReview module!" )
  set(VAST_ITK_LIBRARIES ITKReview ${ITK_LIBRARIES})
else( "${ITK_VERSION_MAJOR}" LESS 4 )
  set(VAST_ITK_LIBRARIES  ${ITK_LIBRARIES})
endif( "${ITK_VERSION_MAJOR}" LESS 4 )
###########################################################
# qt4 stuff
###########################################################
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTOPENGL TRUE)
set(QT_USE_OPENGL TRUE )

INCLUDE(${QT_USE_FILE})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

QT4_ADD_RESOURCES(itkRegistration_rcc_files resources/itkRegistration.qrc)
qt4_wrap_cpp(plugin_moc_files RegistrationDialog.hpp OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)
QT4_WRAP_UI(itkRegistration_ui_h forms/itkRegistrationDialog.ui)

add_library(vastPlugin_itkRegistration SHARED vastPlugin_itkRegistration.cpp RegistrationDialog.cpp  ${itkRegistration_ui_h} ${plugin_moc_files} ${itkRegistration_rcc_files})
target_link_libraries(vastPlugin_itkRegistration ${ISIS_LIB} ${ISIS_LIB_DEPENDS} ${QT_LIBRARIES} ${VAST_ITK_LIBRARIES})

install(TARGETS vastPlugin_itkRegistration DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )