#ifndef PYTHONBRIDGE_HPP
#define PYTHONBRIDGE_HPP


#include <boost/python.hpp>
#include <qviewercore.hpp>
#include "PythonStdIORedirect.hpp"

using namespace boost::python;

class PythonBridge
{
public:
	PythonBridge( isis::viewer::QViewerCore *core );

	virtual ~PythonBridge() { Py_Finalize(); }

	boost::scoped_ptr< object > main_namespace;
	boost::scoped_ptr< object > main_module;
	std::string run( const std::string &code ) const;
private:
	void initializePython();
	isis::viewer::QViewerCore *m_ViewerCore;

	void exposeEnums();
	void exposeViewerCore();
	void exposeImageHolder();

	PythonStdIoRedirect python_stdio_redirector;

};




#endif