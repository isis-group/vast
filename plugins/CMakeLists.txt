############################################################
# include the vast core header files
############################################################
include_directories(${CMAKE_SOURCE_DIR}/viewer)


############################################################
# add global options
############################################################

option(${CMAKE_PROJECT_NAME}_PLUGIN_TIMEPLOTTER "Enable TimePlotter plugin" OFF)
option(${CMAKE_PROJECT_NAME}_PLUGIN_ORIENTATIONCORRECTION "Enable OrientationCorrection plugin" OFF)
option(${CMAKE_PROJECT_NAME}_PLUGIN_MASKEDIT "Enable MaskEdit plugin" OFF)

############################################################
# TimePlotter plugin
############################################################
if(${CMAKE_PROJECT_NAME}_PLUGIN_TIMEPLOTTER)
	add_subdirectory(TimePlotter)
endif(${CMAKE_PROJECT_NAME}_PLUGIN_TIMEPLOTTER)

############################################################
# OrientationCorrection plugin
############################################################
if(${CMAKE_PROJECT_NAME}_PLUGIN_ORIENTATIONCORRECTION)
	add_subdirectory(OrientationCorrection)
endif(${CMAKE_PROJECT_NAME}_PLUGIN_ORIENTATIONCORRECTION)

############################################################
# MaskEdit plugin
############################################################
if(${CMAKE_PROJECT_NAME}_PLUGIN_MASKEDIT)
	add_subdirectory(MaskEdit)
endif(${CMAKE_PROJECT_NAME}_PLUGIN_MASKEDIT)
