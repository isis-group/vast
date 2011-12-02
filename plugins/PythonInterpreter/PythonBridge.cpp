#include "PythonBridge.hpp"


PythonBridge::PythonBridge(isis::viewer::QViewerCore *core )
	: m_ViewerCore(core)
{
	initializePython();
	
	exposeViewerCore();
	exposeImageHolder();
	exposeEnums();	
}

void PythonBridge::exposeEnums()
{	try {
		(*main_namespace)["image_type"] = enum_<isis::viewer::ImageHolder::ImageType>( "image_type" )
			.value( "ZMAP", isis::viewer::ImageHolder::z_map )
			.value( "ANATOMICAL", isis::viewer::ImageHolder::anatomical_image );
	}catch ( error_already_set ) {
		PyErr_Print();
	}
}


void PythonBridge::exposeViewerCore()
{
	(*main_namespace)["Core"] = class_<isis::viewer::QViewerCore, boost::noncopyable >("Core")
							.def("getVersion", &isis::viewer::QViewerCore::getVersion )
							.def("addImage", &isis::viewer::ViewerCoreBase::addImage )
							.def("updateScene", &isis::viewer::QViewerCore::updateScene )
							;
}

void PythonBridge::exposeImageHolder()
{
	(*main_namespace)["ImageHolder"] = class_< isis::viewer::ImageHolder > ("ImageHolder");
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
