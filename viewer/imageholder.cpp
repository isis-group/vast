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
 * Author: Erik Tuerke, tuerke@cbs.mpg.de
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
#include "memoryhandler.hpp"
#include <numeric>

namespace isis
{
namespace viewer
{

ImageHolder::ImageHolder()
	: m_ZeroIsReserved( true ),
	  m_ReservedValue( 0 ),
	  m_AmbiguousOrientation( false )
{}

boost::shared_ptr< const void > ImageHolder::getRawAdress ( size_t timestep ) const
{
	if( getImageProperties().isRGB ) {
		return m_ChunkVector.operator[]( timestep ).getValueArray<InternalImageColorType>().getRawAddress();
	} else {
		return m_ChunkVector.operator[]( timestep ).getValueArray<InternalImageType>().getRawAddress();
	}
}

boost::numeric::ublas::matrix< double > ImageHolder::calculateImageOrientation( bool transposed ) const
{
	boost::numeric::ublas::matrix<double> retMatrix = boost::numeric::ublas::zero_matrix<double>( 4, 4 );
	retMatrix( 3, 3 ) = 1;
	const util::fvector4 &rowVec = m_Image->getPropertyAs<util::fvector4>( "rowVec" );
	const util::fvector4 &columnVec = m_Image->getPropertyAs<util::fvector4>( "columnVec" );
	const util::fvector4 &sliceVec = m_Image->getPropertyAs<util::fvector4>( "sliceVec" );

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

boost::numeric::ublas::matrix< double > ImageHolder::calculateLatchedImageOrientation( bool transposed )
{
	boost::numeric::ublas::matrix<double> retMatrix = boost::numeric::ublas::zero_matrix<double>( 4, 4 );
	retMatrix( 3, 3 ) = 1;
	const util::fvector4 &rowVec = m_Image->getPropertyAs<util::fvector4>( "rowVec" );
	const util::fvector4 &columnVec = m_Image->getPropertyAs<util::fvector4>( "columnVec" );
	const util::fvector4 &sliceVec = m_Image->getPropertyAs<util::fvector4>( "sliceVec" );

	size_t rB = rowVec.getBiggestVecElemAbs();
	size_t cB = columnVec.getBiggestVecElemAbs();
	size_t sB = sliceVec.getBiggestVecElemAbs();

	//if image is rotated of 45 degrees
	m_AmbiguousOrientation = ( rB == cB ) || ( rB == sB ) || ( cB == sB );

	if( rB == cB ) {
		if( sB == 0 ) {
			rB = 1;
			cB = 2;
		} else if ( sB == 1 ) {
			rB = 2;
			cB = 0;
		} else if ( sB == 2 ) {
			rB = 1;
			rB = 0;
		}
	}

	if( rB == sB ) {
		if( cB == 0 ) {
			rB = 2;
			sB = 1;
		} else if ( cB == 1 ) {
			rB = 2;
			sB = 0;
		} else if ( cB == 2 ) {
			rB = 1;
			sB = 0;
		}
	}

	if( cB == sB ) {
		if( rB == 0 ) {
			cB = 1;
			sB = 2;
		} else if ( rB == 1 ) {
			cB = 2;
			sB = 0;
		} else if ( rB == 2 ) {
			cB = 1;
			sB = 0;
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


short unsigned int ImageHolder::getMajorTypeID() const
{
	if( getImageProperties().minMax.first->getTypeID() == getImageProperties().minMax.second->getTypeID() ) { // ok min and max are the same type - trivial case
		return getImageProperties().minMax.first->getTypeID() << 8; // btw: we do the shift, because min and max are Value - but we want the ID's ValuePtr
	} else if( getImageProperties().minMax.first->fitsInto( getImageProperties().minMax.second->getTypeID() ) ) { // if min fits into the type of max, use that
		return getImageProperties().minMax.second->getTypeID() << 8; //@todo maybe use a global static function here instead of a obscure shit operation
	} else if( getImageProperties().minMax.second->fitsInto( getImageProperties().minMax.first->getTypeID() ) ) { // if max fits into the type of min, use that
		return getImageProperties().minMax.first->getTypeID() << 8;
	} else {
		LOG( Runtime, error ) << "Sorry I dont know which datatype I should use. (" << getImageProperties().minMax.first->getTypeName()
							  << " or " << getImageProperties().minMax.second->getTypeName() << ")";
		std::stringstream o;
		o << "Type selection failed. Range was: " << getImageProperties().minMax;
	}

	return 0;

}

void ImageHolder::collectImageInfo()
{
	getImageProperties().minMax = getISISImage()->getMinMax();
	getImageProperties().majorTypeID = getMajorTypeID();
	getImageProperties().isRGB = ( data::ValueArray<util::color24>::staticID == getImageProperties().majorTypeID || data::ValueArray<util::color48>::staticID == getImageProperties().majorTypeID );
	getImageProperties().majorTypeName = isis::util::getTypeMap( false, true ).at( getImageProperties().majorTypeID );
	getImageProperties().majorTypeName = getImageProperties().majorTypeName.substr( 0, getImageProperties().majorTypeName.length() - 1 ).c_str();

	switch( getImageProperties().majorTypeID ) {
	case data::ValueArray<bool>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<bool>( *m_Image ) ) );
		break;
	case data::ValueArray<uint8_t>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<uint8_t>( *m_Image ) ) );
		break;
	case data::ValueArray<int8_t>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<int8_t>( *m_Image ) ) );
		break;
	case data::ValueArray<uint16_t>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<uint16_t>( *m_Image ) ) );
		break;
	case data::ValueArray<int16_t>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<int16_t>( *m_Image ) ) );
		break;
	case data::ValueArray<uint32_t>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<uint32_t>( *m_Image ) ) );
		break;
	case data::ValueArray<int32_t>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<int32_t>( *m_Image ) ) );
		break;
	case data::ValueArray<uint64_t>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<uint64_t>( *m_Image ) ) );
		break;
	case data::ValueArray<int64_t>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<int64_t>( *m_Image ) ) );
		break;
	case data::ValueArray<float>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<float>( *m_Image ) ) );
		break;
	case data::ValueArray<double>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<double>( *m_Image ) ) );
		break;
	case data::ValueArray<util::color24>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<util::color24>( *m_Image ) ) );
		break;
	case data::ValueArray<util::color48>::staticID:
		m_TypedImage.reset( new data::Image( data::TypedImage<util::color48>( *m_Image ) ) );
		break;
	}
}


boost::shared_ptr< data::Image > ImageHolder::getISISImage ( bool typed ) const
{
	if( typed ) {
		return m_TypedImage;
	} else {
		return m_Image;
	}
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
	getImageProperties().filePath = filename;
	boost::filesystem::path p(filename);
	getImageProperties().fileName = p.filename();

	// get some image information
	//add some more properties
	getImageProperties().imageType = _imageType;
	getImageProperties().interpolationType = nn;
	collectImageInfo();
	m_ImageSize = image.getSizeAsVector();
	LOG( Dev, verbose_info )  << "Fetched image of size " << m_ImageSize << " and type "
							  << image.getMajorTypeName() << ".";
	//copy the image into continuous memory space and assure consistent data type
	m_ZeroIsReserved  = !getImageProperties().isRGB && getImageProperties().imageType == statistical_image;
	synchronize( m_ZeroIsReserved );

	LOG_IF( m_ChunkVector.empty(), Dev, error ) << "Size of chunk vector is 0!";

	if( m_ChunkVector.size() != m_ImageSize[3] ) {
		LOG( Dev, error ) << "The number of timesteps (" << m_ImageSize[3]
						  << ") does not coincide with the number of volumes ("  << m_ChunkVector.size() << ").";
		return false;
	}

	LOG( Dev, verbose_info ) << "Spliced image to " << m_ChunkVector.size() << " volumes.";

	//image seems to be ok...i guess



	if( getImageProperties().imageType == statistical_image ) {
		getImageProperties().lowerThreshold = 0;
		getImageProperties().upperThreshold = 0;
		getImageProperties().lut = std::string( "standard_zmap" );
	} else if( getImageProperties().imageType == structural_image ) {
		if( !getImageProperties().isRGB ) {
			getImageProperties().lowerThreshold = getImageProperties().minMax.first->as<double>() ;
			getImageProperties().lowerThreshold = getImageProperties().minMax.second->as<double>();
		}

		getImageProperties().lut = std::string( "standard_grey_values" );
	}

	getImageProperties().voxelSize = image.getPropertyAs<util::fvector4>( "voxelSize" );

	if( image.hasProperty( "voxelGap" ) ) {
		getImageProperties().voxelSize += image.getPropertyAs<util::fvector4>( "voxelGap" );
	}



	getImageProperties().isVisible = true;
	getImageProperties().opacity = 1.0;
	getImageProperties().scaling = 1.0;
	getImageProperties().offset = 0.0;
	getImageProperties().colorMap = util::Singletons::get<color::Color, 10>().getColormapMap().at( getImageProperties().lut );

	if( !getImageProperties().isRGB ) {
		getImageProperties().extent = fabs( getImageProperties().minMax.second->as<double>() - getImageProperties().minMax.first->as<double>() );
		updateHistogram();
		m_PropMap.setPropertyAs<double>( "scalingMinValue", getImageProperties().minMax.first->as<double>() );
		m_PropMap.setPropertyAs<double>( "scalingMaxValue", getImageProperties().minMax.second->as<double>() );
	}

	getImageProperties().alphaMap.resize( 256 );
	getImageProperties().alignedSize32 = MemoryHandler::get32BitAlignedSize( m_ImageSize );
	m_PropMap.setPropertyAs<bool>( "init", true );
	m_PropMap.setPropertyAs<util::slist>( "changedAttributes", util::slist() );
	updateOrientation();
	getImageProperties().voxelCoords = util::ivector4( m_ImageSize[0] / 2, m_ImageSize[1] / 2, m_ImageSize[2] / 2, 0 );
	getImageProperties().physicalCoords = m_Image->getPhysicalCoordsFromIndex( getImageProperties().voxelCoords );
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

	getImageProperties().histogramVector.resize( getImageSize()[3] );
	getImageProperties().histogramVectorWOZero.resize( getImageSize()[3] );

	for( unsigned short t = 0; t < getImageSize()[3]; t++ ) {
		getImageProperties().histogramVector[t] = ( double * ) calloc( extent + 1, sizeof( double ) ) ;
		getImageProperties().histogramVectorWOZero[t] = ( double * ) calloc( extent, sizeof( double ) );
		const InternalImageType *dataPtr = boost::shared_static_cast<const InternalImageType>( getRawAdress( t ) ).get();
		//create the histogram
#pragma omp parallel for

		for( size_t i = 0; i < volume; i++ ) {
			getImageProperties().histogramVector[t][dataPtr[i]]++;

			if( dataPtr[i] > 0 ) {
				getImageProperties().histogramVectorWOZero[t][dataPtr[i] - 1]++;
			}
		}
	}
}



///calls isis::data::Image::updateOrientationMatrices() and sets latchedOrientation and orientation of the isis::viewer::ImageHolder
void ImageHolder::updateOrientation()
{
	m_Image->updateOrientationMatrices();
	getImageProperties().latchedOrientation = calculateLatchedImageOrientation();
	getImageProperties().orientation = calculateImageOrientation();
	getImageProperties().indexOrigin = getISISImage()->getPropertyAs<util::fvector4>( "indexOrigin" );
	getImageProperties().rowVec = getISISImage()->getPropertyAs<util::fvector4>( "rowVec" );
	getImageProperties().columnVec = getISISImage()->getPropertyAs<util::fvector4>( "columnVec" );
	getImageProperties().sliceVec = getISISImage()->getPropertyAs<util::fvector4>( "sliceVec" );
	getImageProperties().voxelSize = getISISImage()->getPropertyAs<util::fvector4>( "voxelSize" ) + ( getISISImage()->hasProperty( "voxelGap" ) ? getISISImage()->getPropertyAs<util::fvector4>( "voxelGap" ) : util::fvector4() );

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
	if( !getImageProperties().isRGB ) {
		getImageProperties().alphaMap.fill( 1 );
		util::Singletons::get<color::Color, 10>().adaptColorMapToImage( this );
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
	LOG( Dev, info ) << "The following image properties are for: " << getImageProperties().fileName;
	LOG( Dev, info ) << "majorTypeID: " << getImageProperties().majorTypeID;

}

void ImageHolder::setVoxel ( const size_t &first, const size_t &second, const size_t &third, const size_t &fourth, const double &value, bool sync )
{
	data::Chunk chunk = getISISImage()->getChunk( first, second, third, fourth, false );
	getChunkVector()[fourth].voxel<InternalImageType>( first, second, third )
	= getImageProperties().scalingToInternalType.second->as<double>() + value * getImageProperties().scalingToInternalType.first->as<double>();

	if( sync ) {
		switch( chunk.getTypeID() ) {
		case data::ValueArray<bool>::staticID:
			chunk.voxel<bool>( first, second, third, fourth ) = static_cast<bool>( value );
			break;
		case data::ValueArray<uint8_t>::staticID:
			chunk.voxel<uint8_t>( first, second, third, fourth ) = static_cast<uint8_t>( value );
			break;
		case data::ValueArray<int8_t>::staticID:
			chunk.voxel<int8_t>( first, second, third, fourth ) = static_cast<int8_t>( value );
			break;
		case data::ValueArray<uint16_t>::staticID:
			chunk.voxel<uint16_t>( first, second, third, fourth ) = static_cast<uint16_t>( value );
			break;
		case data::ValueArray<int16_t>::staticID:
			chunk.voxel<int16_t>( first, second, third, fourth ) = static_cast<int16_t>( value );
			break;
		case data::ValueArray<uint32_t>::staticID:
			chunk.voxel<uint32_t>( first, second, third, fourth ) = static_cast<uint32_t>( value );
			break;
		case data::ValueArray<int32_t>::staticID:
			chunk.voxel<int32_t>( first, second, third, fourth ) = static_cast<int32_t>( value );
			break;
		case data::ValueArray<uint64_t>::staticID:
			chunk.voxel<uint64_t>( first, second, third, fourth ) = static_cast<uint64_t>( value );
			break;
		case data::ValueArray<int64_t>::staticID:
			chunk.voxel<int64_t>( first, second, third, fourth ) = static_cast<int64_t>( value );
			break;
		case data::ValueArray<float>::staticID:
			chunk.voxel<float>( first, second, third, fourth ) = static_cast<float>( value );
			break;
		case data::ValueArray<double>::staticID:
			chunk.voxel<double>( first, second, third, fourth ) = static_cast<double>( value );
			break;
		default:
			LOG( Runtime, error ) << "Tried to set voxel of chunk with type " << chunk.getTypeName() << " to double";
			LOG( Dev, error ) << "ImageHolder::setVoxel with type " << chunk.getTypeName();
		}
	}
}

void ImageHolder::synchronize ( bool isReserved )
{
	m_ZeroIsReserved = isReserved;

	if( getImageProperties().isRGB ) {
		copyImageToVector<InternalImageColorType>( *getISISImage(), isReserved );
	} else {
		copyImageToVector<InternalImageType>( *getISISImage(), isReserved );
	}

	if( m_ZeroIsReserved ) {
		switch ( getImageProperties().majorTypeID ) {
		case data::ValueArray<bool>::staticID:
			_setTrueZero<bool>( *getISISImage() );
			break;
		case data::ValueArray<int8_t>::staticID:
			_setTrueZero<int8_t>( *getISISImage() );
			break;
		case data::ValueArray<uint8_t>::staticID:
			_setTrueZero<uint8_t>( *getISISImage() );
			break;
		case data::ValueArray<int16_t>::staticID:
			_setTrueZero<int16_t>( *getISISImage() );
			break;
		case data::ValueArray<uint16_t>::staticID:
			_setTrueZero<uint16_t>( *getISISImage() );
			break;
		case data::ValueArray<int32_t>::staticID:
			_setTrueZero<int32_t>( *getISISImage() );
			break;
		case data::ValueArray<uint32_t>::staticID:
			_setTrueZero<uint32_t>( *getISISImage() );
			break;
		case data::ValueArray<int64_t>::staticID:
			_setTrueZero<int64_t>( *getISISImage() );
			break;
		case data::ValueArray<uint64_t>::staticID:
			_setTrueZero<uint64_t>( *getISISImage() );
			break;
		case data::ValueArray<float>::staticID:
			_setTrueZero<float>( *getISISImage() );
			break;
		case data::ValueArray<double>::staticID:
			_setTrueZero<double>( *getISISImage() );
			break;
		}
	}
}


}
} //end namespace

