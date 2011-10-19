#ifndef PLUGININTERFACE_HPP
#define PLUGININTERFACE_HPP

#include <iostream>

namespace isis {
namespace viewer {

class ViewerCoreBase;	

namespace plugin {
	
class PluginInterface
{
public:
	PluginInterface() {};
	void setViewerCore( isis::viewer::ViewerCoreBase *core) { viewerCore = core; }
	
	virtual bool call() = 0;
	virtual std::string getName() = 0;
	virtual std::string getDescription() = 0;
	
protected:
	ViewerCoreBase *viewerCore;
	
};
	
	
}}}






#endif