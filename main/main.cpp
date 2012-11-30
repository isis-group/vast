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
 * main.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include <iostream>
#include <signal.h>

#include <Adapter/qtapplication.hpp>
#include <Adapter/qdefaultmessageprint.hpp>
#include <DataStorage/io_factory.hpp>
#include <CoreUtils/log.hpp>
#include "CoreUtils/singletons.hpp"
#include <DataStorage/image.hpp>

#include "viewer/pluginloader.hpp"
#include "viewer/qviewercore.hpp"
#include "viewer/color.hpp"
#include "viewer/uicore.hpp"
#include "viewer/widgetensemble.hpp"
#include "viewer/fileinformation.hpp"
#include "viewer/common.hpp"
#include "viewer/error.hpp"
#include "widgets/mainwindow.hpp"
#include "viewer/style.hpp"


int main( int argc, char *argv[] )
{

	using namespace isis;
	using namespace viewer;
	signal( SIGSEGV, error::sigsegv );



	std::string appName = "vast";
	std::string orgName = "cbs.mpg.de";
	QCoreApplication::setApplicationName( appName.c_str() );
	QCoreApplication::setOrganizationName( orgName.c_str() );
	//setting up vast graphics_system
#if QT_VERSION >= 0x040500
	const char *graphics_system = getenv( "VAST_GRAPHICS_SYSTEM" );
	LOG( Dev, info ) << "QT_VERSION >= 0x040500";

	if( graphics_system && ( !strcmp( graphics_system, "raster" ) || !strcmp( graphics_system, "opengl" ) || !strcmp( graphics_system, "native" ) ) ) {
		QApplication::setGraphicsSystem( graphics_system );
		LOG( Dev, info ) << "Using graphics_system=\"" << std::string( graphics_system ) << "\"";
	} else {
		QApplication::setGraphicsSystem( "raster" );
		LOG( Dev, info ) << "Using graphics_system=\"raster\"";
	}

#else
	std::cout << "Warning! Your Qt version is below Qt4.5. Not able to set graghics system." << std::endl;
	LOG( Dev, warning ) << "QT_VERSION < 0x040500";
#endif

	qt4::IOQtApplication app( appName.c_str(), false, false );
	app.parameters["in"] = util::slist();
	app.parameters["in"].needed() = false;
	app.parameters["in"].setDescription( "The input image file list." );
	app.parameters["zmap"] = util::slist();
	app.parameters["zmap"].needed() = false;
	app.parameters["zmap"].setDescription( "The input image file list is interpreted as statistical maps. " );
	//alias to zmap
	app.parameters["stats"] = util::slist();
	app.parameters["stats"].needed() = false;
	app.parameters["stats"].setDescription( "The input image file list is interpreted as statistical maps. " );
	app.parameters["rf"] = std::string();
	app.parameters["rf"].needed() = false;
	app.parameters["rf"].setDescription( "Override automatic detection of file suffix for reading with given value" );
	app.parameters["rf"].hidden() = true;
	app.parameters["rdialect"] = std::string();
	app.parameters["rdialect"].needed() = false;
	app.parameters["rdialect"].hidden() = true;
	app.parameters["rdialect"].setDescription( "Dialect for reading" );
	app.parameters["split"] = false;
	app.parameters["split"].needed() = false;
	app.parameters["split"].setDescription( "Show each image in a separate view" );
	app.parameters["widget"] = std::string();
	app.parameters["widget"].needed() = false;
	app.parameters["widget"].setDescription( "Use specific widget" );
	app.init( argc, argv, false );
	QViewerCore *core = new QViewerCore;
	boost::shared_ptr<qt4::QDefaultMessagePrint> logging_hanlder_runtime ( new qt4::QDefaultMessagePrint( verbose_info ) );
	boost::shared_ptr<qt4::QDefaultMessagePrint> logging_hanlder_dev ( new qt4::QDefaultMessagePrint( verbose_info ) );
	util::_internal::Log<viewer::Dev>::setHandler( logging_hanlder_dev );
	util::_internal::Log<viewer::Runtime>::setHandler( logging_hanlder_runtime );

	//make vast showing qmessage if an error log is thrown
	logging_hanlder_dev->qmessageBelow( isis::warning );
	logging_hanlder_runtime->qmessageBelow( isis::warning );

	//setting stylesheet
	if ( core->getSettings()->getPropertyAs<bool>( "useStyleSheet" ) ) {
		app.getQApplication().setStyleSheet( util::Singletons::get<style::Style, 10>().getStyleSheet( core->getSettings()->getPropertyAs<std::string>( "styleSheet" ) ) );
	}

	util::_internal::Log<isis::data::Runtime>::setHandler( logging_hanlder_runtime );
	util::_internal::Log<isis::util::Runtime>::setHandler( logging_hanlder_runtime );
	util::_internal::Log<isis::util::Debug>::setHandler( logging_hanlder_runtime );
	util::_internal::Log<isis::data::Debug>::setHandler( logging_hanlder_runtime );
	util::_internal::Log<isis::image_io::Runtime>::setHandler( logging_hanlder_runtime );
	util::_internal::Log<isis::image_io::Debug>::setHandler( logging_hanlder_runtime );


	core->addMessageHandler( logging_hanlder_runtime.get() );
	core->addMessageHandlerDev( logging_hanlder_dev.get() );
	//scan for plugins and hand them to the core
	core->addPlugins( plugin::PluginLoader::get().getPlugins() );
	core->getUICore()->reloadPluginsToGUI();

	std::string widget_name = app.parameters["widget"];

	if( widget_name.empty() ) {
		if( core->getSettings()->getPropertyAs<bool>( "showImagesGeometricalView" ) && core->hasWidget( core->getSettings()->getPropertyAs<std::string>( "widgetGeometrical" ) ) ) {
			widget_name = core->getSettings()->getPropertyAs<std::string>( "widgetGeometrical" );
		} else if ( !core->getSettings()->getPropertyAs<bool>( "showImagesGeometricalView" ) && core->hasWidget( core->getSettings()->getPropertyAs<std::string>( "widgetLatched" ) ) ) {
			widget_name = core->getSettings()->getPropertyAs<std::string>( "widgetLatched" );
		} else {
			widget_name = core->getSettings()->getPropertyAs<std::string>( "defaultViewWidgetIdentifier" );
		}
	}

	if( !core->hasWidget( widget_name ) ) {
		std::cerr << "Error loading widget!" << std::endl;
	}

	util::slist fileList = app.parameters["in"];
	const bool zmapIsSet = app.parameters["zmap"].isSet() || app.parameters["stats"].isSet();
	util::slist zmapFileList = app.parameters["zmap"];

	if( !zmapFileList.size() ) {
		zmapFileList = app.parameters["stats"];
	}

	if( zmapIsSet ) {
		core->setMode( ViewerCoreBase::statistical_mode );
	}  else {
		core->setMode( ViewerCoreBase::default_mode );
	}

	std::list<FileInformation> fileInfoList;

	BOOST_FOREACH( util::slist::const_reference file, fileList ) {
		fileInfoList.push_back( FileInformation( file,
								app.parameters["rdialect"].as<std::string>().c_str(),
								app.parameters["rf"].as<std::string>().c_str(),
								widget_name,
								ImageHolder::structural_image,
								app.parameters["split"].as<bool>() ) );
	}
	BOOST_FOREACH( util::slist::const_reference file, zmapFileList ) {
		fileInfoList.push_back( FileInformation( file,
								app.parameters["rdialect"].as<std::string>().c_str(),
								app.parameters["rf"].as<std::string>().c_str(),
								widget_name,
								ImageHolder::statistical_image,
								app.parameters["split"].as<bool>() ) );
	}

	core->getUICore()->showMainWindow( fileInfoList );
	return app.getQApplication().exec();
}
