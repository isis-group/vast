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
#include <boost/numeric/ublas/matrix.hpp>
#include <isis/math/transform.hpp>

namespace isis::viewer
{

namespace _internal
{
bool __Image::checkVoxel ( const int32_t *coords )
{
	if( ( coords[0] < 0 ) || ( coords[0] >= imageSize[0] ) ) return false;

	if( ( coords[1] < 0 ) || ( coords[1] >= imageSize[1] ) ) return false;

	if( ( coords[2] < 0 ) || ( coords[2] >= imageSize[2] ) ) return false;

	return true;
}

}

ImageHolder::ImageHolder()	:  m_AmbiguousOrientation( false )
{}

std::shared_ptr< const void > ImageHolder::getRawAdress ( size_t timestep ) const
{
	if( getImageProperties().isRGB ) {
		return m_VolumeVector.operator[]( timestep ).getRawAddress();
	} else {
		return m_VolumeVector.operator[]( timestep ).getRawAddress();
	}
}

util::Matrix3x3<float> ImageHolder::calculateImageOrientation( bool transposed ) const
{

	util::Matrix3x3<float> retMatrix {
		m_Image->getValueAs<util::fvector3>( "rowVec" ),
		m_Image->getValueAs<util::fvector3>( "columnVec" ),
		m_Image->getValueAs<util::fvector3>( "sliceVec" )
	};

	if( transposed ) {
		return retMatrix;
	}

	return util::transpose(retMatrix); // has to be transposed!!!!!!!!!!
}

util::Matrix3x3<float> ImageHolder::calculateLatchedImageOrientation( bool transposed )
{
	util::Matrix3x3<float> retMatrix;
	retMatrix.fill( {0,0,0} );
	const util::fvector3 &rowVec = m_Image->getValueAs<util::fvector3>( "rowVec" );
	const util::fvector3 &columnVec = m_Image->getValueAs<util::fvector3>( "columnVec" );
	const util::fvector3 &sliceVec = m_Image->getValueAs<util::fvector3>( "sliceVec" );

	size_t rB = math::getBiggestVecElemAbs(rowVec);
	size_t cB = math::getBiggestVecElemAbs(columnVec);
	size_t sB = math::getBiggestVecElemAbs(sliceVec);

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

	retMatrix[0][rB] = rowVec[rB] < 0 ? -1 : 1;
	retMatrix[1][cB] =  columnVec[cB] < 0 ? -1 : 1;
	retMatrix[2][sB] = sliceVec[sB] < 0 ? -1 : 1;

	if( transposed ) {
		return util::transpose(retMatrix);
	}

	return retMatrix;
}


short unsigned int ImageHolder::getMajorTypeID() const
{
	if( getImageProperties().minMax.first.typeID() == getImageProperties().minMax.second.typeID() ) { // ok min and max are the same type - trivial case
		return getImageProperties().minMax.first.typeID(); // btw: we do the shift, because min and max are Value - but we want the ID's ValuePtr
	} else if( getImageProperties().minMax.first.fitsInto( getImageProperties().minMax.second.typeID() ) ) { // if min fits into the type of max, use that
		return getImageProperties().minMax.second.typeID(); //@todo maybe use a global static function here instead of a obscure shit operation
	} else if( getImageProperties().minMax.second.fitsInto( getImageProperties().minMax.first.typeID() ) ) { // if max fits into the type of min, use that
		return getImageProperties().minMax.first.typeID();
	} else {
		LOG( Runtime, error ) << "Sorry I dont know which datatype I should use. (" << getImageProperties().minMax.first.typeName()
							  << " or " << getImageProperties().minMax.second.typeName() << ")";
		std::stringstream o;
		o << "Type selection failed. Range was: " << getImageProperties().minMax;
	}

	return 0;

}

void ImageHolder::collectImageInfo()
{
	getImageProperties().minMax = getISISImage()->getMinMax();
	getImageProperties().majorTypeID = getMajorTypeID();
	getImageProperties().isRGB = ( util::typeID<util::color24>() == getImageProperties().majorTypeID || util::typeID<util::color48>() == getImageProperties().majorTypeID );
	getImageProperties().zeroIsReserved = getImageProperties().zeroIsReserved || (
			getImageProperties().imageType == statistical_image && getImageProperties().minMax.first.as<double>() < 0 && !getImageProperties().isRGB );

	if( !getImageProperties().isRGB ) {
		getImageProperties().extent = fabs( getImageProperties().minMax.second.as<double>() - getImageProperties().minMax.first.as<double>() );
		getImageProperties().scalingMinMax.first = getImageProperties().minMax.first.as<double>();
		getImageProperties().scalingMinMax.second = getImageProperties().minMax.second.as<double>();
	}

	getImageProperties().majorTypeName = isis::util::getTypeMap( true ).at( getImageProperties().majorTypeID );
	getImageProperties().majorTypeName = getImageProperties().majorTypeName.substr( 0, getImageProperties().majorTypeName.length() - 1 ).c_str();
}


std::shared_ptr< _internal::__Image > ImageHolder::getISISImage() const
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
	std::filesystem::path p( filename );
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
			getImageProperties().lowerThreshold = getImageProperties().minMax.first.as<double>() ;
			getImageProperties().lowerThreshold = getImageProperties().minMax.second.as<double>();
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
	m_PropMap.setValueAs<bool>( "init", true );
	m_PropMap.setValueAs<util::slist>( "changedAttributes", util::slist() );
	updateOrientation();
	getImageProperties().voxelCoords = { m_ImageSize[0] / 2, m_ImageSize[1] / 2, m_ImageSize[2] / 2, 0 };
//@todo implement me	getImageProperties().physicalCoords = m_Image->getPhysicalCoordsFromIndex( getImageProperties().voxelCoords );
	m_PropMap.setValueAs<util::fvector3>( "originalColumnVec", image.getValueAs<util::fvector3>( "columnVec" ) );
	m_PropMap.setValueAs<util::fvector3>( "originalRowVec", image.getValueAs<util::fvector3>( "rowVec" ) );
	m_PropMap.setValueAs<util::fvector3>( "originalSliceVec", image.getValueAs<util::fvector3>( "sliceVec" ) );
	m_PropMap.setValueAs<util::fvector3>( "originalIndexOrigin", image.getValueAs<util::fvector3>( "indexOrigin" ) );
	updateColorMap();
	logImageProps();
	return true;
}

void ImageHolder::addChangedAttribute( const std::string &attribute )
{
	util::slist attributes = m_PropMap.getValueAs<util::slist>( "changedAttributes" );
	attributes.push_back( attribute );
	m_PropMap.setValueAs<util::slist>( "changedAttributes", attributes );
}

bool ImageHolder::removeChangedAttribute( const std::string &attribute )
{
	util::slist attributes = m_PropMap.getValueAs<util::slist>( "changedAttributes" );
	util::slist::iterator iter = std::find( attributes.begin(), attributes.end(), attribute );

	if( iter == attributes.end() ) {
		return false;
	} else {
		attributes.erase( iter );
		m_PropMap.setValueAs<util::slist>( "changedAttributes", attributes );
		return true;
	}
}


///calls isis::data::Image::updateOrientationMatrices() and sets latchedOrientation and orientation of the isis::viewer::ImageHolder
void ImageHolder::updateOrientation()
{
//@todo implement me	m_Image->updateOrientationMatrices();
	getImageProperties().latchedOrientation = calculateLatchedImageOrientation();
	getImageProperties().orientation = calculateImageOrientation();
	getImageProperties().indexOrigin = getISISImage()->getValueAs<util::fvector3>( "indexOrigin" );
	getImageProperties().rowVec = getISISImage()->getValueAs<util::fvector3>( "rowVec" );
	getImageProperties().columnVec = getISISImage()->getValueAs<util::fvector3>( "columnVec" );
	getImageProperties().sliceVec = getISISImage()->getValueAs<util::fvector3>( "sliceVec" );
	m_ImageProperties.voxelSize = getISISImage()->getValueAs<util::fvector3>( "voxelSize" );

	if( getISISImage()->hasProperty( "voxelGap" ) ) {
		getImageProperties().voxelSize += getISISImage()->getValueAs<util::fvector3>( "voxelGap" );
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
	= getImageProperties().scalingToInternalType.offset.as<double>() + value * getImageProperties().scalingToInternalType.scale.as<double>();

	if( sync ) {
		switch( chunk.getTypeID() ) {
			case util::typeID<bool>():
			chunk.voxel<bool>( first, second, third, fourth ) = static_cast<bool>( value );
			break;
		case util::typeID<uint8_t>():
			chunk.voxel<uint8_t>( first, second, third, fourth ) = static_cast<uint8_t>( value );
			break;
		case util::typeID<int8_t>():
			chunk.voxel<int8_t>( first, second, third, fourth ) = static_cast<int8_t>( value );
			break;
		case util::typeID<uint16_t>():
			chunk.voxel<uint16_t>( first, second, third, fourth ) = static_cast<uint16_t>( value );
			break;
		case util::typeID<int16_t>():
			chunk.voxel<int16_t>( first, second, third, fourth ) = static_cast<int16_t>( value );
			break;
		case util::typeID<uint32_t>():
			chunk.voxel<uint32_t>( first, second, third, fourth ) = static_cast<uint32_t>( value );
			break;
		case util::typeID<int32_t>():
			chunk.voxel<int32_t>( first, second, third, fourth ) = static_cast<int32_t>( value );
			break;
		case util::typeID<uint64_t>():
			chunk.voxel<uint64_t>( first, second, third, fourth ) = static_cast<uint64_t>( value );
			break;
		case util::typeID<int64_t>():
			chunk.voxel<int64_t>( first, second, third, fourth ) = static_cast<int64_t>( value );
			break;
		case util::typeID<float>():
			chunk.voxel<float>( first, second, third, fourth ) = static_cast<float>( value );
			break;
		case util::typeID<double>():
			chunk.voxel<double>( first, second, third, fourth ) = static_cast<double>( value );
			break;
		default:
			LOG( Runtime, error ) << "Tried to set voxel of chunk with type " << chunk.typeName() << " to double";
			LOG( Dev, error ) << "ImageHolder::setVoxel with type " << chunk.typeName();
		}
	}
}


void ImageHolder::synchronize ()
{
	collectImageInfo();

	if( getImageProperties().isRGB ) {
		copyImageToVector<InternalImageColorType>( *std::static_pointer_cast<data::Image>(getISISImage()) );
	} else {
		copyImageToVector<InternalImageType>( *std::static_pointer_cast<data::Image>(getISISImage()) );
	}
}

void ImageHolder::phyisicalCoordsChanged ( const util::fvector3 &physicalCoords )
{
	getImageProperties().physicalCoords = physicalCoords;
//@todo implement me	getImageProperties().trueVoxelCoords = getISISImage()->getIndexFromPhysicalCoords( physicalCoords );
	auto correctedVoxelCoords =  getImageProperties().trueVoxelCoords;
	correctVoxelCoords<4>( correctedVoxelCoords );
	getImageProperties().voxelCoords = correctedVoxelCoords;
}

void ImageHolder::voxelCoordsChanged (const util::vector4<size_t> &voxelCoords )
{
	getImageProperties().voxelCoords = voxelCoords;
	getImageProperties().physicalCoords = getPhysicalCoordsFromIndex( voxelCoords );
}

void ImageHolder::timestepChanged ( const size_t &timestep )
{
	getImageProperties().timestep = timestep;

	if( getImageProperties().timestep >= getISISImage()->getSizeAsVector()[3] ) {
		getImageProperties().timestep = getISISImage()->getSizeAsVector()[3] - 1;
	}
}
bool ImageHolder::updateOrientationMatrices()//@todo this need to be called somewhere
{
	const auto rowVec = m_Image->getValueAs<util::fvector3>( "rowVec" );
	const auto columnVec = m_Image->getValueAs<util::fvector3>( "columnVec" );
	const auto sliceVec = m_Image->getValueAs<util::fvector3>( "sliceVec" );
	m_Offset = m_Image->getValueAs<util::fvector3>( "indexOrigin" );
	auto spacing = m_Image->getValueAs<util::fvector3>( "voxelSize" );
	if( m_Image->hasProperty( "voxelGap" ))
		spacing += m_Image->getValueAs<util::fvector3>( "voxelGap" );

	for( auto &s : spacing ) {
		if( s == 0 ) s = 1;
	}

	m_RowVec = rowVec * spacing[0];
	m_ColumnVec = columnVec * spacing[1];
	m_SliceVec = sliceVec * spacing[2];
	LOG( Dev, verbose_info ) << "Created orientation matrix: ";
	LOG( Dev, verbose_info ) << "[ " << m_RowVec[0] << " " << m_ColumnVec[0] << " " << m_SliceVec[0] << " ] + " << m_Offset[0];
	LOG( Dev, verbose_info ) << "[ " << m_RowVec[1] << " " << m_ColumnVec[1] << " " << m_SliceVec[1] << " ] + " << m_Offset[1];
	LOG( Dev, verbose_info ) << "[ " << m_RowVec[2] << " " << m_ColumnVec[2] << " " << m_SliceVec[2] << " ] + " << m_Offset[2];

	//for inversion of the orientation we use boost::ublas
	using namespace boost::numeric::ublas;
	bool invertible;
	const util::Matrix3x3<float> orientation{m_RowVec,m_ColumnVec,m_SliceVec};
	const util::Matrix3x3<float> inverse = math::inverseMatrix(orientation,invertible);

	if( ! invertible ) {
		LOG( Runtime, error ) << "Could not create the inverse of the orientation matrix!";
		return false;
	};
	m_RowVecInv = inverse[0];
	m_ColumnVecInv = inverse[1];
	m_SliceVecInv = inverse[2];

	LOG( Dev, verbose_info ) << "Created transposed orientation matrix: ";
	LOG( Dev, verbose_info ) << "[ " << m_RowVecInv[0] << " " << m_ColumnVecInv[0] << " " << m_SliceVecInv[0] << " ] + " << m_Offset[0];
	LOG( Dev, verbose_info ) << "[ " << m_RowVecInv[1] << " " << m_ColumnVecInv[1] << " " << m_SliceVecInv[1] << " ] + " << m_Offset[1];
	LOG( Dev, verbose_info ) << "[ " << m_RowVecInv[2] << " " << m_ColumnVecInv[2] << " " << m_SliceVecInv[2] << " ] + " << m_Offset[2];
	return true;
}
util::fvector3 ImageHolder::getPhysicalCoordsFromIndex(const util::vector4<size_t> &voxelCoords) const
{
	const auto rowVec = m_RowVec * voxelCoords[0];
	const auto columnVec = m_ColumnVec * voxelCoords[1];
	const auto sliceVec = m_SliceVec * voxelCoords[2];
	return  rowVec + columnVec + sliceVec + m_Offset ;
}
util::vector4<size_t> ImageHolder::getIndexFromPhysicalCoords(const util::fvector3 &physicalCoords) const
{
	const util::fvector3 vec1 = physicalCoords - m_Offset;
	const auto rowVec = m_RowVec * vec1[0];
	const auto columnVec = m_ColumnVec * vec1[1];
	const auto sliceVec = m_SliceVec * vec1[2];

	auto _ret = m_RowVecInv + m_ColumnVecInv + m_SliceVecInv;

	for( uint8_t i = 0; i < 3; i++ ) {
		if( _ret[i] < 0 ) _ret[i] -= 0.5;
		else _ret[i] += 0.5;
	}

	return {_ret[0],_ret[1],_ret[2]};
}
void ImageHolder::mapPhysicalToIndex ( const float *physicalCoords, int32_t *index )
{
	float vec[3] = { physicalCoords[0] - m_Offset[0], physicalCoords[1] - m_Offset[1], physicalCoords[2] - m_Offset[2] };
	index[0] = ( vec[0] * m_RowVecInv[0] + vec[1] * m_ColumnVecInv[0] + vec[2] * m_SliceVecInv[0] + 0.5 );
	index[1] = ( vec[0] * m_RowVecInv[1] + vec[1] * m_ColumnVecInv[1] + vec[2] * m_SliceVecInv[1] + 0.5 );
	index[2] = ( vec[0] * m_RowVecInv[2] + vec[1] * m_ColumnVecInv[2] + vec[2] * m_SliceVecInv[2] + 0.5 );

}

} //end namespace

