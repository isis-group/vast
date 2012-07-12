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
 * imageholder.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef IMAGEHOLDER_HPP
#define IMAGEHOLDER_HPP

#include "common.hpp"
#include "color.hpp"
#include "geometrical.hpp"
#include <boost/foreach.hpp>
#include <vector>
#include <qapplication.h>
#include <CoreUtils/propmap.hpp>
#include <DataStorage/image.hpp>
#include <CoreUtils/matrix.hpp>


namespace isis
{
namespace viewer
{

namespace _internal
{
class __Image : public data::Image
{
public:
	__Image( const data::Image &image ): data::Image( image ) {
		imageSize = getSizeAsVector();
	};

	void mapPhysicalToIndex( const float *physicalCoords, int32_t *index );
	bool checkVoxel( const int32_t *coords );
private:
	util::ivector4 imageSize;
};
}

namespace widget
{
class WidgetInterface;
}
/**
 * Class that holds one image in a vector of data::ValuePtr's
 * It ensures the data is hold in continuous memory and only consists of one type.
 * Furthermore this class handles the meta information of the image
 */

class ImageHolder
{
public:
	enum ImageType { structural_image, statistical_image };
private:
	struct ImageProperties {
		std::string fileName;
		std::string filePath;
		util::ivector4 voxelCoords;
		util::ivector4 trueVoxelCoords;
		util::fvector3 physicalCoords;
		size_t timestep;
		util::fvector3 voxelSize;
		bool isVisible;
		bool isRGB;
		bool zeroIsReserved;
		float opacity;
		util::ivector4 alignedSize32;
		double offset;
		double scaling;
		size_t memSizeInternal;
		double extent;
		double lowerThreshold;
		double upperThreshold;
		color::Color::ColormapType colorMap;
		color::Color::AlphamapType alphaMap;
		util::fvector3 rowVec;
		util::fvector3 columnVec;
		util::fvector3 sliceVec;
		std::string lut;
		ImageType imageType;
		InterpolationType interpolationType;
		std::pair<util::ValueReference, util::ValueReference> minMax;
		std::pair<double, double> scalingMinMax;
		util::fvector3 indexOrigin;
		util::Matrix3x3<float> orientation;
		util::Matrix3x3<float> latchedOrientation;
		unsigned short majorTypeID;
		std::string majorTypeName;
		std::pair<util::ValueReference, util::ValueReference> scalingToInternalType;
		geometrical::BoundingBoxType boundingBox;
		double voxelValue;
	};

public:
	typedef boost::shared_ptr< ImageHolder > Pointer;
	typedef std::vector< Pointer > Vector;
	typedef std::map< std::string, Pointer > Map;


	ImageHolder();

	bool setImage( const data::Image &image, const ImageType &imageType, const std::string &filename );

	const std::vector< data::Chunk > &getChunkVector() const { return m_ChunkVector; }
	std::vector< data::Chunk > &getChunkVector() { return m_ChunkVector; }

	const std::vector< data::Chunk > &getVolumeVector() const { return m_VolumeVector; }
	std::vector< data::Chunk > &getVolumeVector() { return m_VolumeVector; }

	util::PropertyMap &getPropMap() { return m_PropMap; }
	const util::PropertyMap &getPropMap() const { return m_PropMap; }
	const util::FixedVector<size_t, 4> &getImageSize() const { return m_ImageSize; }
	boost::shared_ptr< _internal::__Image >getISISImage() const;

	void addChangedAttribute( const std::string &attribute );
	bool removeChangedAttribute( const std::string &attribute );

	ImageProperties &getImageProperties() { return m_ImageProperties; }
	const ImageProperties &getImageProperties() const { return m_ImageProperties; }

	boost::shared_ptr<const void>
	getRawAdress( size_t timestep = 0 ) const;

	template<unsigned short DIMS>
	void correctVoxelCoords( util::ivector4 &vc ) {
		for( unsigned short i = 0; i < DIMS; i++ ) {
			if( vc[i] < 0 ) vc[i] = 0;
			else if( vc[i] >= static_cast<int32_t>( getImageSize()[i] ) ) vc[i] = static_cast<int32_t>( getImageSize()[i] - 1 );
		}
	}

	template<unsigned short DIMS>
	bool checkVoxelCoords ( const util::ivector4 &vc ) {
		for( unsigned short i = 0; i < DIMS; i++ ) {
			if( vc[i] < 0 || vc[i] >= static_cast<int>( getImageSize()[i] ) ) {
				return false;
			}
		}

		return true;
	}

	bool checkVoxelCoords( const util::ivector4 &voxelCoords );

	void synchronize( );
	double getInternalExtent()  const;

	void updateColorMap();

	bool hasAmbiguousOrientation() const { return m_AmbiguousOrientation; }

	void updateOrientation();

	void setVoxel( const size_t &first, const size_t &second, const size_t &third, const size_t &fourth, const double &value, bool sync = true );

	template<typename TYPE>
	void setTypedVoxel(  const size_t &first, const size_t &second, const size_t &third, const size_t &fourth, const TYPE &value, bool sync = true ) {
		m_VolumeVector[fourth].voxel<InternalImageType>( first, second, third )
		= static_cast<double>( value ) * getImageProperties().scalingToInternalType.first->as<double>() + getImageProperties().scalingToInternalType.second->as<double>();

		if( sync ) {
			getISISImage()->voxel<TYPE>( first, second, third, fourth ) = value;

			if( value > getImageProperties().minMax.second->as<TYPE>() || value < getImageProperties().minMax.first->as<TYPE>() ) {
				getImageProperties().minMax = getISISImage()->getMinMax();
				getImageProperties().extent = fabs( getImageProperties().minMax.second->as<double>() - getImageProperties().minMax.first->as<double>() );
				updateColorMap();
			}
		}
	}

	void phyisicalCoordsChanged( const util::fvector3 &physicalCoords );
	void voxelCoordsChanged( const util::ivector4 &voxelCoords );
	void timestepChanged( const size_t &timestep );

private:
	util::Matrix3x3<float> calculateLatchedImageOrientation( bool transposed = false );
	util::Matrix3x3<float> calculateImageOrientation( bool transposed = false ) const;
	void logImageProps() const;
	unsigned short getMajorTypeID() const;
	void collectImageInfo();

	util::FixedVector<size_t, 4> m_ImageSize;
	util::PropertyMap m_PropMap;

	bool m_AmbiguousOrientation;

	boost::shared_ptr<_internal::__Image> m_Image;
	std::pair<double, double> m_OptimalScalingPair;

	std::vector< data::Chunk > m_ChunkVector;
	std::vector< data::Chunk > m_VolumeVector;

	boost::shared_ptr<color::Color> m_ColorHandler;

	ImageProperties m_ImageProperties;

	template<typename TYPE>
	void copyImageToVector( const data::Image &image ) {
		m_VolumeVector.clear();
		m_ChunkVector = image.copyChunksToVector();
		data::ValueArray<TYPE> imagePtr( ( TYPE * ) calloc( image.getVolume(), sizeof( TYPE ) ), image.getVolume() );
		getImageProperties().memSizeInternal = image.getVolume() * sizeof( TYPE );
		LOG( Dev, info ) << "Needed memory: " << getImageProperties().memSizeInternal / ( 1024.0 * 1024.0 ) << " mb.";

		if( getImageProperties().zeroIsReserved ) {
			data::scaling_pair scalingPair = image.getScalingTo( data::ValueArray<TYPE>::staticID, data::upscale );
			double scaling = scalingPair.first->as<double>();
			double offset = scalingPair.second->as<double>();
			scaling /= static_cast<double>( getInternalExtent() + 1 ) / getInternalExtent();
			offset += 1;
			const data::scaling_pair newScaling( std::make_pair< util::ValueReference, util::ValueReference>( util::Value<double>( scaling ), util::Value<double>( offset ) ) ) ;
			getImageProperties().scalingToInternalType = newScaling;
		} else {
			getImageProperties().scalingToInternalType = image.getScalingTo( data::ValueArray<TYPE>::staticID, data::upscale );
		}

		LOG( Dev, info ) << "scalingToInternalType: " << getImageProperties().scalingToInternalType.first->as<double>() << " : " << getImageProperties().scalingToInternalType.second->as<double>();
		image.copyToMem<TYPE>( &imagePtr[0], image.getVolume(), getImageProperties().scalingToInternalType );
		LOG( Dev, verbose_info ) << "Copied image to continuous memory space.";

		//splice the image in its volumes -> we get a vector of t volumes
		if( m_ImageSize[dim_time] > 1 ) { //splicing is only necessary if we got more than 1 timestep
			std::vector< data::ValueArrayReference > refVec = imagePtr.splice( m_ImageSize[0] * m_ImageSize[1] * m_ImageSize[2] );

			for ( std::vector< data::ValueArrayReference >::const_iterator iter = refVec.begin(); iter != refVec.end(); iter++ ) {
				m_VolumeVector.push_back( data::Chunk( *iter, m_ImageSize[0], m_ImageSize[1], m_ImageSize[2] ) );
			}
		} else {
			m_VolumeVector.push_back( data::Chunk( imagePtr, m_ImageSize[0], m_ImageSize[1], m_ImageSize[2] ) );
		}
	}


};

}
} //end namespace

#endif