#include "viewercorebase.hpp"
#include "common.hpp"

#define STR(s) _xstr_(s)
#define _xstr_(s) std::string(#s)

namespace isis
{
namespace viewer
{

ViewerCoreBase::ViewerCoreBase( )
	: m_CurrentTimestep( 0 ),
	  m_ColorHandler( new color::Color() ),
	  m_OptionsMap( boost::shared_ptr< util::PropertyMap >( new util::PropertyMap ) )
{
	m_ColorHandler->initStandardColormaps();
	m_VoxelTransformation.fill( 1.0 );
	setCommonViewerOptions();
}

ImageHolder::ImageListType ViewerCoreBase::addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType )
{
	ImageHolder::ImageListType retList;

	if( !imageList.empty() ) {
		BOOST_FOREACH( std::list< data::Image >::const_reference imageRef, imageList ) {
			boost::shared_ptr< ImageHolder > imageHolder = m_DataContainer.addImage( imageRef, imageType );
			checkForCaCp( imageHolder );
			retList.push_back( imageHolder );
			m_ImageList.push_back( imageHolder );
		}
		setCurrentImage( m_DataContainer.begin()->second );
	} else {
		LOG( Runtime, info ) << "The image list passed to the core is empty!";
	}

	return retList;
}

boost::shared_ptr<ImageHolder> ViewerCoreBase::addImage( const isis::data::Image &image, const isis::viewer::ImageHolder::ImageType &imageType )
{
	boost::shared_ptr<ImageHolder> retImage = m_DataContainer.addImage( image, imageType );
	checkForCaCp(retImage);
	m_ImageList.push_back( retImage );
	setCurrentImage( m_DataContainer.begin()->second );
	return retImage;
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
	return STR( _ISIS_VIEWER_VERSION_MAJOR ) + "." + STR( _ISIS_VIEWER_VERSION_MINOR ) + "." + STR( _ISIS_VIEWER_VERSION_PATCH );
}

util::fvector4 ViewerCoreBase::getTransformedCoords( const isis::util::fvector4 &coords ) const
{
	return util::fvector4( coords[0] * m_VoxelTransformation[0],
						   coords[1] * m_VoxelTransformation[1],
						   coords[2] * m_VoxelTransformation[2],
						   coords[3] * m_VoxelTransformation[3] );
}

void ViewerCoreBase::setCommonViewerOptions()
{

	m_OptionsMap->setPropertyAs<bool>( "propagateZooming", false );
	m_OptionsMap->setPropertyAs<bool>( "showLables", false );
	m_OptionsMap->setPropertyAs<uint16_t>( "minMaxSearchRadius", 20 );
	m_OptionsMap->setPropertyAs<bool>( "showAdvancedFileDialogOptions", false );
	//logging
	m_OptionsMap->setPropertyAs<uint16_t>( "logDelayTime", 5000 );
	m_OptionsMap->setPropertyAs<bool>( "showErrorMessages", true );
	m_OptionsMap->setPropertyAs<bool>( "showNoticeMessages", true );
	m_OptionsMap->setPropertyAs<bool>( "showWarningMessages", false );
	m_OptionsMap->setPropertyAs<bool>( "showInfoMessages", false );
	m_OptionsMap->setPropertyAs<bool>( "showVerboseInfoMessages", false );
}



}
} // end namespace