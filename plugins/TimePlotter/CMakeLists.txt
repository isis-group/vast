#find qwt
find_path( QWT5_include qwt.h PATH_SUFFIXES qwt-qt4 )
find_library( QWT5_library qwt-qt4 )

MESSAGE(STATUS "Qwt library: ${QWT5_library}")

include_directories(${QWT5_include})
add_library(vastPlugin_TimePlotter SHARED vastPlugin_TimePlotter.cpp)
target_link_libraries(vastPlugin_TimePlotter isis_core  ${ISIS_LIB_DEPENDS} ${QWT5_library} )

set(TARGETS ${TARGETS} vastPlugin_TimePlotter)