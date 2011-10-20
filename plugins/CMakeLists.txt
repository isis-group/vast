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
	add_subdirectory(TimePlotter)
endif(${CMAKE_PROJECT_NAME}_PLUGIN_TIMEPLOTTER)

###########################################################
# actual install
###########################################################
install(TARGETS ${TARGETS} DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )