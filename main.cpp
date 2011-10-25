#include <Adapter/qtapplication.hpp>
#include "mainWindowUIInterface.hpp"
#include "qviewercore.hpp"
#include <iostream>
#include <DataStorage/io_factory.hpp>
#include <DataStorage/image.hpp>
#include <CoreUtils/log.hpp>
#include "pluginloader.hpp"

#include "common.hpp"

int main( int argc, char *argv[] )
{
	using namespace isis;
	using namespace viewer;

	ENABLE_LOG( data::Runtime, util::DefaultMsgPrint, error );
	std::string appName = "vast";
	std::string orgName = "cbs.mpg.de";
	std::map<std::string, WidgetType> wTypeMap;

	QViewerCore *core = new QViewerCore( appName, orgName );

	util::Selection dbg_levels( "error,warning,info,verbose_info" );
	util::Selection wTypes( "gl,qt" );
	wTypeMap.insert( std::make_pair<std::string, WidgetType>( "gl", type_gl ) );
	wTypeMap.insert( std::make_pair<std::string, WidgetType>( "qt", type_qt ) );
	wTypes.set( "qt" );
	dbg_levels.set( "warning" );
	util::Selection image_types( "anatomical,zmap" );
	image_types.set( "anatomical" );

	qt4::IOQtApplication app( appName.c_str(), false, false, std::string("raster") );

	std::cout << "v" << core->getVersion() << " ( isis core: " << app.getCoreVersion() << " )" << std::endl;
	app.parameters["in"] = util::slist();
	app.parameters["in"].needed() = false;
	app.parameters["in"].setDescription( "The input image file list." );
	app.parameters["zmap"] = util::slist();
	app.parameters["zmap"].needed() = false;
	app.parameters["zmap"].setDescription( "The input image file list is interpreted as zmaps. " );
	app.parameters["type"] = image_types;
	app.parameters["type"].needed() = false;
	app.parameters["type"].setDescription( "The type as what the image should be interpreted." );
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
	//setting graphics mode
	app.init( argc, argv, true );
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
			//          if( app.parameters["old_lipsia"] ) {
			//              setOrientationToIdentity( imageRef );
			//          }

			imgList.push_back( imageRef );
		}
	}
	//load the zmap images
	BOOST_FOREACH ( util::slist::const_reference fileName, zmapFileList ) {
		std::list< data::Image > tmpList = data::IOFactory::load( fileName, app.parameters["rf"].toString(), app.parameters["rdialect"].toString() );
		BOOST_FOREACH( std::list< data::Image >::reference imageRef, tmpList ) {
			//          if( app.parameters["old_lipsia"] ) {
			//              setOrientationToIdentity( imageRef );
			//          }

			zImgList.push_back( imageRef );
		}
	}
	bool assamble = false;
	MainWindowUIInterface *isisViewerMainWindow = new MainWindowUIInterface( core, wTypeMap[app.parameters["wtype"].toString()] );
	//because some plugins may use a gui we have to pass a parent
	core->setParentWidget( isisViewerMainWindow );
	//scan for plugins and hand them to the core
	core->addPlugins( plugin::PluginLoader::get().getPlugins() );
	isisViewerMainWindow->reloadPluginsToGUI();
	

	if( app.parameters["zmap"].isSet() ) {
		if( app.parameters["split"] && zImgList.size() > 1 ) {
			core->addImageList( zImgList, ImageHolder::z_map, false );
			assamble = true;
		} else {
			core->addImageList( zImgList, ImageHolder::z_map, true );
		}
	}

	if( app.parameters["type"].toString() == "anatomical" && app.parameters["in"].isSet() ) {
		if( imgList.size() > 1 && app.parameters["split"] ) {
			core->addImageList( imgList, ImageHolder::anatomical_image, false );
			isisViewerMainWindow->assembleViewInRows();
		} else  {
			core->addImageList( imgList, ImageHolder::anatomical_image, true );
		}

	} else if ( app.parameters["type"].toString() == "zmap" && app.parameters["in"].isSet() ) {
		core->addImageList( imgList, ImageHolder::z_map, true );
	}

	if( assamble ) {
		isisViewerMainWindow->assembleViewInRows();
	}

	isisViewerMainWindow->show();
	return app.getQApplication().exec();
}
