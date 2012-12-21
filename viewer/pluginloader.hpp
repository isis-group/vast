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
#include <boost/foreach.hpp>
#include <list>
#include <QDir>
#include <CoreUtils/message.hpp>
#include <QPluginLoader>
#include "common.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{

template<class INTERFACE> class PluginLoader
{
	std::string mask;
public:
	typedef INTERFACE* PluginInterfacePointer;
	typedef std::list< PluginInterfacePointer > PluginListType;
	typedef std::list<QDir> PathsType;

	PluginLoader(std::string pathEnvName,std::string nameMask):mask(nameMask){
		const char *env_path = getenv( pathEnvName.c_str() );
		const char *env_home = getenv( "HOME" );
		
		if( env_path ) {
			addPluginSearchPath( QDir( env_path ) );
		}
		
		if( env_home ) {
			QDir home( env_home );;
			
			if( home.cd("vast") && home.cd("plugins") )
				addPluginSearchPath( home );
		}
	}

	void addPluginSearchPath( const QString path ) { m_PluginSearchPaths.push_back( path ); }

	PluginListType loadPlugins(){
		PluginListType ret;
		BOOST_FOREACH(PathsType::const_reference p,m_PluginSearchPaths){
			PluginListType loaded=findPlugins(p);
		}
	}
protected:
	PluginListType findPlugins( QDir path ){
		PluginListType ret;
		
		if( !path.exists() ) {
			LOG( Dev, warning ) << "Plugin path " << util::MSubject( path.canonicalPath().toStdString() ) << " not found!";
			return 0;
		}
		
		if( !path.isReadable() ) {
			LOG( Dev, warning ) << "Plugin path " << util::MSubject( path.canonicalPath().toStdString() ) << " is not readable!";
			return 0;
		}
		
		LOG( viewer::Dev, info ) << "Scanning " << path.canonicalPath().toStdString() << " for plugins... (" << mask << ")";
		
		foreach (QString fileName, path.entryList(QStringList(mask.c_str()), QDir::Files)) {
			QPluginLoader loader(path.absoluteFilePath(fileName));
			QObject *plLoader = loader.instance();
			PluginInterface *plugin;
			if(plLoader && (plugin = qobject_cast<PluginInterfacePointer>(plLoader))){
				ret.push_back(plugin);
				LOG( Dev, info ) << "Added plugin " << plugin->getName() << " from " << path.canonicalPath().toStdString();
			} else
				LOG( Dev, warning ) << "Could not load plugin " << util::MSubject( path.filePath(fileName).toStdString() ) << ":" <<  loader.errorString().toStdString();
		}
		return ret;
			
	}
	PluginListType pluginList;

private:
	PathsType m_PluginSearchPaths;
};

}
}
}


#endif