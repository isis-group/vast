#include "../viewer/PluginInterface.hpp"

namespace isis {
namespace viewer {
namespace plugin {
	
	
class ProcessTest : public PluginInterface
{
	virtual std::string getName() { return std::string("gna/test/testProcess") ; }
	virtual std::string getDescription() { return std::string("desc"); }
	
	virtual bool call() { std::cout << "calling " << getName() << std::endl; }
};
	
}}}