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
#include "nativeimageops.hpp"
#include <numeric>

namespace isis
{
namespace viewer
{

namespace _internal
{
void __Image::mapPhysicalToIndex ( const float *physicalCoords, int32_t *index )
{
	float vec[3] = { physicalCoords[0] - m_Offset[0], physicalCoords[1] - m_Offset[1], physicalCoords[2] - m_Offset[2] };
	index[0] = ( vec[0] * m_RowVecInv[0] + vec[1] * m_ColumnVecInv[0] + vec[2] * m_SliceVecInv[0] + 0.5 );
	index[1] = ( vec[0] * m_RowVecInv[1] + vec[1] * m_ColumnVecInv[1] + vec[2] * m_SliceVecInv[1] + 0.5 );
	index[2] = ( vec[0] * m_RowVecInv[2] + vec[1] * m_ColumnVecInv[2] + vec[2] * m_SliceVecInv[2] + 0.5 );

}
bool __Image::checkVoxel ( const int32_t *coords )
{
	if( ( coords[0] < 0 ) || ( coords[0] >= imageSize[0] ) ) return false;

	if( ( coords[1] < 0 ) || ( coords[1] >= imageSize[1] ) ) return false;

	if( ( coords[2] < 0 ) || ( coords[2] >= imageSize[2] ) ) return false;

	return true;
}

}

ImageHolder::ImageHolder()
	:  m_AmbiguousOrientation( false )
{}

boost::shared_ptr< const void > ImageHolder::getRawAdress ( size_t timestep ) const
{
	if( getImageProperties().isRGB ) {
		return m_VolumeVector.operator[]( timestep ).getValueArray<InternalImageColorType>().getRawAddress();
	} else {
		return m_VolumeVector.operator[]( timestep ).getValueArray<InternalImageType>().getRawAddress();
	}
}

util::Matrix3x3<float> ImageHolder::calculateImageOrientation( bool transposed ) const
{

	util::Matrix3x3<float> retMatrix (  m_Image->getPropertyAs<util::fvector3>( "rowVec" ),
										m_Image->getPropertyAs<util::fvector3>( "columnVec" ),
										m_Image->getPropertyAs<util::fvector3>( "sliceVec" ) );

	if( transposed ) {
		return retMatrix;
	}

	return retMatrix.transpose(); // has to be transposed!!!!!!!!!!
}

util::Matrix3x3<float> ImageHolder::calculateLatchedImageOrientation( bool transposed )
{
	util::Matrix3x3<float> retMatrix;
	retMatrix.fill( 0 );
	const util::fvector3 &rowVec = m_Image->getPropertyAs<util::fvector3>( "rowVec" );
	const util::fvector3 &columnVec = m_Image->getPropertyAs<util::fvector3>( "columnVec" );
	const util::fvector3 &sliceVec = m_Image->getPropertyAs<util::fvector3>( "sliceVec" );

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
			rB = 0;
			cB = 1;
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

	retMatrix.elem( 0, rB ) = rowVec[rB] < 0 ? -1 : 1;
	retMatrix.elem( 1, cB ) =  columnVec[cB] < 0 ? -1 : 1;
	retMatrix.elem( 2, sB ) = sliceVec[sB] < 0 ? -1 : 1;

	if( transposed ) {
		return retMatrix.transpose();
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
	getImageProperties().zeroIsReserved = getImageProperties().zeroIsReserved || (
			getImageProperties().imageType == statistical_image && getImageProperties().minMax.first->as<double>() < 0 && !getImageProperties().isRGB );

	if( !getImageProperties().isRGB ) {
		getImageProperties().extent = fabs( getImageProperties().minMax.second->as<double>() - getImageProperties().minMax.first->as<double>() );
		getImageProperties().scalingMinMax.first = getImageProperties().minMax.first->as<double>();
		getImageProperties().scalingMinMax.second = getImageProperties().minMax.second->as<double>();
	}

	getImageProperties().majorTypeName = isis::util::getTypeMap( false, true ).at( getImageProperties().majorTypeID );
	getImageProperties().majorTypeName = getImageProperties().majorTypeName.substr( 0, getImageProperties().majorTypeName.length() - 1 ).c_str();
}


boost::shared_ptr< _internal::__Image > ImageHolder::getISISImage() const
{
	return m_Image;
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
	getImageProperties().timestep = 0;
	m_Image.reset( new _internal::__Image( image ) );
	getImageProperties().filePath = filename;
	getImageProperties().zeroIsReserved = false;
	boost::filesystem::path p( filename );
	getImageProperties().fileName = p.filename().string();
	// get some image information
	//add some more properties
	getImageProperties().imageType = _imageType;
	getImageProperties().interpolationType = nn;
	m_ImageSize = image.getSizeAsVector();
	LOG( Dev, verbose_info )  << "Fetched image of size " << m_ImageSize << " and type "
							  << image.getMajorTypeName() << ".";

	//copy the image into continuous memory space and assure consistent data type
	synchronize( );

	LOG_IF( m_VolumeVector.empty(), Dev, error ) << "Size of chunk vector is 0!";

	if( m_VolumeVector.size() != m_ImageSize[3] ) {
		LOG( Dev, error ) << "The number of timesteps (" << m_ImageSize[3]
						  << ") does not coincide with the number of volumes ("  << m_VolumeVector.size() << ").";
		return false;
	}

	LOG( Dev, verbose_info ) << "Spliced image to " << m_VolumeVector.size() << " volumes.";

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

	getImageProperties().isVisible = true;
	getImageProperties().opacity = 1.0;
	getImageProperties().scaling = 1.0;
	getImageProperties().offset = 0.0;
	getImageProperties().colorMap = util::Singletons::get<color::Color, 10>().getColormapMap().at( getImageProperties().lut );

	getImageProperties().alphaMap.resize( 256 );
	getImageProperties().alignedSize32 = MemoryHandler::get32BitAlignedSize( m_ImageSize );
	m_PropMap.setPropertyAs<bool>( "init", true );
	m_PropMap.setPropertyAs<util::slist>( "changedAttributes", util::slist() );
	updateOrientation();
	getImageProperties().voxelCoords = util::ivector4( m_ImageSize[0] / 2, m_ImageSize[1] / 2, m_ImageSize[2] / 2, 0 );
	getImageProperties().physicalCoords = m_Image->getPhysicalCoordsFromIndex( getImageProperties().voxelCoords );
	m_PropMap.setPropertyAs<util::fvector3>( "originalColumnVec", image.getPropertyAs<util::fvector3>( "columnVec" ) );
	m_PropMap.setPropertyAs<util::fvector3>( "originalRowVec", image.getPropertyAs<util::fvector3>( "rowVec" ) );
	m_PropMap.setPropertyAs<util::fvector3>( "originalSliceVec", image.getPropertyAs<util::fvector3>( "sliceVec" ) );
	m_PropMap.setPropertyAs<util::fvector3>( "originalIndexOrigin", image.getPropertyAs<util::fvector3>( "indexOrigin" ) );
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


///calls isis::data::Image::updateOrientationMatrices() and sets latchedOrientation and orientation of the isis::viewer::ImageHolder
void ImageHolder::updateOrientation()
{
	m_Image->updateOrientationMatrices();
	getImageProperties().latchedOrientation = calculateLatchedImageOrientation();
	getImageProperties().orientation = calculateImageOrientation();
	getImageProperties().indexOrigin = getISISImage()->getPropertyAs<util::fvector3>( "indexOrigin" );
	getImageProperties().rowVec = getISISImage()->getPropertyAs<util::fvector3>( "rowVec" );
	getImageProperties().columnVec = getISISImage()->getPropertyAs<util::fvector3>( "columnVec" );
	getImageProperties().sliceVec = getISISImage()->getPropertyAs<util::fvector3>( "sliceVec" );
	m_ImageProperties.voxelSize = getISISImage()->getPropertyAs<util::fvector3>( "voxelSize" );

	if( getISISImage()->hasProperty( "voxelGap" ) ) {
		getImageProperties().voxelSize += getISISImage()->getPropertyAs<util::fvector3>( "voxelGap" );
	}

	m_ImageProperties.boundingBox = geometrical::getPhysicalBoundingBox( ImageHolder::Pointer( new ImageHolder( *this ) ) );
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
	if( getImageProperties().zeroIsReserved ) {
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
	getVolumeVector()[fourth].voxel<InternalImageType>( first, second, third )
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


void ImageHolder::synchronize ()
{
	collectImageInfo();

	if( getImageProperties().isRGB ) {
		copyImageToVector<InternalImageColorType>( *getISISImage() );
	} else {
		copyImageToVector<InternalImageType>( *getISISImage() );
	}
}

void ImageHolder::phyisicalCoordsChanged ( const util::fvector3 &physicalCoords )
{
	getImageProperties().physicalCoords = physicalCoords;
	getImageProperties().trueVoxelCoords = getISISImage()->getIndexFromPhysicalCoords( physicalCoords );
	util::ivector4 correctedVoxelCoords =  getImageProperties().trueVoxelCoords;
	correctVoxelCoords<4>( correctedVoxelCoords );
	getImageProperties().voxelCoords = correctedVoxelCoords;
}

void ImageHolder::voxelCoordsChanged ( const util::ivector4 &voxelCoords )
{
	getImageProperties().voxelCoords = voxelCoords;
	getImageProperties().physicalCoords = getISISImage()->getPhysicalCoordsFromIndex( voxelCoords );
}

void ImageHolder::timestepChanged ( const size_t &timestep )
{
	getImageProperties().timestep = timestep;

	if( getImageProperties().timestep >= getISISImage()->getSizeAsVector()[3] ) {
		getImageProperties().timestep = getISISImage()->getSizeAsVector()[3] - 1;
	}
}


}
} //end namespace

