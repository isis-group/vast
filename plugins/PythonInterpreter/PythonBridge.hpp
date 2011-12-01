#ifndef PYTHONBRIDGE_HPP
#define PYTHONBRIDGE_HPP


#include <boost/python.hpp>
#include <qviewercore.hpp>

using namespace boost::python;

class PythonBridge : QObject
{
	Q_OBJECT
	
public:
	PythonBridge(isis::viewer::QViewerCore *core );

	boost::scoped_ptr< object > main_namespace;
	boost::scoped_ptr< object > main_module;
	bool run( const std::string &code ) const;
private:
	void initializePython();
	isis::viewer::QViewerCore *m_ViewerCore;
	
};




#endif