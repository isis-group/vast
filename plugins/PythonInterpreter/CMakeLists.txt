message(STATUS "Adding PythonInterpreter plugin")

###########################################################
# qt4 stuff
###########################################################
include_directories(${CMAKE_CURRENT_BINARY_DIR})

QT4_ADD_RESOURCES(pythoninterpreter_rcc_files resources/pythoninterpreter.qrc)
# qt4_wrap_cpp(plugin_moc_files PlotterDialog.hpp)
# QT4_WRAP_UI(pythoninterpreter_ui_h ${FILES_UI})

###########################################################
# python stuff
###########################################################


find_package(Boost REQUIRED COMPONENTS python)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})


add_library(vastPlugin_PythonInterpreter SHARED vastPlugin_PythonInterpreter.cpp ${pythoninterpreter_ui_h} ${plugin_moc_files} ${pythoninterpreter_rcc_files})
target_link_libraries(vastPlugin_PythonInterpreter isis_core  ${ISIS_LIB_DEPENDS} ${PYTHON_LIBRARIES} ${Boost_LIBRARIES} )

install(TARGETS vastPlugin_PythonInterpreter DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )