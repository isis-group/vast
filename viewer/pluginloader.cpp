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
 * pluginloader.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include <CoreUtils/message.hpp>
#include "pluginloader.hpp"
#include "common.hpp"
#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace isis
{
namespace viewer
{
namespace plugin
{
namespace _internal
{
struct pluginDeleter {
	void *m_dlHandle;
	std::string m_pluginName;
	pluginDeleter( void *dlHandle, std::string pluginName ): m_dlHandle( dlHandle ), m_pluginName( pluginName ) {}
	void operator()( PluginInterface *format ) {
		delete format;
#ifdef WIN32

		if( !FreeLibrary( ( HINSTANCE )m_dlHandle ) )
#else
		if ( dlclose( m_dlHandle ) != 0 )
#endif
			std::cerr << "Failed to release plugin " << m_pluginName << " (was loaded at " << m_dlHandle << ")";

		//we cannot use LOG here, because the loggers are gone allready
	}
};
}
bool PluginLoader::registerPlugin ( const PluginInterfacePointer plugin )
{
	if ( !plugin )return false;

	pluginList.push_back( plugin );
	return true;
}

unsigned int PluginLoader::findPlugins( std::list< std::string > paths )
{
	unsigned int ret = 0;
	bool pathOk;
	BOOST_FOREACH( PathsType::const_reference pathRef, paths ) {
		pathOk = true;
		boost::filesystem::path p( pathRef );

		if( !boost::filesystem::exists( p ) ) {
			LOG( Runtime, warning ) << "Pluginpath " << util::MSubject( p ) << " not found!";
			pathOk = false;
		}

		if( !boost::filesystem::is_directory( p ) ) {
			LOG( Runtime, warning ) << util::MSubject( p ) << " is not a directory!";
			pathOk = false;
		}

		LOG( Runtime, info ) << "Scanning " << util::MSubject( p ) << " for plugins...";

		boost::regex pluginFilter( std::string( "^" ) + DL_PREFIX + "vastPlugin" + "[[:word:]]+" + DL_SUFFIX + "$" );

		if( pathOk ) {
			for ( boost::filesystem::directory_iterator itr( p ); itr != boost::filesystem::directory_iterator(); ++itr ) {
				if ( boost::filesystem::is_directory( *itr ) )continue;

				if ( boost::regex_match( itr->path().filename().string(), pluginFilter ) ) {
					const std::string pluginName = itr->path().native();
#ifdef WIN32
					HINSTANCE handle = LoadLibrary( pluginName.c_str() );
#else
					void *handle = dlopen( pluginName.c_str(), RTLD_NOW );
#endif

					if ( handle ) {
#ifdef WIN32
						isis::viewer::plugin::PluginInterface* ( *loadPlugin_func )() = ( isis::viewer::plugin::PluginInterface * ( * )() )GetProcAddress( handle, "loadPlugin" );
#else
						isis::viewer::plugin::PluginInterface* ( *loadPlugin_func )() = ( isis::viewer::plugin::PluginInterface * ( * )() )dlsym( handle, "loadPlugin" );
#endif

						if ( loadPlugin_func ) {
							PluginInterfacePointer plugin_class( loadPlugin_func(), _internal::pluginDeleter( handle, pluginName ) );

							if ( registerPlugin( plugin_class ) ) {
								plugin_class->plugin_file = pluginName;
								ret++;
							} else {
								LOG( Runtime, warning ) << "failed to register plugin " << util::MSubject( pluginName );
							}
						} else {
#ifdef WIN32
							LOG( Runtime, warning )
									<< "could not get format factory function from " << util::MSubject( pluginName );
							FreeLibrary( handle );
#else
							LOG( Runtime, warning )
									<< "could not get format factory function from " << util::MSubject( pluginName ) << ":" << util::MSubject( dlerror() );
							dlclose( handle );
#endif
						}
					} else
#ifdef WIN32
						LOG( Runtime, warning ) << "Could not load library " << util::MSubject( pluginName );

#else
						LOG( Runtime, warning ) << "Could not load library " << util::MSubject( pluginName ) << ":" <<  util::MSubject( dlerror() );
#endif
				} else {
					LOG( Runtime, verbose_info )
							<< "Ignoring " << util::MSubject( itr->path() )
							<< " because it doesn't match " << pluginFilter.str();
				}
			}
		}

	}
	return ret;
}

PluginLoader::PluginLoader()
{
	const char *env_path = getenv( "VAST_PLUGIN_PATH" );
	const char *env_home = getenv( "HOME" );

	if( env_path ) {
		addPluginSearchPath( env_path );
	}

	if( env_home ) {
		const boost::filesystem::path home = boost::filesystem::path( env_home ) / "vast" / "plugins";

		if( boost::filesystem::exists( home ) ) {
			addPluginSearchPath( home.native() );
		}
	}

	if( std::string( _VAST_PLUGIN_PATH ).size() ) {
		addPluginSearchPath( std::string ( _VAST_PLUGIN_PATH ) );
	}

	findPlugins( m_PluginSearchPaths );

}

PluginLoader &PluginLoader::get()
{
	return util::Singletons::get< PluginLoader, INT_MAX>();
}


}
}
}

