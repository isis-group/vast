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
 * memoryhandler.hpp
 *
 * Description:
 *
 *  Created on: Mar 16, 2012
 *      Author: tuerke
 ******************************************************************/
#ifndef VAST_MEMORY_HANDLER_HPP
#define VAST_MEMORY_HANDLER_HPP

#include <CoreUtils/vector.hpp>
#include <DataStorage/chunk.hpp>
#include "../viewer/common.hpp"
#include "imageholder.hpp"
#include <boost/timer.hpp>

namespace isis
{
namespace viewer
{

class MemoryHandler
{
public:
	static util::ivector4 get32BitAlignedSize( const util::ivector4 &origSize );

	template< typename TYPE>
	static void fillSliceChunk( data::MemChunk<TYPE> &sliceChunk, const ImageHolder::Pointer image, const PlaneOrientation &orientation ) {

		const util::ivector4 trueVoxelCoords = image->getImageProperties().trueVoxelCoords;

		const util::ivector4 mappedSize = mapCoordsToOrientation( image->getImageSize(), image->getImageProperties().latchedOrientation, orientation );
		const util::ivector4 mappedCoords = mapCoordsToOrientation( trueVoxelCoords, image->getImageProperties().latchedOrientation, orientation );
		const util::ivector4 mapping = mapCoordsToOrientation( util::ivector4( 0, 1, 2, 3 ), image->getImageProperties().latchedOrientation, orientation, true );
		const util::ivector4 _mapping = mapCoordsToOrientation( util::ivector4( 0, 1, 2, 3 ), image->getImageProperties().latchedOrientation, orientation, false );
		const data::Chunk &chunk = image->getVolumeVector()[image->getImageProperties().timestep];

		const bool sliceIsInside = trueVoxelCoords[_mapping[2]] >= 0 && trueVoxelCoords[_mapping[2]] < mappedSize[2];

		TYPE *dest = &static_cast<data::Chunk &>( sliceChunk ).voxel<TYPE>( 0 );

		const util::ivector4 coords( 0, 0, mappedCoords[2] );
		const TYPE *src = &chunk.voxel<TYPE>( coords[mapping[0]], coords[mapping[1]], coords[mapping[2]] );

		const util::ivector4 sizeAligned = static_cast<data::Chunk &>( sliceChunk ).getSizeAsVector();

		const util::ivector4::value_type coords1x[3] = {0, 0, mappedCoords[2] };
		const util::ivector4::value_type coords2x[3] = {1, 0, mappedCoords[2] };
		const size_t lin1x = chunk.getLinearIndex( util::ivector4( coords1x[mapping[0]], coords1x[mapping[1]], coords1x[mapping[2]] ) );
		const size_t lin2x = chunk.getLinearIndex( util::ivector4( coords2x[mapping[0]], coords2x[mapping[1]], coords2x[mapping[2]] ) );
		const size_t linx = lin2x - lin1x;

		const util::ivector4::value_type coords1y[3] = {0, 0, mappedCoords[2] };
		const util::ivector4::value_type coords2y[3] = {0, 1, mappedCoords[2] };
		const size_t lin1y = chunk.getLinearIndex( util::ivector4( coords1y[mapping[0]], coords1y[mapping[1]], coords1y[mapping[2]] ) );
		const size_t lin2y = chunk.getLinearIndex( util::ivector4( coords2y[mapping[0]], coords2y[mapping[1]], coords2y[mapping[2]] ) );
		const size_t liny = lin2y - lin1y;

		for ( util::ivector4::value_type y = 0; y < mappedSize[1]; y++ ) {
			for ( util::ivector4::value_type x = 0; x < mappedSize[0]; x++ ) {
				if( sliceIsInside ) {
					std::memcpy( dest + x + sizeAligned[0] * y,
								 src + x * linx + y * liny,
								 sizeof( TYPE ) );
				}
			}
		}
	}

	template< typename TYPE>
	static void fillSliceChunkOriented( data::MemChunk<TYPE> &sliceChunk, const ImageHolder::Pointer image, const PlaneOrientation &orientation ) {
		if( image->getImageProperties().latchedOrientation == image->getImageProperties().orientation ) {
			fillSliceChunk<TYPE>( sliceChunk, image, orientation );
		} else {
			const data::Chunk &chunk = image->getVolumeVector()[image->getImageProperties().timestep];
			boost::shared_ptr< _internal::__Image > isisImage = image->getISISImage();
			const geometrical::BoundingBoxType &bb = image->getImageProperties().boundingBox;
			const util::ivector4 mapping = mapCoordsToOrientation( util::fvector4( 0, 1, 2 ), image->getImageProperties().latchedOrientation, orientation );
			const util::ivector4 _mapping = mapCoordsToOrientation( util::fvector4( 0, 1, 2 ), util::IdentityMatrix<float, 3>(), orientation );
			float phys[] = { image->getImageProperties().physicalCoords[0], image->getImageProperties().physicalCoords[1], image->getImageProperties().physicalCoords[2] };
			float factor = 1. / sqrt( 2 );
			const float stepI = factor * image->getImageProperties().voxelSize[mapping[0]];
			const float stepJ = factor * image->getImageProperties().voxelSize[mapping[1]];

			const util::ivector4 sizeSliceChunk = static_cast<data::Chunk &>( sliceChunk ).getSizeAsVector();
			const util::ivector4 sizeChunk = chunk.getSizeAsVector();

			const TYPE *src = &chunk.voxel<TYPE>( 0 );
			TYPE *dest = &static_cast<data::Chunk &>( sliceChunk ).voxel<TYPE>( 0 );
			int32_t voxCoords[3];

			for ( float j = bb[_mapping[1]].first; j < bb[_mapping[1]].second; j += stepJ  ) {
				phys[_mapping[1]] = j;

				for ( float i = bb[_mapping[0]].first; i < bb[_mapping[0]].second; i += stepI ) {
					phys[_mapping[0]] = i;
					isisImage->mapPhysicalToIndex( phys, voxCoords );

					if( isisImage->checkVoxel( voxCoords ) ) {
						const int32_t sliceCoords[] = { voxCoords[mapping[0]], voxCoords[mapping[1]] };
						const int32_t chunkCoords[] = { voxCoords[0], voxCoords[1], voxCoords[2] };
						//                      memcpy( dest + sliceCoords[0] + sizeSliceChunk[0] * sliceCoords[1],
						//                                   src + chunkCoords[0] + sizeChunk[0] * chunkCoords[1] + sizeChunk[0] * sizeChunk[1] * chunkCoords[2],
						//                                   sizeof( TYPE ) );
						dest[sliceCoords[0] + sizeSliceChunk[0] * sliceCoords[1]]
						= src[chunkCoords[0] + sizeChunk[0] * chunkCoords[1] + sizeChunk[0] * sizeChunk[1] * chunkCoords[2]];
					}
				}
			}
		}
	}

};



}
} //end namespace



#endif //VAST_MEMORY_HANDLER_HPP
