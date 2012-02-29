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
 * Author: Erik Tuerke, tuerke@cbs.mpg.de
 *
 * widgetloader.cpp
 *
 * Description:
 *
 *  Created on: Feb 28, 2012
 *      Author: tuerke
 ******************************************************************/

#include "widgetloader.hpp"
#include <CoreUtils/message.hpp>
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
namespace widget
{
namespace _internal
{
struct widgetDeleter {
	void *m_dlHandle;
	std::string m_widgetName;
	widgetDeleter( void *dlHandle, std::string widgetName ): m_dlHandle( dlHandle ), m_widgetName( widgetName ) {}
	void operator()( WidgetInterface *format ) {
		delete format;
#ifdef WIN32

		if( !FreeLibrary( ( HINSTANCE )m_dlHandle ) )
#else
		if ( dlclose( m_dlHandle ) != 0 )
#endif
			std::cerr << "Failed to release widget " << m_widgetName << " (was loaded at " << m_dlHandle << ")";

		//we cannot use LOG here, because the loggers are gone allready
	}
};
}

unsigned int WidgetLoader::findWidgets( std::list< std::string > paths )
{
	unsigned int ret = 0;
	bool pathOk;
	BOOST_FOREACH( PathsType::const_reference pathRef, paths ) {
		pathOk = true;
		boost::filesystem::path p( pathRef );

		if( !boost::filesystem::exists( p ) ) {
			LOG( Dev, warning ) << "Widgetpath " << util::MSubject( p.native_file_string() ) << " not found!";
			pathOk = false;
		}

		if( !boost::filesystem::is_directory( p ) ) {
			LOG( Dev, warning ) << util::MSubject( p.native_file_string() ) << " is not a directory!";
			pathOk = false;
		}

		LOG( Dev, info ) << "Scanning " << util::MSubject( p ) << " for widgets...";
		LOG( Dev, info ) << "Scanning " << util::MSubject( p ) << " for widgets...";

		boost::regex widgetFilter( std::string( "^" ) + DL_PREFIX + "vastImageWidget" + "[[:word:]]+" + DL_SUFFIX + "$" );

		if( pathOk ) {
			for ( boost::filesystem::directory_iterator itr( p ); itr != boost::filesystem::directory_iterator(); ++itr ) {
				if ( boost::filesystem::is_directory( *itr ) )continue;

				if ( boost::regex_match( itr->path().leaf(), widgetFilter ) ) {
					const std::string widgetName = itr->path().file_string();
#ifdef WIN32
					HINSTANCE handle = LoadLibrary( widgetName.c_str() );
#else
					void *handle = dlopen( widgetName.c_str(), RTLD_NOW );
#endif
					if ( handle ) {
#ifdef WIN32
						const char* ( *loadIdentifier_func )() = ( const char* ( * )() )GetProcAddress( handle, "getIdentifier" );
						loadWidget_func loadFunc = ( isis::viewer::widget::WidgetInterface * ( * )() )GetProcAddress( handle, "loadWidget" );
#else
						const char* ( *loadIdentifier_func )() = ( const char* ( * )() )dlsym( handle, "getIdentifier" );
						loadWidget_func loadFunc = ( isis::viewer::widget::WidgetInterface * ( * )() )dlsym( handle, "loadWidget" );
#endif
						if ( loadFunc && loadIdentifier_func ) {

							widgetMap[loadIdentifier_func()] = loadFunc;
							ret++;
							LOG( Dev, info ) << "Added widget " << loadIdentifier_func();
						} else {
#ifdef WIN32
							LOG( Dev, warning )
									<< "could not get format factory function from " << util::MSubject( widgetName );
							FreeLibrary( handle );
#else
							LOG( Dev, warning )
									<< "could not get format factory function from " << util::MSubject( widgetName ) << ":" << util::MSubject( dlerror() );
							dlclose( handle );
#endif
						}
					} else
#ifdef WIN32
						LOG( Runtime, warning ) << "Could not load library " << util::MSubject( widgetName );

#else
						LOG( Dev, warning ) << "Could not load library " << util::MSubject( widgetName ) << ":" <<  util::MSubject( dlerror() );
#endif
				} else {
					LOG( Dev, verbose_info )
							<< "Ignoring " << util::MSubject( itr->path() )
							<< " because it doesn't match " << widgetFilter.str();
				}
			}
		}

	}
	return ret;
}

WidgetLoader::WidgetLoader()
{
	const char *env_path = getenv( "VAST_WIDGET_PATH" );
	const char *env_home = getenv( "HOME" );

	if( env_path ) {
		addWidgetSearchPath( boost::filesystem::path( env_path ).directory_string() );
	}

	if( env_home ) {
		const boost::filesystem::path home = boost::filesystem::path( env_home ) / "vast" / "widgets";

		if( boost::filesystem::exists( home ) ) {
			addWidgetSearchPath( home.directory_string() );
		}
	}

	if( std::string( _VAST_WIDGET_PATH ).size() ) {
		addWidgetSearchPath( std::string ( _VAST_WIDGET_PATH ) );
	}

	findWidgets( m_WidgetSearchPaths );

}

WidgetLoader &WidgetLoader::get()
{
	return util::Singletons::get< WidgetLoader, INT_MAX>();
}


}
}
}

