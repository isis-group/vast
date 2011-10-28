#include <Adapter/qtapplication.hpp>
#include "mainWindowUIInterface.hpp"
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
	
	util::DefaultMsgPrint::stopBelow( warning );
	ENABLE_LOG( data::Runtime, util::DefaultMsgPrint, error );
	std::string appName = "vast";
	std::string orgName = "cbs.mpg.de";
	std::map<std::string, WidgetType> wTypeMap;



	util::Selection dbg_levels( "error,warning,info,verbose_info" );
	util::Selection wTypes( "gl,qt" );
	wTypeMap.insert( std::make_pair<std::string, WidgetType>( "gl", type_gl ) );
	wTypeMap.insert( std::make_pair<std::string, WidgetType>( "qt", type_qt ) );
	wTypes.set( "qt" );
	dbg_levels.set( "warning" );
	qt4::IOQtApplication app( appName.c_str(), false, false, std::string( "raster" ) );


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
	app.parameters["old_lipsia"] = false;
	app.parameters["old_lipsia"].needed() = false;
	app.parameters["old_lipsia"].setDescription( "Ignore orientation information and treat files as old lipsia files." );
	app.parameters["wtype"] = wTypes;
	app.parameters["wtype"].needed() = false;
	app.parameters["wtype"].hidden() = true;
	app.parameters["wtype"].setDescription( "Sets the type of the widgets" );
	boost::shared_ptr< util::ProgressFeedback > feedback = boost::shared_ptr<util::ProgressFeedback>( new util::ConsoleFeedback );
	data::IOFactory::setProgressFeedback( feedback );
	app.init( argc, argv, true );
	
	
	QViewerCore *core = new QViewerCore( appName, orgName );
	std::cout << "v" << core->getVersion() << " ( isis core: " << app.getCoreVersion() << " )" << std::endl;
	//scan for plugins and hand them to the core
	core->addPlugins( plugin::PluginLoader::get().getPlugins() );
	core->getUI()->reloadPluginsToGUI();

	app.setLog<ViewerLog>( app.getLLMap()[app.parameters["dViewer"]->as<util::Selection>()] );
	app.setLog<ViewerDebug>( app.getLLMap()[app.parameters["dViewer"]->as<util::Selection>()] );
	util::slist fileList = app.parameters["in"];
	util::slist zmapFileList = app.parameters["zmap"];
	std::list< data::Image > imgList;
	std::list< data::Image > zImgList;

	//load the anatomical images
	BOOST_FOREACH ( util::slist::const_reference fileName, fileList ) {
		std::list< data::Image > tmpList = data::IOFactory::load( fileName, app.parameters["rf"].toString(), app.parameters["rdialect"].toString() );
		BOOST_FOREACH( std::list< data::Image >::reference imageRef, tmpList ) {
			if( app.parameters["old_lipsia"] ) {
				setOrientationToIdentity( imageRef );
			}
			imgList.push_back( imageRef );
		}
	}
	//load the zmap images
	BOOST_FOREACH ( util::slist::const_reference fileName, zmapFileList ) {
		std::list< data::Image > tmpList = data::IOFactory::load( fileName, app.parameters["rf"].toString(), app.parameters["rdialect"].toString() );
		BOOST_FOREACH( std::list< data::Image >::reference imageRef, tmpList ) {
			if( app.parameters["old_lipsia"] ) {
				setOrientationToIdentity( imageRef );
			}
			zImgList.push_back( imageRef );
		}
	}
	
//*****************************************************************************************
//distribution of images
//*****************************************************************************************

	typedef std::list< boost::shared_ptr<ImageHolder > >::const_reference ImageListRef;
	
	//particular distribution of images in widgets
	if( app.parameters["zmap"].isSet() && zImgList.size() > 1 ) {
		core->getUI()->setViewWidgetArrangement( UICore::InRow );
		BOOST_FOREACH( ImageListRef image, core->addImageList( zImgList, ImageHolder::z_map ) )
		{
			UICore::ViewWidgetEnsembleType ensemble = core->getUI()->createViewWidgetEnsemble( "", image );
			if( app.parameters["in"].isSet() ) {	
				boost::shared_ptr<ImageHolder> anatomicalImage = core->addImage( imgList.front(), ImageHolder::anatomical_image);
				ensemble[0].widgetImplementation->addImage( anatomicalImage );
				ensemble[1].widgetImplementation->addImage( anatomicalImage );
				ensemble[2].widgetImplementation->addImage( anatomicalImage );
			}
		}
		core->getUI()->setOptionPosition( isis::viewer::UICore::bottom );
	//only anatomical images with split option was specified
	} else if ( app.parameters["in"].isSet() && app.parameters["split"].isSet() ) {
		core->getUI()->setViewWidgetArrangement( UICore::InRow );
		BOOST_FOREACH( ImageListRef image, core->addImageList( imgList, ImageHolder::anatomical_image ) )
		{
			core->getUI()->createViewWidgetEnsemble( "", image );
		}
		core->getUI()->setOptionPosition( isis::viewer::UICore::bottom );
	} else if ( app.parameters["in"].isSet() || app.parameters["zmap"].isSet() ) {
		core->getUI()->setViewWidgetArrangement( UICore::Default );
		UICore::ViewWidgetEnsembleType ensemble = core->getUI()->createViewWidgetEnsemble( "" );
		BOOST_FOREACH( ImageListRef image, core->addImageList( imgList, ImageHolder::anatomical_image ) )
		{
			core->attachImageToWidget( image, ensemble[0]. widgetImplementation );
			core->attachImageToWidget( image, ensemble[1]. widgetImplementation );
			core->attachImageToWidget( image, ensemble[2]. widgetImplementation );
		}
		BOOST_FOREACH( ImageListRef image, core->addImageList( zImgList, ImageHolder::z_map	) )
		{
			core->attachImageToWidget( image, ensemble[0]. widgetImplementation );
			core->attachImageToWidget( image, ensemble[1]. widgetImplementation );
			core->attachImageToWidget( image, ensemble[2]. widgetImplementation );
		}
		core->getUI()->setOptionPosition( isis::viewer::UICore::central11 );
		
	}
	core->getUI()->synchronize();
	core->getUI()->showMainWindow();

	return app.getQApplication().exec();
}
