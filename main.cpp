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
#include "qviewercore.hpp"
#include <iostream>


#include <DataStorage/io_factory.hpp>
#include "CoreUtils/singletons.hpp"
#include <DataStorage/image.hpp>
#include <CoreUtils/log.hpp>
#include "pluginloader.hpp"
#include "color.hpp"
#include "uicore.hpp"

#include "common.hpp"
#include "internal/error.hpp"

int main( int argc, char *argv[] )
{

	using namespace isis;
	using namespace viewer;
    signal( SIGSEGV, error::sigsegv);
	
	boost::shared_ptr<qt4::QDefaultMessagePrint> logging_hanlder_runtime ( new qt4::QDefaultMessagePrint( verbose_info ) );
	boost::shared_ptr<qt4::QDefaultMessagePrint> logging_hanlder_dev ( new qt4::QDefaultMessagePrint( verbose_info ) );
	util::_internal::Log<viewer::Dev>::setHandler( logging_hanlder_dev );
	util::_internal::Log<viewer::Runtime>::setHandler( logging_hanlder_runtime );

	std::string appName = "vast";
	std::string orgName = "cbs.mpg.de";

#if QT_VERSION >= 0x040500
	
	const char *graphics_system = getenv( "VAST_GRAPHICS_SYSTEM" );
    LOG(Dev, info) << "QT_VERSION >= 0x040500";
	
	if( graphics_system && ( !strcmp( graphics_system, "raster" ) || !strcmp( graphics_system, "opengl" ) || !strcmp( graphics_system, "native" ) ) ) {
		QApplication::setGraphicsSystem( graphics_system );
		LOG(Dev, info) << "Using graphics_system=\"" << std::string( graphics_system ) << "\"";
	} else {
		QApplication::setGraphicsSystem( "raster" );
		LOG(Dev, info) << "Using graphics_system=\"raster\"";
	}

#else
	std::cout << "Warning! Your Qt version is below Qt4.5. Not able to set graghics system." << std::endl;
	LOG(Dev, warning) << "QT_VERSION < 0x040500";
#endif

	qt4::IOQtApplication app( appName.c_str(), false, false );
	app.parameters["in"] = util::slist();
	app.parameters["in"].needed() = false;
	app.parameters["in"].setDescription( "The input image file list." );
	app.parameters["zmap"] = util::slist();
	app.parameters["zmap"].needed() = false;
	app.parameters["zmap"].setDescription( "The input image file list is interpreted as zmaps. " );
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
	boost::shared_ptr< util::ProgressFeedback > feedback = boost::shared_ptr<util::ProgressFeedback>( new util::ConsoleFeedback );
	data::IOFactory::setProgressFeedback( feedback );
	app.init( argc, argv, true );

	util::_internal::Log<isis::data::Runtime>::setHandler( logging_hanlder_runtime );
	util::_internal::Log<isis::util::Runtime>::setHandler( logging_hanlder_runtime );
	util::_internal::Log<isis::util::Debug>::setHandler( logging_hanlder_runtime );
	util::_internal::Log<isis::data::Debug>::setHandler( logging_hanlder_runtime );
	util::_internal::Log<isis::image_io::Runtime>::setHandler( logging_hanlder_runtime );
	util::_internal::Log<isis::image_io::Debug>::setHandler( logging_hanlder_runtime );

	QViewerCore *core = new QViewerCore( appName, orgName );

	core->addMessageHandler( logging_hanlder_runtime.get() );
	core->addMessageHandlerDev( logging_hanlder_dev.get() );
	//scan for plugins and hand them to the core
	core->addPlugins( plugin::PluginLoader::get().getPlugins() );
	core->getUICore()->reloadPluginsToGUI();

	const util::slist fileList = app.parameters["in"];
	const util::slist zmapFileList = app.parameters["zmap"];
	std::list< data::Image > imgList;
	std::list< data::Image > zImgList;

	if( fileList.size() || zmapFileList.size() ) {
		core->getUICore()->getMainWindow()->setCursor( Qt::WaitCursor );
		QApplication::processEvents();
		//load the anatomical images
		BOOST_FOREACH ( util::slist::const_reference fileName, fileList ) {
			std::stringstream fileLoad;
			fileLoad << "Loading \"" << boost::filesystem::path( fileName ).leaf() << "\" ...";
			core->receiveMessage( fileLoad.str() );
			std::list< data::Image > tmpList = data::IOFactory::load( fileName, app.parameters["rf"].toString(), app.parameters["rdialect"].toString() );
			BOOST_FOREACH( std::list< data::Image >::reference imageRef, tmpList ) {
				imgList.push_back( imageRef );

			}
		}
		//load the zmap images
		BOOST_FOREACH ( util::slist::const_reference fileName, zmapFileList ) {
			std::string dialect = app.parameters["rdialect"].toString();
			std::stringstream fileLoad;
			fileLoad << "Loading \"" << boost::filesystem::path( fileName ).leaf() << "\" ...";
			core->receiveMessage( fileLoad.str() );

			std::list< data::Image > tmpList = data::IOFactory::load( fileName, app.parameters["rf"].toString(), dialect );
			BOOST_FOREACH( std::list< data::Image >::reference imageRef, tmpList ) {
				zImgList.push_back( imageRef );

			}
		}
	} else if( core->getOptionMap()->getPropertyAs<bool>( "showStartWidget" ) ) {
		core->getUICore()->getMainWindow()->startWidget->show();
	}

	//*****************************************************************************************
	//distribution of images
	//*****************************************************************************************

	typedef std::list< boost::shared_ptr<ImageHolder > >::const_reference ImageListRef;

	if( app.parameters["zmap"].isSet() ) {
		core->setMode( ViewerCoreBase::zmap );
	}  else {
		core->setMode( ViewerCoreBase::standard );
	}

	//particular distribution of images in widgets
	if( app.parameters["zmap"].isSet() && zImgList.size() > 1 ) {
		core->getUICore()->setViewWidgetArrangement( UICore::InRow );
		BOOST_FOREACH( ImageListRef image, core->addImageList( zImgList, ImageHolder::z_map ) ) {
			checkForCaCp( image );
			UICore::ViewWidgetEnsembleType ensemble = core->getUICore()->createViewWidgetEnsemble( "", image );

			if( app.parameters["in"].isSet() ) {
				BOOST_FOREACH( std::list<data::Image>::const_reference image, imgList ) {
					boost::shared_ptr<ImageHolder> anatomicalImage = core->addImage( image, ImageHolder::structural_image );
					checkForCaCp( anatomicalImage );

					if( anatomicalImage->getImageSize()[3] == 1 ) {
						ensemble[0].widgetImplementation->addImage( anatomicalImage );
						ensemble[1].widgetImplementation->addImage( anatomicalImage );
						ensemble[2].widgetImplementation->addImage( anatomicalImage );
					}
				}
			}
		}
		core->getUICore()->setOptionPosition( isis::viewer::UICore::bottom );
		core->getUICore()->getMainWindow()->startWidget->close();
		//only anatomical images with split option was specified
	} else if ( app.parameters["in"].isSet() && app.parameters["split"].isSet() ) {
		core->getUICore()->setViewWidgetArrangement( UICore::InRow );
		BOOST_FOREACH( ImageListRef image, core->addImageList( imgList, ImageHolder::structural_image ) ) {
			checkForCaCp( image );
			core->getUICore()->createViewWidgetEnsemble( "", image );
		}
		core->getUICore()->setOptionPosition( isis::viewer::UICore::bottom );
		core->getUICore()->getMainWindow()->startWidget->close();
	} else if ( app.parameters["in"].isSet() || app.parameters["zmap"].isSet() ) {
		core->getUICore()->setViewWidgetArrangement( UICore::InRow );
		UICore::ViewWidgetEnsembleType ensemble = core->getUICore()->createViewWidgetEnsemble( "" );
		BOOST_FOREACH( ImageListRef image, core->addImageList( imgList, ImageHolder::structural_image ) ) {
			checkForCaCp( image );
			core->attachImageToWidget( image, ensemble[0]. widgetImplementation );
			core->attachImageToWidget( image, ensemble[1]. widgetImplementation );
			core->attachImageToWidget( image, ensemble[2]. widgetImplementation );
		}
		BOOST_FOREACH( ImageListRef image, core->addImageList( zImgList, ImageHolder::z_map ) ) {
			checkForCaCp( image );
			core->attachImageToWidget( image, ensemble[0]. widgetImplementation );
			core->attachImageToWidget( image, ensemble[1]. widgetImplementation );
			core->attachImageToWidget( image, ensemble[2]. widgetImplementation );
		}
		core->getUICore()->setOptionPosition( isis::viewer::UICore::bottom );
		core->getUICore()->getMainWindow()->startWidget->close();

	}
	
	core->getUICore()->showMainWindow();

	core->settingsChanged();
	
	return app.getQApplication().exec();
}
