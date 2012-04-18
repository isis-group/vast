############################################################
# include the vast core header files
############################################################
include_directories(${CMAKE_SOURCE_DIR}/viewer)


############################################################
# add global options
############################################################

option(${CMAKE_PROJECT_NAME}_PLUGIN_PROFILEPLOTTER "Enable ProfilePlotter plugin" OFF)
option(${CMAKE_PROJECT_NAME}_PLUGIN_ORIENTATIONCORRECTION "Enable OrientationCorrection plugin" OFF)
option(${CMAKE_PROJECT_NAME}_PLUGIN_MASKEDIT "Enable MaskEdit plugin" OFF)
option(${CMAKE_PROJECT_NAME}_PLUGIN_CORRELATIONPLOTTER "Enable CorrelationPlotter plugin" OFF)
option(${CMAKE_PROJECT_NAME}_PLUGIN_PYTHONINTERPRETER "Enable PythonInterpreter plugin" OFF)
option(${CMAKE_PROJECT_NAME}_PLUGIN_HISTOGRAM "Enable Histogram plugin" OFF)
option(${CMAKE_PROJECT_NAME}_PLUGIN_PROPERTYTOOL "Enable PropertyTool plugin" OFF)
option(${CMAKE_PROJECT_NAME}_PLUGIN_ITK_REGISTRATION "Enable itkRegistration plugin" OFF)
option(${CMAKE_PROJECT_NAME}_PLUGIN_VOXELOPERATION "Enable VoxelOperation plugin" OFF)

SET (CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS_INIT} -Wl,-undefined,dynamic_lookup)

############################################################
# TimePlotter plugin
############################################################
if(${CMAKE_PROJECT_NAME}_PLUGIN_PROFILEPLOTTER)
	add_subdirectory(ProfilePlotter)
endif(${CMAKE_PROJECT_NAME}_PLUGIN_PROFILEPLOTTER)

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

############################################################
# CorrelationPlotter plugin
############################################################
if(${CMAKE_PROJECT_NAME}_PLUGIN_CORRELATIONPLOTTER)
	add_subdirectory(CorrelationPlotter)
endif(${CMAKE_PROJECT_NAME}_PLUGIN_CORRELATIONPLOTTER)

############################################################
# PythonInterpreter plugin
############################################################
if(${CMAKE_PROJECT_NAME}_PLUGIN_PYTHONINTERPRETER)
	add_subdirectory(PythonInterpreter)
endif(${CMAKE_PROJECT_NAME}_PLUGIN_PYTHONINTERPRETER)

############################################################
# Histogram plugin
############################################################
if(${CMAKE_PROJECT_NAME}_PLUGIN_HISTOGRAM)
	add_subdirectory(Histogram)
endif(${CMAKE_PROJECT_NAME}_PLUGIN_HISTOGRAM)

############################################################
# PropertyTool plugin
############################################################
if(${CMAKE_PROJECT_NAME}_PLUGIN_PROPERTYTOOL)
	add_subdirectory(PropertyTool)
endif(${CMAKE_PROJECT_NAME}_PLUGIN_PROPERTYTOOL)

############################################################
# itkRegistration plugin
############################################################
if(${CMAKE_PROJECT_NAME}_PLUGIN_ITK_REGISTRATION)
	add_subdirectory(itkRegistration)
endif(${CMAKE_PROJECT_NAME}_PLUGIN_ITK_REGISTRATION)

############################################################
# VoxelOperation plugin
############################################################
if(${CMAKE_PROJECT_NAME}_PLUGIN_VOXELOPERATION)
	add_subdirectory(VoxelOperation)
endif(${CMAKE_PROJECT_NAME}_PLUGIN_VOXELOPERATION)