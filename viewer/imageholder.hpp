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
 * imageholder.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef IMAGEHOLDER_HPP
#define IMAGEHOLDER_HPP

#include <boost/foreach.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <vector>
#include <CoreUtils/propmap.hpp>
#include <DataStorage/image.hpp>
#include "common.hpp"
#include "color.hpp"
#include "widgetinterface.hpp"
#include "common.hpp"

namespace isis
{
namespace viewer
{

namespace color
{
class Color;
}
class WidgetInterface;
/**
 * Class that holds one image in a vector of data::ValuePtr's
 * It ensures the data is hold in continuous memory and only consists of one type.
 * Furthermore this class handles the meta information of the image
 */

class ImageHolder
{


public:
	typedef std::list<boost::shared_ptr< ImageHolder > > ImageListType;
	typedef data::_internal::ValuePtrBase::Reference ImagePointerType;

	enum ImageType { structural_image, z_map };

	ImageHolder();

	bool setImage( const data::Image &image, const ImageType &imageType, const std::string &filename = "" );

	size_t getID() const { return m_ID; }
	void setID( size_t id ) { m_ID = id; }

	std::vector< ImagePointerType > getImageVector() const { return m_ImageVector; }
	const std::vector< data::Chunk > &getChunkVector() const { return m_ChunkVector; }
	std::vector< data::Chunk > &getChunkVector() { return m_ChunkVector; }
	util::PropertyMap &getPropMap() { return m_PropMap; }
	const util::PropertyMap &getPropMap() const { return m_PropMap; }
	const util::FixedVector<size_t, 4> &getImageSize() const { return m_ImageSize; }
	boost::shared_ptr< data::Image >getISISImage() const { return m_Image; }
	boost::numeric::ublas::matrix<double> getNormalizedImageOrientation( bool transposed = false ) const;
	boost::numeric::ublas::matrix<double> getImageOrientation( bool transposed = false ) const;
	void addChangedAttribute( const std::string &attribute );
	bool removeChangedAttribute( const std::string &attribute );

	boost::weak_ptr<void>
	getImageWeakPointer( size_t timestep = 0 ) const {
		return getImageVector()[timestep]->getRawAddress();
	}

	util::slist getFileNames() const { return m_Filenames; }

	bool operator<( const ImageHolder &ref ) const { return m_ID < ref.getID(); }

	void checkVoxelCoords( util::ivector4 &voxelCoords );

	void setZeroIsReserved( bool isReserved ) { m_ZeroIsReserved = isReserved; }
	double getInternalExtent()  const;

	void updateColorMap();

	void addWidget( WidgetInterface *widget ) { m_WidgetList.push_back( widget ); }
	void removeWidget( WidgetInterface *widget );
	std::list< WidgetInterface * > getWidgetList() { return m_WidgetList; }

	void updateOrientation();
	void updateHistogram();

	void setVoxel( const size_t &first, const size_t &second, const size_t &third, const size_t &fourth, const double &value, bool sync = true );

	template<typename TYPE>
	void setTypedVoxel(  const size_t &first, const size_t &second, const size_t &third, const size_t &fourth, const TYPE &value, bool sync = true ) {
		getChunkVector()[fourth].voxel<InternalImageType>(first, second, third) = static_cast<double>( value ) * scalingToInternalType.first->as<double>() + scalingToInternalType.second->as<double>();
		if( sync ) {
			getISISImage()->getChunk(first, second, third, fourth, false).voxel<TYPE>(first, second, third, fourth ) = value;
		}
	}

	util::ivector4 voxelCoords;
	util::fvector4 physicalCoords;
	util::fvector4 voxelSize;
	bool isVisible;
	bool isRGB;
	float opacity;
	util::ivector4 alignedSize32;
	double offset;
	double scaling;
	double extent;
	double lowerThreshold;
	double upperThreshold;
	QVector<QRgb> colorMap;
	util::fvector4 rowVec;
	util::fvector4 columnVec;
	util::fvector4 sliceVec;
	std::string lut;
	ImageType imageType;
	InterpolationType interpolationType;
	std::pair<util::ValueReference, util::ValueReference> minMax;
	std::pair<util::ValueReference, util::ValueReference> internMinMax;
	boost::numeric::ublas::matrix<double> orientation;
	util::fvector4 indexOrigin;
	boost::numeric::ublas::matrix<double> latchedOrientation;
	unsigned short majorTypeID;
	std::string majorTypeName;
	std::vector< double *> histogramVector;
	std::vector< double *> histogramVectorWOZero;
	std::pair<util::ValueReference, util::ValueReference> scalingToInternalType;

private:

	  void logImageProps() const;
	
	util::FixedVector<size_t, 4> m_ImageSize;
	util::PropertyMap m_PropMap;

	bool m_ZeroIsReserved;
	InternalImageType m_ReservedValue;

	boost::shared_ptr<data::Image> m_Image;
	util::slist m_Filenames;
	size_t m_ID;
	std::pair<double, double> m_OptimalScalingPair;

	std::vector< ImagePointerType > m_ImageVector;
	std::vector< data::Chunk > m_ChunkVector;

	std::list<WidgetInterface *> m_WidgetList;

	boost::shared_ptr<color::Color> m_ColorHandler;

	template<typename TYPE>
	void copyImageToVector( const data::Image &image, bool reserveZero ) {
		data::ValuePtr<TYPE> imagePtr( ( TYPE * ) calloc( image.getVolume(), sizeof( TYPE ) ), image.getVolume() );
		LOG( Dev, info) << "Needed memory: " << image.getVolume() * sizeof( TYPE ) / ( 1024.0 * 1024.0 ) << " mb.";

		if( reserveZero ) {
			LOG( Dev, info ) << "0 is reserved";
			// calculate new scaling  
			data::scaling_pair scalingPair = image.getScalingTo( data::ValuePtr<TYPE>::staticID, data::upscale );
			double scaling = scalingPair.first->as<double>();
			double offset = scalingPair.second->as<double>();
			scaling /= static_cast<double>( getInternalExtent() + 1 ) / getInternalExtent();
			offset += 1;
			const data::scaling_pair newScaling( std::make_pair< util::ValueReference, util::ValueReference>( util::Value<double>( scaling ), util::Value<double>( offset ) ) ) ;
			scalingToInternalType = newScaling;
		} else {
			LOG( Dev, info ) << "0 is not reserved";
			scalingToInternalType = image.getScalingTo( data::ValuePtr<TYPE>::staticID, data::upscale );
		}
		LOG( Dev, info ) << "scalingToInternalType: " << scalingToInternalType.first->as<double>() << " : " << scalingToInternalType.second->as<double>();
		image.copyToMem<TYPE>( &imagePtr[0], image.getVolume(), scalingToInternalType );
		LOG( Dev, verbose_info ) << "Copied image to continuous memory space.";
		internMinMax = imagePtr.getMinMax();
		LOG( Dev, info ) << "internMinMax: " << internMinMax.first->as<double>() << " : " << internMinMax.second->as<double>();
		//splice the image in its volumes -> we get a vector of t volumes
		if( m_ImageSize[3] > 1 ) { //splicing is only necessary if we got more than 1 timestep
			m_ImageVector = imagePtr.splice( m_ImageSize[0] * m_ImageSize[1] * m_ImageSize[2] );
		} else {
			m_ImageVector.push_back( imagePtr );
		}
	}

	template<typename TYPE>
	void _setTrueZero( const data::Image &image ) {
		// first make shure the images datatype is consistent
		data::TypedImage<TYPE> tImage ( image );
// 		now set all voxels to the m_ReservedValue that are 0 in the origin image
		for( size_t t = 0; t < getImageSize()[3]; t++ ) {
			for( size_t z = 0; z < getImageSize()[2]; z++ ) {
				for( size_t y = 0; y < getImageSize()[1]; y++ ) {
#pragma omp parallel for
					for( size_t x = 0; x < getImageSize()[0]; x++ ) {
						if( static_cast<data::Image &>( tImage ).voxel<TYPE>( x, y, z, t ) == static_cast<TYPE>( 0 ) ) {
							m_ChunkVector[t].voxel<InternalImageType>( x, y, z ) = m_ReservedValue;
						}
					}
				}
			}
		}

	}
};

}
} //end namespace

#endif