message(STATUS "Adding PythonInterpreter plugin")

###########################################################
# qt4 stuff
###########################################################
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(qpyconsole)


qt4_wrap_cpp(plugin_moc_files PythonInterpreterDialog.hpp OPTIONS -DBOOST_TT_HAS_OPERATOR_HPP_INCLUDED)
QT4_WRAP_UI(pythoninterpreter_ui_h forms/pythonInterpreter.ui)
QT4_ADD_RESOURCES(pythoninterpreter_rcc_files resources/pythoninterpreter.qrc)

###########################################################
# python stuff
###########################################################


find_package(Boost REQUIRED COMPONENTS python)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})


add_library(vastPlugin_PythonInterpreter SHARED vastPlugin_PythonInterpreter.cpp PythonInterpreterDialog.cpp PythonBridge.cpp PythonStdIORedirect.cpp
	${pythoninterpreter_ui_h} ${plugin_moc_files} ${pythoninterpreter_rcc_files})
target_link_libraries(vastPlugin_PythonInterpreter ${ISIS_LIB}  ${ISIS_LIB_DEPENDS} ${PYTHON_LIBRARIES} ${Boost_LIBRARIES} )

install(TARGETS vastPlugin_PythonInterpreter DESTINATION ${VAST_PLUGIN_INFIX} COMPONENT "vast plugins" )