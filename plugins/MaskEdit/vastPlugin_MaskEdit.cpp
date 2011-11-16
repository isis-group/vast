#include "plugininterface.h"

namespace isis {
namespace viewer {
namespace plugin {

	
class MaskEdit : public PluginInterface
{
public:
	virtual std::string getName() { return std::string( "MaskEdit" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "" ); }
	virtual QKeySequence getShortcut() { return QKeySequence( "M, E" ) ;}
	virtual bool isGUI() { return true; }
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/maskEdit.png" ); }
	
	virtual bool call() {};
	
	virtual ~MaskEdit() {};
};
	
	
}}}


isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::MaskEdit();
}