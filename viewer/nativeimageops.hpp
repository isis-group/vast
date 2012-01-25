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
#include "imageholder.hpp"
#include <DataStorage/image.hpp>

namespace isis
{
namespace viewer
{
namespace operation
{

class NativeImageOps
{
	static boost::shared_ptr< QProgressFeedback > m_ProgressFeedback;
public:

	static util::ivector4 getGlobalMin( const boost::shared_ptr<ImageHolder> image, const util::ivector4 &startPos, const unsigned short &radius );
	static util::ivector4 getGlobalMax( const boost::shared_ptr<ImageHolder> image, const util::ivector4 &startPos, const unsigned short &radius );

	static void setProgressFeedBack( boost::shared_ptr< QProgressFeedback > progressFeedback );

private:

	template<typename TYPE>
	static util::ivector4 internGetMin( const boost::shared_ptr<ImageHolder> image, const util::ivector4 &startPos, const unsigned short &radius ) {
		const util::ivector4 size = image->getImageSize();
		util::ivector4 start;
		util::ivector4 end;
        const size_t timestep = image->voxelCoords[3];
		if( radius ) {
			for( size_t i = 0; i < 3; i++ ) {
				start[i] = ( startPos[i] - radius ) < 0 ? 0 : startPos[i] - radius;
				end[i] = ( startPos[i] + radius ) > size[i] ? size[i] : startPos[i] + radius;
			}
		} else {
			start = util::ivector4( 0, 0, 0, 0 );
			end = size;
		}

		util::ivector4 currentPos;
		TYPE currentValue;
		data::TypedImage<TYPE> typedImage = *image->getISISImage();
		TYPE currentMin = std::numeric_limits<TYPE>::max();
		#pragma omp parallel for

		for( int32_t z = start[2] + 1; z < end[2]; z++ ) {
			for( int32_t y = start[1] + 1; y < end[1]; y++ ) {
				for( int32_t x = start[0] + 1; x < end[0]; x++ ) {
					currentValue = static_cast<data::Image &>( typedImage ).voxel<TYPE>( x, y, z, timestep );

					if( currentValue < currentMin ) {
						currentPos = util::ivector4( x, y, z, timestep );
						currentMin = currentValue;
					}
				}
			}
		}

		return currentPos;
	}

	template<typename TYPE>
	static util::ivector4 internGetMax( const boost::shared_ptr<ImageHolder> image, const util::ivector4 &startPos, const unsigned short &radius ) {
		const util::ivector4 size = image->getImageSize();
		util::ivector4 start;
		util::ivector4 end;
        const size_t timestep = image->voxelCoords[3];
		
		if( radius ) {
			for( size_t i = 0; i < 3; i++ ) {
				start[i] = ( startPos[i] - radius ) < 0 ? 0 : startPos[i] - radius;
				end[i] = ( startPos[i] + radius ) > size[i] ? size[i] : startPos[i] + radius;
			}
		} else {
			start = util::ivector4( 0, 0, 0, 0 );
			end = size;
		}

		util::ivector4 currentPos;
		TYPE currentValue;
		data::TypedImage<TYPE> typedImage = *image->getISISImage();
		TYPE currentMax = std::numeric_limits<TYPE>::min();
		#pragma omp parallel for

		for( int32_t z = start[2] + 1; z < end[2]; z++ ) {
			for( int32_t y = start[1] + 1; y < end[1]; y++ ) {
				for( int32_t x = start[0] + 1; x < end[0]; x++ ) {
					currentValue = static_cast<data::Image &>( typedImage ).voxel<TYPE>( x, y, z, timestep );

					if( currentValue > currentMax ) {
						currentPos = util::ivector4( x, y, z, timestep );
						currentMax = currentValue;
					}
				}
			}
		}

		return currentPos;
	}
};



}
}
}


#endif