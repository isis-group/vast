#include <Adapter/qtapplication.hpp>
#include "MainWindowUIInterface.hpp"
#include "QViewerCore.hpp"
#include <iostream>
#include <DataStorage/io_factory.hpp>
#include <DataStorage/image.hpp>
#include <CoreUtils/log.hpp>

#include "common.hpp"

int main( int argc, char *argv[] )
{
	std::string appName = "vast";
	std::string orgName = "cbs.mpg.de";
	std::map<std::string, isis::viewer::WidgetType> wTypeMap;
	using namespace isis::viewer;
	isis::viewer::QViewerCore *core = new isis::viewer::QViewerCore( appName, orgName );

	isis::util::Selection dbg_levels( "error,warning,info,verbose_info" );
	isis::util::Selection wTypes( "gl,qt" );
	wTypeMap.insert( std::make_pair<std::string, isis::viewer::WidgetType>( "gl", type_gl ) );
	wTypeMap.insert( std::make_pair<std::string, isis::viewer::WidgetType>( "qt", type_qt ) );
	wTypes.set( "gl" );
	dbg_levels.set( "warning" );
	isis::util::Selection image_types( "anatomical,zmap" );
	image_types.set( "anatomical" );

	isis::qt4::IOQtApplication app( appName.c_str(), false, false );
	std::cout << "v" << core->getVersion() << " ( isis core: " << app.getCoreVersion() << " )" << std::endl;
	app.parameters["in"] = isis::util::slist();
	app.parameters["in"].needed() = false;
	app.parameters["in"].setDescription( "The input image file list." );
	app.parameters["zmap"] = isis::util::slist();
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
	boost::shared_ptr< isis::util::ProgressFeedback > feedback = boost::shared_ptr<isis::util::ProgressFeedback>( new isis::util::ConsoleFeedback );
	isis::data::IOFactory::setProgressFeedback( feedback );
	app.init( argc, argv, true );
	app.setLog<isis::ViewerLog>( app.getLLMap()[app.parameters["dViewer"]->as<isis::util::Selection>()] );
	app.setLog<isis::ViewerDebug>( app.getLLMap()[app.parameters["dViewer"]->as<isis::util::Selection>()] );
	isis::util::slist fileList = app.parameters["in"];
	isis::util::slist zmapFileList = app.parameters["zmap"];
	std::list< isis::data::Image > imgList;
	std::list< isis::data::Image > zImgList;

	//load the anatomical images
	BOOST_FOREACH ( isis::util::slist::const_reference fileName, fileList ) {
		std::list< isis::data::Image > tmpList = isis::data::IOFactory::load( fileName, app.parameters["rf"].toString(), app.parameters["rdialect"].toString() );
		BOOST_FOREACH( std::list< isis::data::Image >::reference imageRef, tmpList ) {
			//          if( app.parameters["old_lipsia"] ) {
			//              setOrientationToIdentity( imageRef );
			//          }

			imgList.push_back( imageRef );
		}
	}
	//load the zmap images
	BOOST_FOREACH ( isis::util::slist::const_reference fileName, zmapFileList ) {
		std::list< isis::data::Image > tmpList = isis::data::IOFactory::load( fileName, app.parameters["rf"].toString(), app.parameters["rdialect"].toString() );
		BOOST_FOREACH( std::list< isis::data::Image >::reference imageRef, tmpList ) {
			//          if( app.parameters["old_lipsia"] ) {
			//              setOrientationToIdentity( imageRef );
			//          }

			zImgList.push_back( imageRef );
		}
	}
	bool assamble = false;

	isis::viewer::MainWindowUIInterface isisViewerMainWindow( core, wTypeMap[app.parameters["wtype"].toString()] );

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
			isisViewerMainWindow.assembleViewInRows();
		} else  {
			core->addImageList( imgList, ImageHolder::anatomical_image, true );
		}

	} else if ( app.parameters["type"].toString() == "zmap" && app.parameters["in"].isSet() ) {
		core->addImageList( imgList, ImageHolder::z_map, true );
	}

	if( assamble ) {
		isisViewerMainWindow.assembleViewInRows();
	}

	isisViewerMainWindow.show();
	return app.getQApplication().exec();
}
