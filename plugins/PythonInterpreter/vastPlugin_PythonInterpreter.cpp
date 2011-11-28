#include "plugininterface.h"


namespace isis {
namespace viewer {
namespace plugin {
	
class PythonInterpreter : public PluginInterface
{
public:
	virtual std::string getName() { return std::string( "Python Interpreter" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "" ); }
	virtual QKeySequence getShortcut() { return QKeySequence( "P, I" ) ;}
	virtual bool isGUI() { return true; }
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/pythoninterpreter.png" ); }
	virtual bool call() {}
};
	
	
	
}}}



isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::PythonInterpreter();
}