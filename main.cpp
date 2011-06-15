#include "Adapter/qtapplication.hpp"
#include "MainWindow.hpp"
#include "QViewerCore.hpp"
#include <iostream>
#include <DataStorage/io_factory.hpp>
#include <DataStorage/image.hpp>
#include <CoreUtils/log.hpp>

#include "common.hpp"

int main( int argc, char *argv[] )
{
	using namespace isis::viewer;
	isis::viewer::QViewerCore *core = new isis::viewer::QViewerCore;

	isis::util::Selection dbg_levels( "error,warning,info,verbose_info" );
	dbg_levels.set( "warning" );
	isis::util::Selection image_types( "anatomical,zmap" );
	image_types.set( "anatomical" );
	isis::qt4::IOQtApplication app( "isisViewer", false, false );
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
	app.parameters["identity"] = bool();
	app.parameters["identity"] = false;
	app.parameters["identity"].needed() = false;
	app.parameters["identity"].setDescription( "Sets the orientation of all images to identity and index origin to 0" );
	app.parameters["dViewer"] = dbg_levels;
	app.parameters["dViewer"].setDescription( "Debugging level for the Viewer module" );
	app.parameters["dViewer"].hidden() = true;
	app.parameters["dViewer"].needed() = false;
	app.parameters["rf"] = std::string();
	app.parameters["rf"].needed() = false;
	app.parameters["rf"].setDescription( "Override automatic detection of file suffix for reading with given value" );
	app.parameters["rf"].hidden() = true;
	app.parameters["split"] = false;
	app.parameters["split"].needed() = false;
	app.parameters["split"].setDescription( "Show each image in a separate view" );
	app.init( argc, argv, true );
	app.setLog<isis::ViewerLog>( app.getLLMap()[app.parameters["dViewer"]->as<isis::util::Selection>()] );
	app.setLog<isis::ViewerDebug>( app.getLLMap()[app.parameters["dViewer"]->as<isis::util::Selection>()] );
	isis::util::slist fileList = app.parameters["in"];
	isis::util::slist zmapFileList = app.parameters["zmap"];
	std::list< isis::data::Image > imgList;
	std::list< isis::data::Image > zImgList;

	//load the anatomical images
	BOOST_FOREACH ( isis::util::slist::const_reference fileName, fileList ) {
		std::list< isis::data::Image > tmpList = isis::data::IOFactory::load( fileName, app.parameters["rf"].toString() );
		BOOST_FOREACH( std::list< isis::data::Image >::reference imageRef, tmpList ) {
			if( app.parameters["identity"] ) {
				setOrientationToIdentity( imageRef );
			}

			imgList.push_back( imageRef );
		}
	}
	//load the zmap images
	BOOST_FOREACH ( isis::util::slist::const_reference fileName, zmapFileList ) {
		std::list< isis::data::Image > tmpList = isis::data::IOFactory::load( fileName, app.parameters["rf"].toString() );
		BOOST_FOREACH( std::list< isis::data::Image >::reference imageRef, tmpList ) {
			if( app.parameters["identity"] ) {
				setOrientationToIdentity( imageRef );
			}

			zImgList.push_back( imageRef );
		}
	}
	bool assamble = false;
	isis::viewer::MainWindow isisViewerMainWindow( core );

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
	if(assamble) {
		isisViewerMainWindow.assembleViewInRows();
	}
	isisViewerMainWindow.show();
	return app.getQApplication().exec();
}
