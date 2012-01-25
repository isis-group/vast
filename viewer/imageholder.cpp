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
 * imageholder.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "imageholder.hpp"
#include "common.hpp"
#include <numeric>

namespace isis
{
namespace viewer
{

ImageHolder::ImageHolder()
	: m_ZeroIsReserved( true ),
	  m_ReservedValue( 0 )
{}

boost::numeric::ublas::matrix< double > ImageHolder::getNormalizedImageOrientation( bool transposed ) const
{
	boost::numeric::ublas::matrix<double> retMatrix = boost::numeric::ublas::zero_matrix<double>( 4, 4 );
	retMatrix( 3, 3 ) = 1;
	const util::fvector4 &rowVec = m_Image->propertyValue( "rowVec" )->castTo<util::fvector4>();
	const util::fvector4 &columnVec = m_Image->propertyValue( "columnVec" )->castTo<util::fvector4>();
	const util::fvector4 &sliceVec = m_Image->propertyValue( "sliceVec" )->castTo<util::fvector4>();
	size_t rB = rowVec.getBiggestVecElemAbs();
	size_t cB = columnVec.getBiggestVecElemAbs();
	size_t sB = sliceVec.getBiggestVecElemAbs();

	//if image is rotated of 45 °
	if( rB == cB ) {
		if( sB == 0 ) {
			rB = 1;
			cB = 2;
		} else if ( sB == 1 ) {
			rB = 0;
			cB = 2;
		} else if ( sB == 2 ) {
			rB = 0;
			rB = 1;
		}
	}

	if( rB == sB ) {
		if( cB == 0 ) {
			rB = 1;
			sB = 2;
		} else if ( cB == 1 ) {
			rB = 0;
			sB = 2;
		} else if ( cB == 2 ) {
			rB = 0;
			sB = 1;
		}
	}

	if( cB == sB ) {
		if( rB == 0 ) {
			cB = 1;
			sB = 2;
		} else if ( rB == 1 ) {
			cB = 0;
			sB = 2;
		} else if ( rB == 2 ) {
			cB = 0;
			sB = 1;
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

boost::numeric::ublas::matrix< double > ImageHolder::getImageOrientation( bool transposed ) const
{
	boost::numeric::ublas::matrix<double> retMatrix = boost::numeric::ublas::zero_matrix<double>( 4, 4 );
	retMatrix( 3, 3 ) = 1;
	const util::fvector4 &rowVec = m_Image->propertyValue( "rowVec" )->castTo<util::fvector4>();
	const util::fvector4 &columnVec = m_Image->propertyValue( "columnVec" )->castTo<util::fvector4>();
	const util::fvector4 &sliceVec = m_Image->propertyValue( "sliceVec" )->castTo<util::fvector4>();

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

bool ImageHolder::setImage( const data::Image &image, const ImageType &_imageType, const std::string &filename )
{
	LOG( Dev, info ) << "setImage of " << filename;
	//some checks
	if( image.isEmpty() ) {
		LOG( Runtime, error ) << "Getting an empty image? Obviously something went wrong.";
		LOG( Dev, error ) << "Image " << filename << " is empty";
		return false;
	}

	m_Image.reset( new data::Image( image ) );

	//if no filename was specified we have to search for the filename by ourselfes
	if( filename.empty() ) {
		LOG( Dev, warning ) << "filename.empty() is true";
		// go through all the chunks and search for filenames. We use a set here to avoid redundantly filenames
		std::set<std::string> filenameSet;
		BOOST_FOREACH( std::vector< data::Chunk >::const_reference chRef, image.copyChunksToVector() ) {
			filenameSet.insert( chRef.getPropertyAs<std::string>( "source" ) );
		}
		//now we pack our filenameSet into our slist of filenames
		BOOST_FOREACH( std::set<std::string>::const_reference setRef, filenameSet ) {
			m_Filenames.push_back( setRef );
		}
		LOG( Dev, info ) << "Created filename: " << m_Filenames.front();
	} else {
		m_Filenames.push_back( filename );
	}

	// get some image information
	//add some more properties
	imageType = _imageType;
	interpolationType = nn;
	majorTypeID = image.getMajorTypeID();
	majorTypeName = image.getMajorTypeName();
	majorTypeName = majorTypeName.substr( 0, majorTypeName.length() - 1 ).c_str();
	m_ImageSize = image.getSizeAsVector();
	LOG( Dev, verbose_info )  << "Fetched image of size " << m_ImageSize << " and type "
								<< image.getMajorTypeName() << ".";
	//copy the image into continuous memory space and assure consistent data type
	isRGB = !(data::ValuePtr<util::color24>::staticID != majorTypeID && data::ValuePtr<util::color48>::staticID != majorTypeID);
    const bool reserveZero = m_ZeroIsReserved && !isRGB && imageType == z_map;
	if( !isRGB ) {
		minMax = image.getMinMax();
		copyImageToVector<InternalImageType>( image, reserveZero );
	} else {
		copyImageToVector<InternalImageColorType>( image, reserveZero );
	}
	
	
	LOG_IF( m_ImageVector.empty(), Dev, error ) << "Size of image vector is 0!";

	if( m_ImageVector.size() != m_ImageSize[3] ) {
		LOG( Dev, error ) << "The number of timesteps (" << m_ImageSize[3]
							  << ") does not coincide with the number of volumes ("  << m_ImageVector.size() << ").";
		return false;
	}

	//create the chunk vector
	BOOST_FOREACH( std::vector< ImagePointerType >::const_reference pointerRef, m_ImageVector ) {
		m_ChunkVector.push_back( data::Chunk(  pointerRef, m_ImageSize[0], m_ImageSize[1], m_ImageSize[2] ) );
	}

	// if m_ZeroIsReserved is set we reserve a value (m_ReservedValue) in the internal image that indicates the true zero value in the origin image
	if( reserveZero ) {
		switch ( majorTypeID ) {
		case data::ValuePtr<bool>::staticID:
			_setTrueZero<bool>( image );
			break;
		case data::ValuePtr<int8_t>::staticID:
			_setTrueZero<int8_t>( image );
			break;
		case data::ValuePtr<uint8_t>::staticID:
			_setTrueZero<uint8_t>( image );
			break;
		case data::ValuePtr<int16_t>::staticID:
			_setTrueZero<int16_t>( image );
			break;
		case data::ValuePtr<uint16_t>::staticID:
			_setTrueZero<uint16_t>( image );
			break;
		case data::ValuePtr<int32_t>::staticID:
			_setTrueZero<int32_t>( image );
			break;
		case data::ValuePtr<uint32_t>::staticID:
			_setTrueZero<uint32_t>( image );
			break;
		case data::ValuePtr<int64_t>::staticID:
			_setTrueZero<int64_t>( image );
			break;
		case data::ValuePtr<uint64_t>::staticID:
			_setTrueZero<uint64_t>( image );
			break;
		case data::ValuePtr<float>::staticID:
			_setTrueZero<float>( image );
			break;
		case data::ValuePtr<double>::staticID:
			_setTrueZero<double>( image );
			break;
		}
	}
	LOG( Dev, verbose_info ) << "Spliced image to " << m_ImageVector.size() << " volumes.";

	//image seems to be ok...i guess



	if( imageType == z_map ) {
		lowerThreshold = 0;
		upperThreshold = 0;
		lut = std::string( "standard_zmap" );
	} else if( imageType == structural_image ) {
		if( !isRGB ) {
			lowerThreshold = minMax.first->as<double>() ;
			lowerThreshold = minMax.second->as<double>();
		}

		lut = std::string( "standard_grey_values" );
	}

	voxelSize = image.getPropertyAs<util::fvector4>( "voxelSize" );

	if( image.hasProperty( "voxelGap" ) ) {
		voxelSize += image.getPropertyAs<util::fvector4>( "voxelGap" );
	}


	voxelCoords = util::ivector4( m_ImageSize[0] / 2, m_ImageSize[1] / 2, m_ImageSize[2] / 2, 0 );
	physicalCoords = m_Image->getPhysicalCoordsFromIndex( voxelCoords );
	isVisible = true;
	opacity = 1.0;
	scaling = 1.0;
	offset = 0.0;
	colorMap = util::Singletons::get<color::Color, 10>().getColormapMap().at( lut );

	if( !isRGB ) {
		extent = fabs( minMax.second->as<double>() - minMax.first->as<double>() );

		m_PropMap.setPropertyAs<double>( "scalingMinValue", minMax.first->as<double>() );
		m_PropMap.setPropertyAs<double>( "scalingMaxValue", minMax.second->as<double>() );
	}

	alignedSize32 = get32BitAlignedSize( m_ImageSize );
	m_PropMap.setPropertyAs<bool>( "init", true );
	m_PropMap.setPropertyAs<util::slist>( "changedAttributes", util::slist() );
	updateOrientation();
	m_PropMap.setPropertyAs<util::fvector4>( "originalColumnVec", image.getPropertyAs<util::fvector4>( "columnVec" ) );
	m_PropMap.setPropertyAs<util::fvector4>( "originalRowVec", image.getPropertyAs<util::fvector4>( "rowVec" ) );
	m_PropMap.setPropertyAs<util::fvector4>( "originalSliceVec", image.getPropertyAs<util::fvector4>( "sliceVec" ) );
	m_PropMap.setPropertyAs<util::fvector4>( "originalIndexOrigin", image.getPropertyAs<util::fvector4>( "indexOrigin" ) );
	updateColorMap();
	logImageProps();
	return true;
}

void ImageHolder::addChangedAttribute( const std::string &attribute )
{
	util::slist attributes = m_PropMap.getPropertyAs<util::slist>( "changedAttributes" );
	attributes.push_back( attribute );
	m_PropMap.setPropertyAs<util::slist>( "changedAttributes", attributes );
}

bool ImageHolder::removeChangedAttribute( const std::string &attribute )
{
	util::slist attributes = m_PropMap.getPropertyAs<util::slist>( "changedAttributes" );
	util::slist::iterator iter = std::find( attributes.begin(), attributes.end(), attribute );

	if( iter == attributes.end() ) {
		return false;
	} else {
		attributes.erase( iter );
		m_PropMap.setPropertyAs<util::slist>( "changedAttributes", attributes );
		return true;
	}
}

void ImageHolder::updateHistogram()
{
	const size_t volume = getImageSize()[0] * getImageSize()[1] * getImageSize()[2];
	const double extent = getInternalExtent();

	boost::scoped_array<double> nHistogram ( new double[( size_t )extent + 1] );

	histogramVector.resize( getImageSize()[3] );
	histogramVectorWOZero.resize( getImageSize()[3] );

	for( unsigned short t = 0; t < getImageSize()[3]; t++ ) {
		histogramVector[t] = ( double * ) calloc( extent + 1, sizeof( double ) ) ;
		histogramVectorWOZero[t] = ( double * ) calloc( extent, sizeof( double ) );
		const InternalImageType *dataPtr = static_cast<InternalImageType *>( getImageVector()[t]->getRawAddress().get() );

		//create the histogram
		//      #pragma omp parallel for
		for( size_t i = 0; i < volume; i++ ) {
			histogramVector[t][dataPtr[i]]++;
		}

		//      #pragma omp parallel for
		for( size_t i = 0; i < volume; i++ ) {
			if( dataPtr[i] > 0 ) {
				histogramVectorWOZero[t][dataPtr[i] - 1]++;
			}
		}
	}
}



///calls isis::data::Image::updateOrientationMatrices() and sets latchedOrientation and orientation of the isis::viewer::ImageHolder
void ImageHolder::updateOrientation()
{
	m_Image->updateOrientationMatrices();
	latchedOrientation = getNormalizedImageOrientation();
	orientation = getImageOrientation();
	indexOrigin = getISISImage()->getPropertyAs<util::fvector4>("indexOrigin");
	rowVec = getISISImage()->getPropertyAs<util::fvector4>("rowVec");
	columnVec = getISISImage()->getPropertyAs<util::fvector4>("columnVec");
	sliveVec = getISISImage()->getPropertyAs<util::fvector4>("sliveVec");

}

void ImageHolder::checkVoxelCoords( util::ivector4 &vc )
{
	for( unsigned short i = 0; i < 4; i++ ) {
		vc[i] = vc[i] < 0 ? 0 : vc[i];
		vc[i] = vc[i] >= static_cast<int>( getImageSize()[i] ) ? static_cast<int>( getImageSize()[i] ) - 1 : vc[i]; //cast to avoid warning

	}
}

void ImageHolder::updateColorMap()
{
	if( !isRGB ) {
		util::Singletons::get<color::Color, 10>().adaptColorMapToImage( this );
	}
}

void ImageHolder::removeWidget( WidgetInterface *widget )
{
	std::list<WidgetInterface *>::iterator iter = std::find( m_WidgetList.begin(), m_WidgetList.end(), widget );

	if( iter != m_WidgetList.end() ) {
		m_WidgetList.erase( iter );
	}
}


double ImageHolder::getInternalExtent() const
{
	if( m_ZeroIsReserved ) {
		return std::numeric_limits<InternalImageType>::max() - std::numeric_limits<InternalImageType>::min();
	} else {
		return std::numeric_limits<InternalImageType>::max() - std::numeric_limits<InternalImageType>::min() + 1;
	}
}

void ImageHolder::logImageProps() const
{
	LOG( Dev, info ) << "The following image properties are for: " << getFileNames().front();
	LOG( Dev, info ) << "majorTypeID: " << majorTypeID;
	
}

void ImageHolder::setVoxel ( const size_t& first, const size_t& second, const size_t& third, const size_t& fourth, const double& value, bool sync )
{
	data::Chunk chunk = getISISImage()->getChunk( first, second, third, fourth, false );
	getChunkVector()[fourth].voxel<InternalImageType>( first, second, third ) = scalingToInternalType.second->as<double>() + value * scalingToInternalType.first->as<double>();
	if( sync ) {
		switch( chunk.getTypeID() ) {
			case data::ValuePtr<bool>::staticID:
				chunk.voxel<bool>(first, second, third, fourth) = static_cast<bool>( value );
				break;
			case data::ValuePtr<uint8_t>::staticID:
				chunk.voxel<uint8_t>(first, second, third, fourth) = static_cast<uint8_t>( value );
				break;
			case data::ValuePtr<int8_t>::staticID:
				chunk.voxel<int8_t>(first, second, third, fourth) = static_cast<int8_t>( value );
				break;
			case data::ValuePtr<uint16_t>::staticID:
				chunk.voxel<uint16_t>(first, second, third, fourth) = static_cast<uint16_t>( value );
				break;
			case data::ValuePtr<int16_t>::staticID:
				chunk.voxel<int16_t>(first, second, third, fourth) = static_cast<int16_t>( value );
				break;
			case data::ValuePtr<uint32_t>::staticID:
				chunk.voxel<uint32_t>(first, second, third, fourth) = static_cast<uint32_t>( value );
				break;
			case data::ValuePtr<int32_t>::staticID:
				chunk.voxel<int32_t>(first, second, third, fourth) = static_cast<int32_t>( value );
				break;
			case data::ValuePtr<uint64_t>::staticID:
				chunk.voxel<uint64_t>(first, second, third, fourth) = static_cast<uint64_t>( value );
				break;
			case data::ValuePtr<int64_t>::staticID:
				chunk.voxel<int64_t>(first, second, third, fourth) = static_cast<int64_t>( value );
				break;
			case data::ValuePtr<float>::staticID:
				chunk.voxel<float>(first, second, third, fourth) = static_cast<float>( value );
				break;
			case data::ValuePtr<double>::staticID:
				chunk.voxel<double>(first, second, third, fourth) = static_cast<double>( value );
				break;
			default:
				LOG( Runtime, error ) << "Tried to set voxel of chunk with type " << chunk.getTypeName() << " to double";
				LOG( Dev, error ) << "ImageHolder::setVoxel with type " << chunk.getTypeName();

		}
	}
}


}
} //end namespace

