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

int main( int argc, char *argv[] )
{

	using namespace isis;
	using namespace viewer;
	qt4::QDefaultMessagePrint *viewer_handler = new qt4::QDefaultMessagePrint( info );
	qt4::QDefaultMessagePrint *isis_handler = new qt4::QDefaultMessagePrint( verbose_info );
	qt4::QDefaultMessagePrint *imageio_handler = new qt4::QDefaultMessagePrint( verbose_info );
	util::_internal::Log<viewer::Runtime>::setHandler( boost::shared_ptr<qt4::QDefaultMessagePrint>( viewer_handler ) );
	util::_internal::Log<data::Runtime>::setHandler( boost::shared_ptr<qt4::QDefaultMessagePrint>( isis_handler ) );
	util::_internal::Log<image_io::Runtime>::setHandler( boost::shared_ptr<qt4::QDefaultMessagePrint>( imageio_handler ) );
	
	std::string appName = "vast";
	std::string orgName = "cbs.mpg.de";

	util::Selection dbg_levels( "error,warning,info,verbose_info" );
	dbg_levels.set( "warning" );
#if QT_VERSION >= 0x040500 
	const char *graphics_system = getenv( "VAST_GRAPHICS_SYSTEM" );

	if( graphics_system && ( !strcmp( graphics_system, "raster" ) || !strcmp( graphics_system, "opengl" ) || !strcmp( graphics_system, "native" ) ) ) {
		QApplication::setGraphicsSystem( graphics_system );
	} else {
		QApplication::setGraphicsSystem( "raster" );
	}
#else
	std::cout << "Warning! Your Qt version is below Qt4.5. Not able to set graghics system." << std::endl;
#endif
	qt4::IOQtApplication app( appName.c_str(), false, false );
	app.parameters["in"] = util::slist();
	app.parameters["in"].needed() = false;
	app.parameters["in"].setDescription( "The input image file list." );
	app.parameters["zmap"] = util::slist();
	app.parameters["zmap"].needed() = false;
	app.parameters["zmap"].setDescription( "The input image file list is interpreted as zmaps. " );
	app.parameters["dViewer"] = dbg_levels;
	app.parameters["dViewer"].setDescription( "Debugging level for the Viewer module" );
	app.parameters["dViewer"].hidden() = true;
	app.parameters["dViewer"].needed() = false;
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

	QViewerCore *core = new QViewerCore( appName, orgName );
	
	core->addMessageHandler( viewer_handler );
	core->addMessageHandler( isis_handler );
	core->addMessageHandler( imageio_handler );
	std::cout << "v" << core->getVersion() << " ( isis core: " << app.getCoreVersion() << " )" << std::endl;
	//scan for plugins and hand them to the core
	core->addPlugins( plugin::PluginLoader::get().getPlugins() );
	core->getUICore()->reloadPluginsToGUI();
	
	app.setLog<ViewerLog>( static_cast<LogLevel>( static_cast <unsigned short>( app.parameters["dViewer"]->as<util::Selection>() ) ) );
	app.setLog<ViewerDebug>( static_cast<LogLevel>( static_cast <unsigned short>( app.parameters["dViewer"]->as<util::Selection>() ) ) );
	util::slist fileList = app.parameters["in"];
	util::slist zmapFileList = app.parameters["zmap"];
	std::list< data::Image > imgList;
	std::list< data::Image > zImgList;
	if( fileList.size() || zmapFileList.size() ) {
		if( core->getOptionMap()->getPropertyAs<bool>("showLoadingWidget") ) {
			core->getUICore()->getMainWindow()->startWidget->showMe( false );
		}
			//load the anatomical images
		BOOST_FOREACH ( util::slist::const_reference fileName, fileList ) {
			std::stringstream fileLoad;
			fileLoad << "Loading \"" << boost::filesystem::path(fileName).filename() << "\" ...";
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
			fileLoad << "Loading \"" << boost::filesystem::path(fileName).filename() << "\" ...";
			core->receiveMessage( fileLoad.str() );
			//what a nasty hack :-( but necessary, since only vista understands the onlyfirst dialect
			if( boost::filesystem::extension( boost::filesystem::path(fileName) ) == std::string(".v") ) {
				if( !dialect.size() ) {
					dialect = std::string( "onlyfirst" );
				}
			}

			std::list< data::Image > tmpList = data::IOFactory::load( fileName, app.parameters["rf"].toString(), dialect );
			BOOST_FOREACH( std::list< data::Image >::reference imageRef, tmpList ) {
				zImgList.push_back( imageRef );
				
			}
		}
	} else if( core->getOptionMap()->getPropertyAs<bool>("showStartWidget") ) {
		core->getUICore()->getMainWindow()->startWidget->showMe( true );
	}

	//*****************************************************************************************
	//distribution of images
	//*****************************************************************************************

	typedef std::list< boost::shared_ptr<ImageHolder > >::const_reference ImageListRef;

	if( app.parameters["zmap"].isSet() ) {
		core->setMode( ViewerCoreBase::zmap );
		core->getUICore()->getMainWindow()->setWindowTitle( "vast (zmap mode)" );
	}	
	//particular distribution of images in widgets
	if( app.parameters["zmap"].isSet() && zImgList.size() > 1 ) {
		core->getUICore()->setViewWidgetArrangement( UICore::InRow );
		BOOST_FOREACH( ImageListRef image, core->addImageList( zImgList, ImageHolder::z_map ) ) {
			checkForCaCp(image);
			UICore::ViewWidgetEnsembleType ensemble = core->getUICore()->createViewWidgetEnsemble( "", image );
			if( app.parameters["in"].isSet() ) {
				BOOST_FOREACH( std::list<data::Image>::const_reference image, imgList)
				{
					boost::shared_ptr<ImageHolder> anatomicalImage = core->addImage( image, ImageHolder::anatomical_image );
					checkForCaCp(anatomicalImage);
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
		BOOST_FOREACH( ImageListRef image, core->addImageList( imgList, ImageHolder::anatomical_image ) ) {
			checkForCaCp(image);
			core->getUICore()->createViewWidgetEnsemble( "", image );
		}
		core->getUICore()->setOptionPosition( isis::viewer::UICore::bottom );
		core->getUICore()->getMainWindow()->startWidget->close();
	} else if ( app.parameters["in"].isSet() || app.parameters["zmap"].isSet() ) {
		core->getUICore()->setViewWidgetArrangement( UICore::InRow );
		UICore::ViewWidgetEnsembleType ensemble = core->getUICore()->createViewWidgetEnsemble( "" );
		BOOST_FOREACH( ImageListRef image, core->addImageList( imgList, ImageHolder::anatomical_image ) ) {
			checkForCaCp(image);
			core->attachImageToWidget( image, ensemble[0]. widgetImplementation );
			core->attachImageToWidget( image, ensemble[1]. widgetImplementation );
			core->attachImageToWidget( image, ensemble[2]. widgetImplementation );
		}
		BOOST_FOREACH( ImageListRef image, core->addImageList( zImgList, ImageHolder::z_map ) ) {
			checkForCaCp(image);
			core->attachImageToWidget( image, ensemble[0]. widgetImplementation );
			core->attachImageToWidget( image, ensemble[1]. widgetImplementation );
			core->attachImageToWidget( image, ensemble[2]. widgetImplementation );
		}
		core->getUICore()->setOptionPosition( isis::viewer::UICore::bottom );
		core->getUICore()->getMainWindow()->startWidget->close();

	}

	LOG( isis::viewer::Runtime, info ) << "Welcome to vast ;-)";
	core->getUICore()->showMainWindow();

	core->settingsChanged();

	return app.getQApplication().exec();
}
