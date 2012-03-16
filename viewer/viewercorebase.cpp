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

ImageHolder::List ViewerCoreBase::addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType )
{
	ImageHolder::List retList;

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
	ImageHolder::Pointer retImage = m_DataContainer.addImage( image, imageType );
	if( retImage->hasAmbiguousOrientation() ) {
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Warning);
		std::stringstream message;
		message << "The image " << retImage->getFileNames().front()
			<< " has an ambiguous orientation (rotated through 45 degrees).\n\n Alignment might look wrong.";
		msgBox.setText( message.str().c_str() );
		msgBox.exec();
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

	if ( getMode() == ViewerCoreBase::statistical_mode && retImage->getImageProperties().imageType == ImageHolder::statistical_image ) {
		setCurrentImage( retImage );
	} else if ( getMode() == ViewerCoreBase::default_mode ) {
		setCurrentImage( retImage );
	}

	return retImage;
}

ImageHolder::Pointer ViewerCoreBase::getCurrentImage()
{
	if( m_CurrentImage.get() ) {
		return m_CurrentImage;
	} else {
		LOG( Runtime, error ) << "Trying to fetch the current image. But there is no current image at all. Should be checked before.";
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



}
} // end namespace