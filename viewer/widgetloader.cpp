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

unsigned int WidgetLoader::findWidgets( QDir path )
{
	unsigned int ret = 0;

	if( !path.exists() ) {
		LOG( Dev, warning ) << "Widgetpath " << util::MSubject( path.canonicalPath().toStdString() ) << " not found!";
		return 0;
	}

	if( !path.isReadable() ) {
		LOG( Dev, warning ) << util::MSubject( path.canonicalPath().toStdString() ) << " is not readable!";
		return 0;
	}

	LOG( viewer::Dev, info ) << "Scanning " << util::MSubject( path.canonicalPath().toStdString() ) << " for widgets... (" << DL_PREFIX << "*" << DL_SUFFIX << ")";

	foreach (QString fileName, path.entryList(QStringList(QString(DL_PREFIX)+"*"+DL_SUFFIX), QDir::Files)) {
		QPluginLoader loader(path.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();
		WidgetInterface *widget;
		if(plugin && (widget = qobject_cast<WidgetInterface*>(plugin))){
			const util::PropertyMap props=widget->getProperties();

			if( props.hasProperty( "widgetIdent" ) ) {
				const std::string widgetIdent = props.getPropertyAs<std::string>( "widgetIdent" );
				widgetMap[widgetIdent] = widget;
				widgetPropertyMap[widgetIdent] = props;
// 						optionDialogMap[widgetIdent] = oLoadFunc;
				ret++;
				LOG( Dev, info ) << "Added widget " << widgetIdent;
			} else {
				LOG( Dev, error ) << "The widget " << fileName.toStdString() << " has no property \"widgetIdent\" ! Will not load it.";
			}
		} else {
			LOG( Dev, warning ) << "Could not load widget plugin " << util::MSubject( fileName.toStdString() ) << ":" <<  loader.errorString().toStdString();
		}
	}
	return ret;
}

WidgetLoader::WidgetMapType WidgetLoader::getWidgetMap()
{
	if(widgetMap.empty()){ // search for widgets if none are found yet 
		BOOST_FOREACH(PathsType::const_reference path, m_WidgetSearchPaths){
			findWidgets( path );
		}
	}
	return widgetMap;
}

WidgetLoader::WidgetLoader()
{
	const char *env_path = getenv( "VAST_WIDGET_PATH" );
	const char *env_home = getenv( "HOME" );
	
	if( env_path ) {
		addWidgetSearchPath( QString(env_path) );
	}
	
	if( env_home ) {
		const boost::filesystem::path home = boost::filesystem::path( env_home ) / "vast" / "widgets";
		
		if( boost::filesystem::exists( home ) ) {
			addWidgetSearchPath( QString::fromStdString (home.directory_string()) );
		}
	}
	
	if( std::string( _VAST_WIDGET_PATH ).size() ) {
		addWidgetSearchPath( QString(_VAST_WIDGET_PATH ) );
	}
}

WidgetLoader &WidgetLoader::get()
{
	return util::Singletons::get< WidgetLoader, INT_MAX>();
}


}
}
}

