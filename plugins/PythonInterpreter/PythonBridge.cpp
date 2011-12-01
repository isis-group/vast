#include "PythonBridge.hpp"


PythonBridge::PythonBridge(isis::viewer::QViewerCore *core )
	: m_ViewerCore(core)
{

	initializePython();
	(*main_namespace)["Core"] = class_<isis::viewer::ViewerCoreBase>("Core")
							.def("getVersion", &isis::viewer::ViewerCoreBase::getVersion )
							.staticmethod("getVersion")
							.def("getCurrentImage", &isis::viewer::ViewerCoreBase::getCurrentImageAsISIS )
							;
						
	
}

void PythonBridge::initializePython()
{
	Py_Initialize();
	main_module.reset( new object(  handle<> (borrowed(PyImport_AddModule("__main__")))));
	main_namespace.reset( new object ( main_module->attr("__dict__") ) );
	handle<> ignored(( PyRun_String( "from isis import core; from isis import data", Py_file_input, main_namespace->ptr(), main_namespace->ptr() ) ));
}



bool PythonBridge::run(const std::string& code) const
{
	(*main_namespace)["ci"] = &m_ViewerCore->getCurrentImageAsISIS();
	try {
		handle<> ignored(( PyRun_String( code.c_str(), Py_file_input, main_namespace->ptr(), main_namespace->ptr() ) ));
	} catch( error_already_set ) {
		PyErr_Print();
	}
}
