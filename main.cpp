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

	QCoreApplication::setOrganizationName( "cbs.mpg.de" );
	QSettings settings;

	qt5::IOQtApplication app( "vast", true, false );
	app.parameters["split"] = false;
	app.parameters["split"].needed() = false;
	app.parameters["split"].setDescription( "Show each image in a separate view" );
	app.parameters["widget"] = std::string();
	app.parameters["widget"].needed() = false;
	app.parameters["widget"].setDescription( "Use specific widget" );
	auto core = app.init( argc, argv, &QViewerCore::images_loaded );

	app.addLogging<viewer::Runtime>("");
	app.addLogging<viewer::Dev>("");

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

	core->getUICore()->showMainWindow( );
	return app.exec();
}
