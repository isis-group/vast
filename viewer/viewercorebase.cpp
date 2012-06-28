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
 * viewercorebase.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "viewercorebase.hpp"
#include "common.hpp"
#include "geometrical.hpp"
#include "nativeimageops.hpp"

#define STR(s) _xstr_(s)
#define _xstr_(s) std::string(#s)

namespace isis
{
namespace viewer
{

ViewerCoreBase::ViewerCoreBase( )
	: m_Settings( boost::shared_ptr< Settings >( new Settings ) ),
	  m_CurrentAnatomicalReference( boost::shared_ptr<ImageHolder>() ),
	  m_Mode( default_mode )
{
	util::Singletons::get<color::Color, 10>().initStandardColormaps();
}

ImageHolder::Vector ViewerCoreBase::addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType )
{
	ImageHolder::Vector retList;

	if( !imageList.empty() ) {
		BOOST_FOREACH( std::list< data::Image >::const_reference imageRef, imageList ) {
			retList.push_back( addImage( imageRef, imageType ) );
		}
	} else {
		LOG( Runtime, info ) << "The image list passed to the core is empty!";
	}

	return retList;
}

ImageHolder::Pointer ViewerCoreBase::addImage( const isis::data::Image &image, const isis::viewer::ImageHolder::ImageType &imageType )
{
	std::string fileName;

	if( image.hasProperty( "source" ) ) {
		fileName = image.getPropertyAs<std::string>( "source" );
	} else {
		const boost::filesystem::path path = image.getChunk( 0 ).getPropertyAs<std::string>( "source" );
		fileName = path.branch_path().string();
	}

	ImageHolder::Pointer  retImage = ImageHolder::Pointer( new ImageHolder );
	m_imageVector.push_back( retImage );

	//look if this filename already exists.
	if( m_ImageMap.find( fileName ) != m_ImageMap.end() ) {
		unsigned short index = 0;
		std::string newFileName = fileName;

		while( m_ImageMap.find( newFileName ) != m_ImageMap.end() ) {
			std::stringstream ss;
			ss << fileName << " (" << ++index << ")";
			newFileName = ss.str();
		}

		retImage->setImage( image, imageType, newFileName );
		m_ImageMap[newFileName] = retImage;
	} else {
		retImage->setImage( image, imageType, fileName );
		m_ImageMap[fileName] = retImage;
	}

	//setting the lutStructural
	if( imageType == ImageHolder::structural_image ) {
		retImage->getImageProperties().lut = getSettings()->getPropertyAs<std::string>( "lutStructural" );

		if( !util::Singletons::get<color::Color, 10>().hasColormap( retImage->getImageProperties().lut ) ) {
			retImage->getImageProperties().lut = std::string( "standard_grey_values" );
			getSettings()->setPropertyAs<std::string>( "lutStructural", retImage->getImageProperties().lut );
		}
	} else {
		retImage->getImageProperties().lut = getSettings()->getPropertyAs<std::string>( "lutZMap" );

		if( !util::Singletons::get<color::Color, 10>().hasColormap( retImage->getImageProperties().lut ) ) {
			retImage->getImageProperties().lut = std::string( "standard_zmap" );
			getSettings()->setPropertyAs<std::string>( "lutZMap", retImage->getImageProperties().lut );
		}
	}

	retImage->updateColorMap();

	if( imageType == ImageHolder::structural_image && image.getSizeAsVector()[3] == 1 ) {
		m_CurrentAnatomicalReference = retImage;
	}

	if( getMode() == ViewerCoreBase::statistical_mode && retImage->getImageSize()[3] > 1 && retImage->getImageProperties().imageType != ImageHolder::statistical_image ) {
		retImage->getImageProperties().isVisible = false;
	}

	retImage->getImageProperties().boundingBox = geometrical::getPhysicalBoundingBox( retImage );

	if( getSettings()->getPropertyAs<bool>( "checkCACP" ) ) {
		checkForCaCp( retImage );
	}

	if( getSettings()->getPropertyAs<bool>( "setZeroToBlackStructural" ) && retImage->getImageProperties().imageType == ImageHolder::structural_image ) {
		operation::NativeImageOps::setTrueZero( retImage );
	}

	if( getSettings()->getPropertyAs<bool>( "setZeroToBlackStatistical" ) && retImage->getImageProperties().imageType == ImageHolder::statistical_image ) {
		operation::NativeImageOps::setTrueZero( retImage );
	}

	//connect signals to image
	emitGlobalPhysicalCoordsChanged.connect( boost::bind( &ImageHolder::phyisicalCoordsChanged, retImage, _1 ) );
	emitGlobalVoxelCoordsChanged.connect( boost::bind( &ImageHolder::voxelCoordsChanged, retImage, _1 ) );
	emitGlobalTimestepChanged.connect( boost::bind( &ImageHolder::timestepChanged, retImage, _1 ) );

	//emit the signal
	emitAddImage( retImage );
	return retImage;
}

bool ViewerCoreBase::removeImage ( const ImageHolder::Pointer image )
{
	const size_t oldNumberImages = getImageVector().size();
	LOG( Dev, info ) << "Removing image " << image->getImageProperties().fileName;

	//if this image is the current one, we have to set one of the residual images to the current one
	if( getCurrentImage().get() == image.get() ) {
		LOG( Dev, info ) << "This was the current image so setting one of the residual images to current image";
		ImageHolder::Vector tmpList = getImageVector();
		tmpList.erase( std::find ( tmpList.begin(), tmpList.end(), image ) );

		if( tmpList.size() ) {
			setCurrentImage( tmpList.front() );
		} else {
			setCurrentImage( ImageHolder::Pointer() );
		}
	}

	getImageVector().erase( std::find ( getImageVector().begin(), getImageVector().end(), image ) );
	getImageMap().erase( image->getImageProperties().filePath );

	emitRefreshAllWidgets();

	if( ( getImageVector().size() == getImageMap().size() ) && ( getImageVector().size() == ( oldNumberImages - 1 ) ) ) {
		return true;
	} else {
		return false;
	}
}


ImageHolder::Pointer ViewerCoreBase::getCurrentImage() const
{
	if( m_CurrentImage.get() ) {
		return m_CurrentImage;
	} else {
		LOG( Dev, error ) << "Trying to fetch the current image. But there is no current image. Should be checked before.";
		return ImageHolder::Pointer();
	}

}


std::string ViewerCoreBase::getVersion()
{
#ifdef VAST_RCS_REVISION
	return STR( _VAST_VERSION_MAJOR ) + "." + STR( _VAST_VERSION_MINOR ) + "." + STR( _VAST_VERSION_PATCH ) + " [" + STR( VAST_RCS_REVISION ) + "]";
#else
	return STR( _VAST_VERSION_MAJOR ) + "." + STR( _VAST_VERSION_MINOR ) + "." + STR( _VAST_VERSION_PATCH );
#endif
}

bool ViewerCoreBase::hasWidget ( const std::string &identifier )
{
	const widget::WidgetLoader::WidgetMapType widgetMap = util::Singletons::get<widget::WidgetLoader, 10>().getWidgetMap();
	return widgetMap.find( identifier ) != widgetMap.end();
}


widget::WidgetInterface *ViewerCoreBase::getWidget ( const std::string &identifier ) throw( std::runtime_error & )
{
	const widget::WidgetLoader::WidgetMapType widgetMap = util::Singletons::get<widget::WidgetLoader, 10>().getWidgetMap();

	if( widgetMap.empty() ) {
		LOG( Dev, error ) << "Could not find any widget!" ;
	}

	if( widgetMap.find( identifier ) != widgetMap.end() ) {
		LOG( Dev, info ) << "Loading widget of identifier \"" << identifier << "\".";
		return widgetMap.at( identifier )();
	} else {
		LOG( Dev, error ) << "Can not find any widget with identifier \"" << identifier
						  << "\"! Returning first widget i can find.";
		const std::string fallback = widgetMap.begin()->first;
		getSettings()->setPropertyAs<std::string>( "defaultViewWidgetIdentifier", fallback );
		getSettings()->save();
		return getWidget( fallback );
	}
}


const util::PropertyMap *ViewerCoreBase::getWidgetProperties ( const std::string &identifier )
{
	widget::WidgetLoader::WidgetPropertyMapType widgetPropertyMap = util::Singletons::get<widget::WidgetLoader, 10>().getWidgetPropertyMap();

	if( widgetPropertyMap.empty() ) {
		LOG( Dev, error ) << "Could not find any widget!" ;
	}

	if( widgetPropertyMap.find( identifier ) != widgetPropertyMap.end() ) {
		LOG( Dev, info ) << "Loading widget properties of identifier \"" << identifier << "\".";
		return widgetPropertyMap.at( identifier );
	} else {
		LOG( Dev, error ) << "Can not find any widget properties with identifier \"" << identifier
						  << "\"! Returning properties of the first widget i can find.";
		const std::string fallback = widgetPropertyMap.begin()->first;
		getSettings()->setPropertyAs<std::string>( "defaultViewWidgetIdentifier", fallback );
		getSettings()->save();
		return getWidgetProperties( fallback );
	}
}

}
} // end namespace