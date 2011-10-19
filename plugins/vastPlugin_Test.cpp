#include "../viewer/PluginInterface.h"

namespace isis
{
namespace viewer
{
namespace plugin
{


class ProcessTest : public PluginInterface
{
	virtual std::string getName() { return std::string( "test/testProcess" ) ; }
	virtual std::string getDescription() { return std::string( "desc" ); }
	virtual std::string getTooltip() { return std::string( "tooltip" ); }

	virtual bool call() { std::cout << "calling " << getName() << std::endl; }
};

}
}
}

isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::ProcessTest();
}