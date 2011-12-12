#include "plugininterface.h"
#include "PythonInterpreterDialog.hpp"

namespace isis {
namespace viewer {
namespace plugin {
	
class PythonInterpreter : public PluginInterface
{
public:
	PythonInterpreter() : isInitialized(false) {};
	virtual std::string getName() { return std::string( "Python Interpreter" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "" ); }
	virtual QKeySequence getShortcut() { return QKeySequence( "P, I" ) ;}
	virtual bool isGUI() { return true; }
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/pythonInterpreter.png" ); }
	virtual bool call() {
		if( !isInitialized ) {
			m_Dialog = new PyhtonInterpreterDialog( parentWidget, viewerCore );
			isInitialized = true;
		}
		m_Dialog->show();
		return true;
	}

private:
	PyhtonInterpreterDialog *m_Dialog;
	bool isInitialized;

};
	
	
	
}}}



isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::PythonInterpreter();
}