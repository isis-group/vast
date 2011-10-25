#include "viewercorebase.hpp"

#define STR(s) _xstr_(s)
#define _xstr_(s) std::string(#s)

namespace isis
{
namespace viewer
{

ViewerCoreBase::ViewerCoreBase( )
	: m_CurrentTimestep( 0 )
{
	m_Options = new OptionStruct;
	m_Options->propagateZooming = false;
	m_VoxelTransformation.fill( 1.0 );
}

std::list<boost::shared_ptr<ImageHolder> > ViewerCoreBase::addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType )
{
	std::list<boost::shared_ptr<ImageHolder> > retList;

	if( !imageList.empty() ) {
		BOOST_FOREACH( std::list< data::Image >::const_reference imageRef, imageList ) {
			retList.push_back( m_DataContainer.addImage( imageRef, imageType ) );
		}
		setCurrentImage( m_DataContainer.begin()->second );
	} else {
		LOG( Runtime, warning ) << "The image list passed to the core is empty!";
	}

	return retList;
}

void ViewerCoreBase::addImage( const isis::data::Image &image, const isis::viewer::ImageHolder::ImageType &imageType )
{
	m_DataContainer.addImage( image, imageType );
	setCurrentImage( m_DataContainer.begin()->second );
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




}
} // end namespace