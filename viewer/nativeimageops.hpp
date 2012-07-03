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
 * nativeimageops.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef NATIVEIMAGEOPS_HPP
#define NATIVEIMAGEOPS_HPP

#include "common.hpp"

#include <boost/concept_check.hpp>
#include <numeric>
#include "qprogressfeedback.hpp"
#include "qviewercore.hpp"
#include "imageholder.hpp"
#include "uicore.hpp"
#include <DataStorage/image.hpp>

namespace isis
{
namespace viewer
{
namespace operation
{

class NativeImageOps
{
	static isis::viewer::QViewerCore *m_ViewerCore;
public:

	static util::ivector4 getGlobalMin( const ImageHolder::Pointer image, const util::ivector4 &startPos, const unsigned short &radius );
	static util::ivector4 getGlobalMax( const ImageHolder::Pointer image, const util::ivector4 &startPos, const unsigned short &radius );
	static void setTrueZero( ImageHolder::Pointer image );
	static std::vector<double> getHistogramFromImage( const ImageHolder::Pointer image );

	static std::pair<double, double> getMinMaxFromScalingOffset( const std::pair<double, double> &scalingOffset, const ImageHolder::Pointer image );
	static std::pair<double, double> getScalingOffsetFromMinMax( const std::pair<double, double> &minMax, const ImageHolder::Pointer image );

	static void setViewerCore( QViewerCore *core );

private:

	template<typename TYPE>
	static util::ivector4 internGetMin( const ImageHolder::Pointer image, const util::ivector4 &startPos, const unsigned short &radius ) {
		const util::ivector4 size = image->getImageSize();
		util::ivector4 start;
		util::ivector4 end;

		if( radius ) {
			for( size_t i = 0; i < 3; i++ ) {
				start[i] = ( startPos[i] - radius ) < 0 ? 0 : startPos[i] - radius;
				end[i] = ( startPos[i] + radius ) > size[i] ? size[i] : startPos[i] + radius;
			}

			end[3] = size[3];
			start[3] = 0;
		} else {
			start = util::ivector4( 0, 0, 0, 0 );
			end = size;
		}

		util::ivector4 currentPos;
		TYPE currentValue;
		data::TypedImage<TYPE> typedImage = *image->getISISImage();
		TYPE currentMin = std::numeric_limits<TYPE>::max();
		m_ViewerCore->getProgressFeedback()->show( ( end[2] - start[2] ) * ( end[3] - start[3] ), "Searching minimum..." );

		for( int32_t t = start[3]; t < end[3]; t++ ) {
			for( int32_t z = start[2]; z < end[2]; z++ ) {
				m_ViewerCore->getProgressFeedback()->progress();

				for( int32_t y = start[1]; y < end[1]; y++ ) {
					for( int32_t x = start[0]; x < end[0]; x++ ) {
						currentValue = static_cast<data::Image &>( typedImage ).voxel<TYPE>( x, y, z, t );

						if( currentValue < currentMin ) {
							currentPos = util::ivector4( x, y, z, t );
							currentMin = currentValue;
						}
					}
				}
			}
		}

		m_ViewerCore->getUICore()->toggleLoadingIcon( false );
		return currentPos;
	}

	template<typename TYPE>
	static util::ivector4 internGetMax( const ImageHolder::Pointer image, const util::ivector4 &startPos, const unsigned short &radius ) {
		const util::ivector4 size = image->getImageSize();
		util::ivector4 start;
		util::ivector4 end;

		if( radius ) {
			for( size_t i = 0; i < 3; i++ ) {
				start[i] = ( startPos[i] - radius ) < 0 ? 0 : startPos[i] - radius;
				end[i] = ( startPos[i] + radius ) > size[i] ? size[i] : startPos[i] + radius;
			}

			end[3] = size[3];
			start[3] = 0;
		} else {
			start = util::ivector4( 0, 0, 0, 0 );
			end = size;
		}

		util::ivector4 currentPos;
		TYPE currentValue;
		data::TypedImage<TYPE> typedImage = *image->getISISImage();
		TYPE currentMax = -std::numeric_limits<TYPE>::max();

		m_ViewerCore->getProgressFeedback()->show( ( end[2] - start[2] ) * ( end[3] - start[3] ), "Searching maximum..." );

		for( int32_t t = start[3]; t < end[3]; t++ ) {
			for( int32_t z = start[2]; z < end[2]; z++ ) {
				m_ViewerCore->getProgressFeedback()->progress();

				for( int32_t y = start[1]; y < end[1]; y++ ) {
					for( int32_t x = start[0]; x < end[0]; x++ ) {
						currentValue = static_cast<data::Image &>( typedImage ).voxel<TYPE>( x, y, z, t );

						if( currentValue > currentMax ) {
							currentPos = util::ivector4( x, y, z, t );
							currentMax = currentValue;
						}
					}
				}
			}
		}

		m_ViewerCore->getUICore()->toggleLoadingIcon( false );
		return currentPos;
	}


	template<typename TYPE>
	static void _setTrueZero( ImageHolder::Pointer image ) {
		LOG( Dev, info ) << "Setting true zero for " << image->getImageProperties().fileName;
		image->getImageProperties().zeroIsReserved = true;
		// first make shure the images datatype is consistent
		data::TypedImage<TYPE> tImage ( *image->getISISImage() );
		//now set all voxels to the m_ReservedValue that are 0 in the origin image
		m_ViewerCore->getProgressFeedback()->show( image->getImageSize()[2] * image->getImageSize()[3], "Setting 0 to black..." );

		for( size_t t = 0; t < image->getImageSize()[3]; t++ ) {
			for( size_t z = 0; z < image->getImageSize()[2]; z++ ) {
				m_ViewerCore->getProgressFeedback()->progress();

				for( size_t y = 0; y < image->getImageSize()[1]; y++ ) {
					for( size_t x = 0; x < image->getImageSize()[0]; x++ ) {
						if( static_cast<data::Image &>( tImage ).voxel<TYPE>( x, y, z, t ) == static_cast<TYPE>( 0 ) ) {
							image->getVolumeVector()[t].voxel<InternalImageType>( x, y, z ) = 0;
						}
					}
				}
			}
		}

		m_ViewerCore->emitImageContentChanged( image );

	}
};



}
}
}


#endif