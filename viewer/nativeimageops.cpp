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
 * nativeimageops.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "nativeimageops.hpp"
#include <boost/shared_ptr.hpp>

isis::viewer::QViewerCore *isis::viewer::operation::NativeImageOps::m_ViewerCore;

std::pair< double, double > isis::viewer::operation::NativeImageOps::getMinMaxFromScalingOffset ( const std::pair< double, double >& scalingOffset, const isis::viewer::ImageHolder::Pointer image )
{
	std::pair<double, double> retMinMax;
	const double min = image->getImageProperties().minMax.first->as<double>();
	const double extent = image->getImageProperties().extent;
	const double offset = scalingOffset.second;
	const double scaling = scalingOffset.first;
	retMinMax.first = min + offset;
	retMinMax.second = ( min + offset ) + extent / scaling;
	return retMinMax;
}

std::pair< double, double > isis::viewer::operation::NativeImageOps::getScalingOffsetFromMinMax ( const std::pair< double, double >& minMax, const isis::viewer::ImageHolder::Pointer image )
{
	std::pair<double, double> retScalingOffset;
	const double extent = image->getImageProperties().extent;
	const double _extent = minMax.second - minMax.first;
	const double min = image->getImageProperties().minMax.first->as<double>();
	retScalingOffset.first = extent / _extent;
	retScalingOffset.second = minMax.first - min ;
	return retScalingOffset;
}


isis::util::ivector4 isis::viewer::operation::NativeImageOps::getGlobalMin( const boost::shared_ptr< isis::viewer::ImageHolder > image, const util::ivector4 &startPos, const unsigned short &radius )
{
	if( image->getISISImage()->getVolume() >= 1e6 ) {
		m_ViewerCore->getUICore()->toggleLoadingIcon( true );
	}

	switch ( image->getISISImage()->getMajorTypeID() ) {
	case data::ValueArray<bool>::staticID:
		return internGetMax<bool>( image, startPos, radius );
		break;
	case data::ValueArray<int8_t>::staticID:
		return internGetMin<int8_t>( image, startPos, radius );
		break;
	case data::ValueArray<uint8_t>::staticID:
		return internGetMin<uint8_t>( image, startPos, radius );
		break;
	case data::ValueArray<int16_t>::staticID:
		return internGetMin<int16_t>( image, startPos, radius );
		break;
	case data::ValueArray<uint16_t>::staticID:
		return internGetMin<uint16_t>( image, startPos, radius );
		break;
	case data::ValueArray<int32_t>::staticID:
		return internGetMin<int32_t>( image, startPos, radius );
		break;
	case data::ValueArray<uint32_t>::staticID:
		return internGetMin<uint32_t>( image, startPos, radius );
		break;
	case data::ValueArray<int64_t>::staticID:
		return internGetMin<int64_t>( image, startPos, radius );
		break;
	case data::ValueArray<uint64_t>::staticID:
		return internGetMin<uint64_t>( image, startPos, radius );
		break;
	case data::ValueArray<float>::staticID:
		return internGetMin<float>( image, startPos, radius );
		break;
	case data::ValueArray<double>::staticID:
		return internGetMin<double>( image, startPos, radius );
		break;
	default:
		LOG( Runtime, error ) << "Search of min/max is not suported for " << image->getISISImage()->getMajorTypeID() << " !";
		return isis::util::ivector4();
		break;
	}

}

isis::util::ivector4 isis::viewer::operation::NativeImageOps::getGlobalMax( const boost::shared_ptr< isis::viewer::ImageHolder > image, const util::ivector4 &startPos, const unsigned short &radius )
{
	if( image->getISISImage()->getVolume() >= 1e6 ) {
		m_ViewerCore->getUICore()->toggleLoadingIcon( true );
	}

	switch ( image->getISISImage()->getMajorTypeID() ) {
	case data::ValueArray<bool>::staticID:
		return internGetMax<bool>( image, startPos, radius );
		break;
	case data::ValueArray<int8_t>::staticID:
		return internGetMax<int8_t>( image, startPos, radius );
		break;
	case data::ValueArray<uint8_t>::staticID:
		return internGetMax<uint8_t>( image, startPos, radius );
		break;
	case data::ValueArray<int16_t>::staticID:
		return internGetMax<int16_t>( image, startPos, radius );
		break;
	case data::ValueArray<uint16_t>::staticID:
		return internGetMax<uint16_t>( image, startPos, radius );
		break;
	case data::ValueArray<int32_t>::staticID:
		return internGetMax<int32_t>( image, startPos, radius );
		break;
	case data::ValueArray<uint32_t>::staticID:
		return internGetMax<uint32_t>( image, startPos, radius );
		break;
	case data::ValueArray<int64_t>::staticID:
		return internGetMax<int64_t>( image, startPos, radius );
		break;
	case data::ValueArray<uint64_t>::staticID:
		return internGetMax<uint64_t>( image, startPos, radius );
		break;
	case data::ValueArray<float>::staticID:
		return internGetMax<float>( image, startPos, radius );
		break;
	case data::ValueArray<double>::staticID:
		return internGetMax<double>( image, startPos, radius );
		break;
	default:
		LOG( Runtime, error ) << "Search of min/max is not suported for " << image->getISISImage()->getMajorTypeID() << " !";
		return isis::util::ivector4();
		break;
	}
}

void isis::viewer::operation::NativeImageOps::setTrueZero ( boost::shared_ptr< isis::viewer::ImageHolder > image )
{
	if( !image->getImageProperties().isRGB ) {
		m_ViewerCore->getUICore()->toggleLoadingIcon( true );

		switch ( image->getImageProperties().majorTypeID ) {
		case data::ValueArray<bool>::staticID:
			_setTrueZero<bool>( image );
			break;
		case data::ValueArray<int8_t>::staticID:
			_setTrueZero<int8_t>( image );
			break;
		case data::ValueArray<uint8_t>::staticID:
			_setTrueZero<uint8_t>( image );
			break;
		case data::ValueArray<int16_t>::staticID:
			_setTrueZero<int16_t>( image );
			break;
		case data::ValueArray<uint16_t>::staticID:
			_setTrueZero<uint16_t>( image );
			break;
		case data::ValueArray<int32_t>::staticID:
			_setTrueZero<int32_t>( image );
			break;
		case data::ValueArray<uint32_t>::staticID:
			_setTrueZero<uint32_t>( image );
			break;
		case data::ValueArray<int64_t>::staticID:
			_setTrueZero<int64_t>( image );
			break;
		case data::ValueArray<uint64_t>::staticID:
			_setTrueZero<uint64_t>( image );
			break;
		case data::ValueArray<float>::staticID:
			_setTrueZero<float>( image );
			break;
		case data::ValueArray<double>::staticID:
			_setTrueZero<double>( image );
			break;
		}
	}

	m_ViewerCore->getUICore()->toggleLoadingIcon( false );
}

std::vector<double> isis::viewer::operation::NativeImageOps::getHistogramFromImage ( const isis::viewer::ImageHolder::Pointer image )
{
	std::vector<double> histogram;
	const size_t volume = image->getImageSize()[0] * image->getImageSize()[1] * image->getImageSize()[2];
	const double extent = image->getInternalExtent();

	const int32_t timestep = image->getImageProperties().voxelCoords[dim_time];

	histogram.resize( extent );

	const InternalImageType *dataPtr = boost::static_pointer_cast<const InternalImageType>( image->getRawAdress( timestep ) ).get();

	if( extent > 1e6 ) {
		m_ViewerCore->getUICore()->toggleLoadingIcon( true, QString( "Calculating histogram for image " ) + image->getImageProperties().fileName.c_str() );
	}

	//create the histogram
	for( size_t i = 0; i < volume; i++ ) {
		if( dataPtr[i] > 0 ) {
			histogram[dataPtr[i] - 1]++;
		}
	}

	m_ViewerCore->getUICore()->toggleLoadingIcon( false );
	return histogram;
}



void isis::viewer::operation::NativeImageOps::setViewerCore ( isis::viewer::QViewerCore *core )
{
	m_ViewerCore = core;
}

