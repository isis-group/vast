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
	const double min = image->getImageProperties().minMax.first.as<double>();
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
	const double min = image->getImageProperties().minMax.first.as<double>();
	retScalingOffset.first = extent / _extent;
	retScalingOffset.second = minMax.first - min ;
	return retScalingOffset;
}


isis::util::vector4<size_t> isis::viewer::operation::NativeImageOps::getGlobalMin(const std::shared_ptr< isis::viewer::ImageHolder > image, const util::vector4<size_t> &startPos, const unsigned short &radius )
{
	if( image->getISISImage()->getVolume() >= 1e6 ) {
		m_ViewerCore->getUICore()->toggleLoadingIcon( true );
	}

	//@todo make this less ridiculous
	switch ( image->getISISImage()->getMajorTypeID() ) {
		case util::typeID<bool>():
		return internGetMax<bool>( image, startPos, radius );
		break;
	case util::typeID<int8_t>():
		return internGetMin<int8_t>( image, startPos, radius );
		break;
	case util::typeID<uint8_t>():
		return internGetMin<uint8_t>( image, startPos, radius );
		break;
	case util::typeID<int16_t>():
		return internGetMin<int16_t>( image, startPos, radius );
		break;
	case util::typeID<uint16_t>():
		return internGetMin<uint16_t>( image, startPos, radius );
		break;
	case util::typeID<int32_t>():
		return internGetMin<int32_t>( image, startPos, radius );
		break;
	case util::typeID<uint32_t>():
		return internGetMin<uint32_t>( image, startPos, radius );
		break;
	case util::typeID<int64_t>():
		return internGetMin<int64_t>( image, startPos, radius );
		break;
	case util::typeID<uint64_t>():
		return internGetMin<uint64_t>( image, startPos, radius );
		break;
	case util::typeID<float>():
		return internGetMin<float>( image, startPos, radius );
		break;
	case util::typeID<double>():
		return internGetMin<double>( image, startPos, radius );
		break;
	default:
		LOG( Runtime, error ) << "Search of min/max is not suported for " << image->getISISImage()->getMajorTypeID() << " !";
		return {0,0,0,0};
		break;
	}

}

isis::util::vector4<size_t> isis::viewer::operation::NativeImageOps::getGlobalMax(const std::shared_ptr< isis::viewer::ImageHolder > image, const util::vector4<size_t> &startPos, const unsigned short &radius )
{
	if( image->getISISImage()->getVolume() >= 1e6 ) {
		m_ViewerCore->getUICore()->toggleLoadingIcon( true );
	}

	switch ( image->getISISImage()->getMajorTypeID() ) {
	case util::typeID<bool>():
		return internGetMax<bool>( image, startPos, radius );
		break;
	case util::typeID<int8_t>():
		return internGetMax<int8_t>( image, startPos, radius );
		break;
	case util::typeID<uint8_t>():
		return internGetMax<uint8_t>( image, startPos, radius );
		break;
	case util::typeID<int16_t>():
		return internGetMax<int16_t>( image, startPos, radius );
		break;
	case util::typeID<uint16_t>():
		return internGetMax<uint16_t>( image, startPos, radius );
		break;
	case util::typeID<int32_t>():
		return internGetMax<int32_t>( image, startPos, radius );
		break;
	case util::typeID<uint32_t>():
		return internGetMax<uint32_t>( image, startPos, radius );
		break;
	case util::typeID<int64_t>():
		return internGetMax<int64_t>( image, startPos, radius );
		break;
	case util::typeID<uint64_t>():
		return internGetMax<uint64_t>( image, startPos, radius );
		break;
	case util::typeID<float>():
		return internGetMax<float>( image, startPos, radius );
		break;
	case util::typeID<double>():
		return internGetMax<double>( image, startPos, radius );
		break;
	default:
		LOG( Runtime, error ) << "Search of min/max is not suported for " << image->getISISImage()->getMajorTypeID() << " !";
		return {0,0,0,0};
		break;
	}
}

void isis::viewer::operation::NativeImageOps::setTrueZero ( std::shared_ptr< isis::viewer::ImageHolder > image )
{
	if( !image->getImageProperties().isRGB ) {
		m_ViewerCore->getUICore()->toggleLoadingIcon( true );

		switch ( image->getImageProperties().majorTypeID ) {
		case util::typeID<bool>():
			_setTrueZero<bool>( image );
			break;
		case util::typeID<int8_t>():
			_setTrueZero<int8_t>( image );
			break;
		case util::typeID<uint8_t>():
			_setTrueZero<uint8_t>( image );
			break;
		case util::typeID<int16_t>():
			_setTrueZero<int16_t>( image );
			break;
		case util::typeID<uint16_t>():
			_setTrueZero<uint16_t>( image );
			break;
		case util::typeID<int32_t>():
			_setTrueZero<int32_t>( image );
			break;
		case util::typeID<uint32_t>():
			_setTrueZero<uint32_t>( image );
			break;
		case util::typeID<int64_t>():
			_setTrueZero<int64_t>( image );
			break;
		case util::typeID<uint64_t>():
			_setTrueZero<uint64_t>( image );
			break;
		case util::typeID<float>():
			_setTrueZero<float>( image );
			break;
		case util::typeID<double>():
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

	const InternalImageType *dataPtr = std::static_pointer_cast<const InternalImageType>( image->getRawAdress( timestep ) ).get();

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

