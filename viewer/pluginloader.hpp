#ifndef PLUGINLOADER_HPP
#define PLUGINLOADER_HPP

#include "plugininterface.h"
#include <boost/filesystem.hpp>
#include <boost/regex.h>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <list>

namespace isis
{
namespace viewer
{
namespace plugin
{

class PluginLoader
{
public:
	typedef boost::shared_ptr< PluginInterface > PluginInterfacePointer;
	typedef std::list< PluginInterfacePointer > PluginListType;
	typedef std::list<std::string> PathsType;

	void addPluginSearchPath( const std::string &path ) { m_PluginSearchPaths.push_back( path ); }
	PathsType getPluginSearchPaths() const { return m_PluginSearchPaths; }

	PluginListType getPlugins() const { return pluginList; }

	static PluginLoader &get();

protected:
	PluginLoader();

	bool registerPlugin( const PluginInterfacePointer plugin );
	unsigned int findPlugins( std::list<std::string> paths );
	PluginListType pluginList;

private:
	PathsType m_PluginSearchPaths;
};

}
}
}


#endif