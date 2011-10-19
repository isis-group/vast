#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#ifdef __cplusplus
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
	
	virtual ~PluginInterface() {}
	
	std::string plugin_file;
protected:
	ViewerCoreBase *viewerCore;
	
};

	
}}}

#else
typedef struct PluginInterface PluginInterface;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__STDC__) || defined(__cplusplus)
#ifdef WIN32
	extern __declspec( dllexport ) isis::viewer::plugin::PluginInterface *loadPlugin();
#else
	extern isis::viewer::plugin::PluginInterface *loadPlugin();
#endif
#else
#ifdef WIN32
	extern __declspec( dllexport ) PluginInterface *loadPlugin();
#else
	extern PluginInterface *loadPlugin();
#endif
#endif

#ifdef __cplusplus
}
#endif



#endif