
#include "imageholder.hpp"

namespace isis
{
namespace viewer
{

ImageHolder::ImageHolder( )
	: m_NumberOfTimeSteps( 0 ),
	  m_CutAwayPair( std::make_pair<double, double>( 0.03, 0.03 ) )
{
}
boost::numeric::ublas::matrix< float > ImageHolder::getNormalizedImageOrientation( bool transposed ) const
{
	boost::numeric::ublas::matrix<float> retMatrix = boost::numeric::ublas::zero_matrix<float>( 4, 4 );
	retMatrix( 3, 3 ) = 1;
	float deg45 = sin((45.0 / 180) * M_PI);
	util::fvector4 rowVec = m_Image->getPropertyAs<util::fvector4>( "rowVec" );
	util::fvector4 columnVec = m_Image->getPropertyAs<util::fvector4>( "columnVec" );
	util::fvector4 sliceVec = m_Image->getPropertyAs<util::fvector4>( "sliceVec" );
	size_t rB = rowVec.getBiggestVecElemAbs();
	size_t cB = columnVec.getBiggestVecElemAbs();
	size_t sB = sliceVec.getBiggestVecElemAbs();
	//if image is rotated of 45 °
	if( rB == cB ) {
		if( sB == 0 ) {
			rB = 1; cB = 2;
		} else if ( sB == 1 ) {
			rB = 0; cB = 2;
		} else if ( sB = 2 ) {
			rB = 0; rB = 1;
		}
	}
	if( rB == sB ) {
		if( cB == 0 ) {
			rB = 1; sB = 2;
		} else if ( cB == 1 ) {
			rB = 0; sB = 2;
		} else if ( cB = 2 ) {
			rB = 0; sB = 1;
		}
	}
	if( cB == sB ) {
		if( rB == 0 ) {
			cB = 1; sB = 2;
		} else if ( rB == 1 ) {
			cB = 0; sB = 2;
		} else if ( rB = 2 ) {
			cB = 0; sB = 1;
		}
	}
	if( !transposed ) {
		retMatrix( rB, 0 ) = rowVec[rB] < 0 ? -1 : 1;
		retMatrix( cB, 1 ) = columnVec[cB] < 0 ? -1 : 1;
		retMatrix( sB, 2 ) =  sliceVec[sB] < 0 ? -1 : 1;
	} else {
		retMatrix( 0, rB ) = rowVec[rB] < 0 ? -1 : 1;
		retMatrix( 1, cB ) =  columnVec[cB] < 0 ? -1 : 1;
		retMatrix( 2, sB ) = sliceVec[sB] < 0 ? -1 : 1;
	}
	return retMatrix;
}

boost::numeric::ublas::matrix< float > ImageHolder::getImageOrientation( bool transposed ) const
{
	boost::numeric::ublas::matrix<float> retMatrix = boost::numeric::ublas::zero_matrix<float>( 4, 4 );
	retMatrix( 3, 3 ) = 1;
	util::fvector4 rowVec = m_Image->getPropertyAs<util::fvector4>( "rowVec" );
	util::fvector4 columnVec = m_Image->getPropertyAs<util::fvector4>( "columnVec" );
	util::fvector4 sliceVec = m_Image->getPropertyAs<util::fvector4>( "sliceVec" );

	for ( uint16_t i = 0; i < 3; i++ ) {
		if( !transposed ) {
			retMatrix( i, 0 ) = rowVec[i];
			retMatrix( i, 1 ) = columnVec[i];
			retMatrix( i, 2 ) = sliceVec[i];
		} else {
			retMatrix( 0, i ) = rowVec[i];
			retMatrix( 1, i ) = columnVec[i];
			retMatrix( 2, i ) = sliceVec[i];
		}
	}

	return retMatrix;
}

bool ImageHolder::setImage( const data::Image &image, const ImageType &imageType, const std::string &filename )
{

	//some checks
	if( image.isEmpty() ) {
		LOG( Runtime, error ) << "Getting an empty image? Obviously something went wrong.";
		return false;
	}

	m_Image.reset( new data::Image( image ) );

	//if no filename was specified we have to search for the filename by ourselfes
	if( filename.empty() ) {
		// go through all the chunks and search for filenames. We use a set here to avoid redundantly filenames
		std::set<std::string> filenameSet;
		BOOST_FOREACH( std::vector< data::Chunk >::const_reference chRef, image.copyChunksToVector() ) {
			filenameSet.insert( chRef.getPropertyAs<std::string>( "source" ) );
		}
		//now we pack our filenameSet into our slist of filenames
		BOOST_FOREACH( std::set<std::string>::const_reference setRef, filenameSet ) {
			m_Filenames.push_back( setRef );
		}
	} else {
		m_Filenames.push_back( filename );
	}

	// get some image information
	m_MinMax = image.getMinMax();
	m_ImageSize = image.getSizeAsVector();
	m_NumberOfTimeSteps = m_ImageSize[3];
	LOG( Debug, verbose_info )  << "Fetched image of size " << m_ImageSize << " and type "
								<< image.getMajorTypeName() << ".";
	//copy the image into continuous memory space and assure consistent data type
	data::ValuePtr<InternalImageType> imagePtr( ( InternalImageType * ) calloc( image.getVolume(), sizeof( InternalImageType ) ), image.getVolume() );
	LOG( Debug, verbose_info ) << "Needed memory: " << image.getVolume() * sizeof( InternalImageType ) / ( 1024.0 * 1024.0 ) << " mb.";
	image.copyToMem<InternalImageType>( &imagePtr[0], image.getVolume() );
	LOG( Debug, verbose_info ) << "Copied image to continuous memory space.";
	m_InternMinMax = imagePtr.getMinMax();

	//splice the image in its volumes -> we get a vector of t volumes
	if( m_NumberOfTimeSteps > 1 ) { //splicing is only necessary if we got more than 1 timestep
		m_ImageVector = imagePtr.splice( m_ImageSize[0] * m_ImageSize[1] * m_ImageSize[2] );
	} else {
		m_ImageVector.push_back( imagePtr );
	}

	LOG_IF( m_ImageVector.empty(), Runtime, error ) << "Size of image vector is 0!";

	if( m_ImageVector.size() != m_NumberOfTimeSteps ) {
		LOG( Runtime, error ) << "The number of timesteps (" << m_NumberOfTimeSteps
							  << ") does not coincide with the number of volumes ("  << m_ImageVector.size() << ").";
		return false;
	}

	//create the chunk vector
	BOOST_FOREACH( std::vector< ImagePointerType >::const_reference pointerRef, m_ImageVector ) {
		m_ChunkVector.push_back( data::Chunk( pointerRef, m_ImageSize[0], m_ImageSize[1], m_ImageSize[2] ) );
	}


	LOG( Debug, verbose_info ) << "Spliced image to " << m_ImageVector.size() << " volumes.";

	m_OptimalScalingPair = getOptimalScalingToForType<InternalImageType>( m_CutAwayPair );
	//image seems to be ok...i guess

	//add some more properties
	m_ImageProperties.imageType = imageType;
	m_ImageProperties.interpolationType = nn;

	m_ImageProperties.zmapThreshold = std::make_pair<double, double>( 0, 0 );

	if( imageType == z_map ) {
		m_PropMap.setPropertyAs<double>( "lowerThreshold", 0 );
		m_PropMap.setPropertyAs<double>( "upperThreshold", 0 );
		m_PropMap.setPropertyAs<std::string>("lut", std::string("standard_zmap") );
	} else if( imageType == anatomical_image ) {
		m_PropMap.setPropertyAs<double>( "lowerThreshold", getMinMax().first->as<double>() );
		m_PropMap.setPropertyAs<double>( "upperThreshold", getMinMax().second->as<double>() );
		m_PropMap.setPropertyAs<std::string>("lut", std::string("standard_grey_values") );
	}
	m_PropMap.setPropertyAs<double>( "extent", fabs( getMinMax().second->as<double>() - getMinMax().first->as<double>() ) );
	m_PropMap.setPropertyAs<util::ivector4>( "voxelCoords", util::ivector4( m_ImageSize[0] / 2, m_ImageSize[1] / 2, m_ImageSize[2] / 2, 0 ) );
	m_PropMap.setPropertyAs<util::fvector4>( "physicalCoords", m_Image->getPhysicalCoordsFromIndex( m_PropMap.getPropertyAs<util::ivector4>( "voxelCoords" ) ) );

	m_PropMap.setPropertyAs<bool>( "isVisible", true );
	m_PropMap.setPropertyAs<float>( "opacity", 1.0 );
	m_PropMap.setPropertyAs<uint16_t>( "currentTimestep", 0 );
	m_PropMap.setPropertyAs<util::ivector4>( "alignedSize32Bit", get32BitAlignedSize( m_ImageSize ) );
	m_PropMap.setPropertyAs<bool>( "init", true );
	m_PropMap.setPropertyAs<util::slist>( "changedAttributes", util::slist() );
	m_PropMap.setPropertyAs<double>("offset", 0.0);
	m_PropMap.setPropertyAs<double>("scaling", 1.0);
	m_Image->updateOrientationMatrices();
	return true;
}

void ImageHolder::addChangedAttribute(const std::string& attribute)
{
	util::slist attributes = m_PropMap.getPropertyAs<util::slist>("changedAttributes");
	attributes.push_back( attribute );
	m_PropMap.setPropertyAs<util::slist>("changedAttributes", attributes );
}


}
} //end namespace

