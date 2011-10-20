############################################################
# include the vast core header files
############################################################
include_directories(${CMAKE_SOURCE_DIR}/viewer)


############################################################
# add global options
############################################################

option(${CMAKE_PROJECT_NAME}_PLUGIN_TIMEPLOTTER "Enable TimePlotter plugin" OFF)


############################################################
# TimePlotter plugin
############################################################
if(${CMAKE_PROJECT_NAME}_PLUGIN_TIMEPLOTTER)

	#find qwt
	find_path( QWT5_include qwt.h PATH_SUFFIXES qwt-qt4 )
	find_library( QWT5_library qwt-qt4 )

	MESSAGE(STATUS "Qwt library: ${QWT5_library}")

	include_directories(${QWT5_include})
	add_library(vastPlugin_TimePlotter SHARED vastPlugin_TimePlotter.cpp)
	target_link_libraries(vastPlugin_TimePlotter isis_core  ${ISIS_LIB_DEPENDS} ${QWT5_library} )

	set(TARGETS ${TARGETS} vastPlugin_TimePlotter)
endif(${CMAKE_PROJECT_NAME}_PLUGIN_TIMEPLOTTER)

###########################################################
# actual install
###########################################################
install(TARGETS ${TARGETS} DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )