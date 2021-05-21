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
#include <csignal>

#include <isis/adapter/qt5/qtapplication.hpp>
#include <isis/adapter/qt5/qdefaultmessageprint.hpp>
#include <isis/core/io_factory.hpp>

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
	QSettings settings;

	qt5::IOQtApplication app( appName.c_str(), false, false );
	app.parameters["in"] = util::slist();
	app.parameters["in"].needed() = false;
	app.parameters["in"].setDescription( "The input image file list." );
//	app.parameters["zmap"] = util::slist();
//	app.parameters["zmap"].needed() = false;
//	app.parameters["zmap"].setDescription( "The input image file list is interpreted as statistical maps. " );
	//alias to zmap
//	app.parameters["stats"] = util::slist();
//	app.parameters["stats"].needed() = false;
//	app.parameters["stats"].setDescription( "The input image file list is interpreted as statistical maps. " );
	app.parameters["split"] = false;
	app.parameters["split"].needed() = false;
	app.parameters["split"].setDescription( "Show each image in a separate view" );
	app.parameters["widget"] = std::string();
	app.parameters["widget"].needed() = false;
	app.parameters["widget"].setDescription( "Use specific widget" );
	app.init( argc, argv, false );
	auto core = std::make_unique<QViewerCore>();

	app.addLogging<viewer::Runtime>("");
	app.addLogging<viewer::Dev>("");

	//setting stylesheet
	if ( settings.value( "useStyleSheet" ).toBool() ) {
		app.getQApplication().setStyleSheet( util::Singletons::get<style::Style, 10>().getStyleSheet( settings.value("styleSheet").toString().toStdString() ) );
	}

	//@todo implement me
//	core->addMessageHandler( logging_hanlder_runtime.get() );
//	core->addMessageHandlerDev( logging_hanlder_dev.get() );
	//scan for plugins and hand them to the core
	core->addPlugins( plugin::PluginLoader::get().getPlugins() );
	core->getUICore()->reloadPluginsToGUI();

	std::string widget_name = app.parameters["widget"];


	if( widget_name.empty() ) {
		if( settings.value( "showImagesGeometricalView" ).toBool() && core->hasWidget( settings.value( "widgetGeometrical" ).toString().toStdString() ) ) {
			widget_name = settings.value("widgetGeometrical").toString().toStdString();
		} else if ( !settings.value( "showImagesGeometricalView" ).toBool() && core->hasWidget( settings.value( "widgetLatched" ).toString().toStdString() ) ) {
			widget_name = settings.value( "widgetLatched" ).toString().toStdString();
		} else {
			widget_name = settings.value( "defaultViewWidgetIdentifier" ).toString().toStdString();
		}
	}

	if( !core->hasWidget( widget_name ) ) {
		std::cerr << "Error loading widget!" << std::endl;
	}

	util::slist fileList = app.parameters["in"];
//	const bool zmapIsSet = app.parameters["zmap"].isParsed() || app.parameters["stats"].isParsed();
//	util::slist zmapFileList = app.parameters["zmap"];
//
//	if( !zmapFileList.size() ) {
//		zmapFileList = app.parameters["stats"];
//	}
//
//	if( zmapIsSet ) {
//		core->setMode( ViewerCoreBase::statistical_mode );
//	}  else {
//		core->setMode( ViewerCoreBase::default_mode );
//	}

	std::list<FileInformation> fileInfoList;

	for( util::slist::const_reference file: fileList ) {
		fileInfoList.push_back( FileInformation( file,
								app.parameters["rdialect"].as<std::string>().c_str(),
								app.parameters["rf"].as<std::string>().c_str(),
								widget_name,
								ImageHolder::structural_image,
								app.parameters["split"].as<bool>() ) );
	}
//	for( util::slist::const_reference file: zmapFileList ) {
//		fileInfoList.push_back( FileInformation( file,
//								app.parameters["rdialect"].as<std::string>().c_str(),
//								app.parameters["rf"].as<std::string>().c_str(),
//								widget_name,
//								ImageHolder::statistical_image,
//								app.parameters["split"].as<bool>() ) );
//	}

	core->getUICore()->showMainWindow( fileInfoList );
	return app.getQApplication().exec();
}
