add_library(vastPlugin_Test SHARED vastPlugin_Test.cpp)
target_link_libraries(vastPlugin_Test isis_core  ${ISIS_LIB_DEPENDS})

set(TARGETS ${TARGETS} vastPlugin_Test)

###########################################################
# actual install
###########################################################
install(TARGETS ${TARGETS} DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )