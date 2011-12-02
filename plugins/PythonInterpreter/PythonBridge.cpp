#include "PythonBridge.hpp"


PythonBridge::PythonBridge(isis::viewer::QViewerCore *core )
	: m_ViewerCore(core)
{

	initializePython();
	(*main_namespace)["Core"] = class_<isis::viewer::ViewerCoreBase>("Core")
							.def("getVersion", &isis::viewer::ViewerCoreBase::getVersion )
							;
	(*main_namespace)["core"] = static_cast<isis::viewer::ViewerCoreBase*>( m_ViewerCore);
						
	
}

void PythonBridge::initializePython()
{
	Py_Initialize();
	main_module.reset( new object(  handle<> (borrowed(PyImport_AddModule("__main__")))));
	main_namespace.reset( new object ( main_module->attr("__dict__") ) );
	handle<> ignored(( PyRun_String( "from isis import core; from isis import data", Py_file_input, main_namespace->ptr(), main_namespace->ptr() ) ));
	
	//redirect stdio
	(*main_namespace)["PythonStdIoRedirect"] = class_<PythonStdIoRedirect>("PythonStdIoRedirect", init<>())
												.def("write", &PythonStdIoRedirect::Write);
	
	boost::python::import("sys").attr("stderr") = python_stdio_redirector;
	boost::python::import("sys").attr("stdout") = python_stdio_redirector; 
}



std::string PythonBridge::run(const std::string& code) const
{
	if( m_ViewerCore->hasImage() ) {
		(*main_namespace)["ci"] = m_ViewerCore->getCurrentImage()->getISISImage().get();
	}
	try {
		handle<> ignored(( PyRun_String( code.c_str(), Py_file_input, main_namespace->ptr(), main_namespace->ptr() ) ));
	} catch( error_already_set ) {
		PyErr_Print();
	}
	return python_stdio_redirector.GetOutput(); 	
}
