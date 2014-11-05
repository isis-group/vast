/****************************************************************
 *
 * <Copyright information>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Author: Erik Türke, tuerke@cbs.mpg.de
 *
 * pluginloader.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/

#ifndef PLUGINLOADER_HPP
#define PLUGINLOADER_HPP

#include "plugininterface.h"
#include <boost/filesystem.hpp>
#include <boost/regex.h>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <list>
#include <CoreUtils/singletons.hpp>

namespace isis
{
namespace viewer
{
namespace plugin
{

class PluginLoader
{
	friend class util::Singletons;
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