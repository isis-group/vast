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

#include <signal.h>

#define STR(s) _xstr_(s)
#define _xstr_(s) std::string(#s)

namespace isis
{
namespace viewer
{

ViewerCoreBase::ViewerCoreBase( )
	: m_OptionsMap( boost::shared_ptr< util::PropertyMap >( new util::PropertyMap ) ),
	  m_CurrentAnatomicalReference( boost::shared_ptr<ImageHolder>() ),
	  m_Mode( standard )
{
	
	util::Singletons::get<color::Color, 10>().initStandardColormaps();
	setCommonViewerOptions();
}

ImageHolder::ImageListType ViewerCoreBase::addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType )
{
	ImageHolder::ImageListType retList;

	if( !imageList.empty() ) {
		BOOST_FOREACH( std::list< data::Image >::const_reference imageRef, imageList ) {
			retList.push_back( addImage( imageRef, imageType ) );
		}
	} else {
		LOG( Runtime, info ) << "The image list passed to the core is empty!";
	}

	return retList;
}

boost::shared_ptr<ImageHolder> ViewerCoreBase::addImage( const isis::data::Image &image, const isis::viewer::ImageHolder::ImageType &imageType )
{
	boost::shared_ptr<ImageHolder> retImage = m_DataContainer.addImage( image, imageType );

	//setting the lutStructural
	if( imageType == ImageHolder::structural_image ) {
		retImage->lut = getOptionMap()->getPropertyAs<std::string>( "lutStructural" );

		if( !util::Singletons::get<color::Color, 10>().hasColormap( retImage->lut ) ) {
			retImage->lut = std::string( "standard_grey_values" );
			getOptionMap()->setPropertyAs<std::string>( "lutStructural", retImage->lut );
		}
	} else {
		retImage->lut = getOptionMap()->getPropertyAs<std::string>( "lutZMap" );

		if( !util::Singletons::get<color::Color, 10>().hasColormap( retImage->lut ) ) {
			retImage->lut = std::string( "standard_zmap" );
			getOptionMap()->setPropertyAs<std::string>( "lutZMap", retImage->lut );
		}

	}

	retImage->updateColorMap();

	if( imageType == ImageHolder::structural_image && image.getSizeAsVector()[3] == 1 ) {
		m_CurrentAnatomicalReference = retImage;
	}

	m_ImageList.push_back( retImage );

	if( getMode() == ViewerCoreBase::zmap && retImage->getImageSize()[3] > 1 && retImage->imageType != ImageHolder::z_map ) {
		retImage->isVisible = false;
	}

	if ( getMode() == ViewerCoreBase::zmap && retImage->imageType == ImageHolder::z_map ) {
		setCurrentImage( retImage );
	} else if ( getMode() == ViewerCoreBase::standard ) {
		setCurrentImage( retImage );
	}

	return retImage;
}

boost::shared_ptr< ImageHolder > ViewerCoreBase::getCurrentImage()
{
	if( m_CurrentImage.get() ) {
		return m_CurrentImage;
	} else {
		LOG( Runtime, error ) << "Trying to fetch the current image. But there is no current image at all. Should be checked before.";
		return boost::shared_ptr< ImageHolder >();
	}

}

void ViewerCoreBase::setImageList( std::list< data::Image > imgList, const ImageHolder::ImageType &imageType )
{
	if( !imgList.empty() ) {
		m_DataContainer.clear();
	}

	ViewerCoreBase::addImageList( imgList, imageType );
}


std::string ViewerCoreBase::getVersion() const
{
#ifdef VAST_RCS_REVISION
        return STR( _VAST_VERSION_MAJOR ) + "." + STR( _VAST_VERSION_MINOR ) + "." + STR( _VAST_VERSION_PATCH ) + " [" + STR( VAST_RCS_REVISION ) + "]";
#else
        return STR( _VAST_VERSION_MAJOR ) + "." + STR( _VAST_VERSION_MINOR ) + "." + STR( _VAST_VERSION_PATCH );
#endif
}



void ViewerCoreBase::setCommonViewerOptions()
{
	m_OptionsMap->setPropertyAs<bool>( "zmapGlobal", false );
	m_OptionsMap->setPropertyAs<bool>("visualizeOnlyFirstVista", false );
	m_OptionsMap->setPropertyAs<bool>( "propagateZooming", false );
	m_OptionsMap->setPropertyAs<uint16_t>( "interpolationType" , 0 );
	m_OptionsMap->setPropertyAs<bool>( "showLables", false );
	m_OptionsMap->setPropertyAs<bool>( "showCrosshair", true );
	m_OptionsMap->setPropertyAs<uint16_t>( "minMaxSearchRadius", 20 );
	m_OptionsMap->setPropertyAs<bool>( "showAdvancedFileDialogOptions", false );
	m_OptionsMap->setPropertyAs<bool>( "showFavoriteFileList", false );
	m_OptionsMap->setPropertyAs<uint16_t>( "maxWidgetHeight", 200 );
	m_OptionsMap->setPropertyAs<uint16_t>( "maxWidgetWidth", 200 );
	m_OptionsMap->setPropertyAs<uint16_t>( "maxOptionWidgetHeight", 90 );
	m_OptionsMap->setPropertyAs<uint16_t>( "minOptionWidgetHeight", 90 );
	m_OptionsMap->setPropertyAs<bool>( "showStartWidget", true );
	m_OptionsMap->setPropertyAs<bool>( "showCrashMessage", true );
	m_OptionsMap->setPropertyAs<uint16_t>( "startWidgetHeight", 600 );
	m_OptionsMap->setPropertyAs<uint16_t>( "startWidgetWidth", 400 );
	m_OptionsMap->setPropertyAs<uint16_t>( "viewerWidgetMargin", 5 );
	//omp
	m_OptionsMap->setPropertyAs<uint16_t>( "numberOfThreads", 0 );
	m_OptionsMap->setPropertyAs<bool>( "ompAvailable", false );
	m_OptionsMap->setPropertyAs<bool>( "enableMultithreading", false );
	m_OptionsMap->setPropertyAs<uint16_t>( "initialMaxNumberThreads", 4 );
	m_OptionsMap->setPropertyAs<bool>( "useAllAvailableThreads", false );
	m_OptionsMap->setPropertyAs<uint16_t>( "maxNumberOfThreads", 1 );
	//screenshot
	m_OptionsMap->setPropertyAs<uint16_t>( "screenshotQuality", 70 );
	m_OptionsMap->setPropertyAs<uint16_t>( "screenshotWidth", 700 );
	m_OptionsMap->setPropertyAs<uint16_t>( "screenshotHeight", 700 );
	m_OptionsMap->setPropertyAs<uint16_t>( "screenshotDPIX", 300 );
	m_OptionsMap->setPropertyAs<uint16_t>( "screenshotDPIY", 300 );
	m_OptionsMap->setPropertyAs<bool>( "screenshotManualScaling", false );
	m_OptionsMap->setPropertyAs<bool>( "screenshotKeepAspectRatio", true );
	//lut
	m_OptionsMap->setPropertyAs<std::string>( "lutStructural", "standard_grey_values" );
	m_OptionsMap->setPropertyAs<std::string>( "lutZMap", "standard_zmap" );
	//misc
	m_OptionsMap->setPropertyAs<uint16_t>( "timeseriesPlayDelayTime", 50 );
	m_OptionsMap->setPropertyAs<bool>( "histogramOmitZero", true );
	m_OptionsMap->setPropertyAs<uint16_t>("maxRecentOpenListSize", 10 );
	//logging
	m_OptionsMap->setPropertyAs<uint16_t>( "logDelayTime", 6000 );
	m_OptionsMap->setPropertyAs<bool>( "showErrorMessages", true );
	m_OptionsMap->setPropertyAs<bool>( "showNoticeMessages", true );
	m_OptionsMap->setPropertyAs<bool>( "showWarningMessages", false );
	m_OptionsMap->setPropertyAs<bool>( "showInfoMessages", false );
	m_OptionsMap->setPropertyAs<bool>( "showVerboseInfoMessages", false );
	m_OptionsMap->setPropertyAs<std::string>("vastSymbol", std::string(":/common/minerva-MPG.png") );

	std::stringstream signature;
	signature << "vast v" << getVersion() << ", Max Planck Institut for human cognitive and brain sciences";
	m_OptionsMap->setPropertyAs<std::string>( "signature", signature.str() );
}



}
} // end namespace