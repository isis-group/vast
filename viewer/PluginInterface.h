#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#ifdef __cplusplus
#include <iostream>
#include <QObject>

namespace isis {
namespace viewer {

class QViewerCore;	

namespace plugin {

	
/// Base class for all plugins that are aiming to do any operation on the images
class PluginInterface
{
public:
	PluginInterface() {};
	void setViewerCore( isis::viewer::QViewerCore *core) { viewerCore = core; }
	void setParentWidget( QWidget *p ) { parent = p; }
	
	/**
	 * Calls the plugin. This function should execute the plugin 
	 * (e.g. showing a dialog or directly executing an operation)
	 */
	virtual bool call() = 0;
	
	
	/**
	 * This function returns the name of the plugin.
	 * A name for instance can look like "category/subcategory/pluginname
	 * and automaticall will be in the respective submenu (plugins/category/subcategory).
	 */
	virtual std::string getName() = 0;
	
	///returns a description of the plugin
	virtual std::string getDescription() = 0;
	
	///returns the tooltip of the plugin
	virtual std::string getTooltip() = 0;
	
	///returns if the plugin uses a gui
	virtual bool isGUI() = 0;
	
	virtual ~PluginInterface() {}
	
	std::string plugin_file;
protected:
	QViewerCore *viewerCore;
	QWidget *parent;
	
	
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