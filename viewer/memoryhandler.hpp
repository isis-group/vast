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
#include <common.hpp>

namespace isis {
namespace viewer {

class MemoryHandler
{
public:
	static util::ivector4 get32BitAlignedSize( const util::ivector4 &origSize );

	template< typename TYPE>
	static void fillSliceChunk( data::MemChunk<TYPE> &sliceChunk, const boost::shared_ptr< ImageHolder > image, const PlaneOrientation &orientation, const size_t &timestep = 0 )
	{
		const util::ivector4 mappedSize = mapCoordsToOrientation( image->getImageSize(), image->getImageProperties().latchedOrientation, orientation );
		const util::ivector4 mappedCoords = mapCoordsToOrientation( image->getImageProperties().voxelCoords, image->getImageProperties().latchedOrientation, orientation );
		const util::ivector4 mapping = mapCoordsToOrientation( util::ivector4( 0, 1, 2, 3 ), image->getImageProperties().latchedOrientation, orientation, true );
		const data::Chunk &chunk = image->getChunkVector()[timestep];

		for ( int32_t y = 0; y < mappedSize[1]; y++ ) {
#pragma omp parallel for
			for ( int32_t x = 0; x < mappedSize[0]; x++ ) {
				const util::ivector4 coords( x, y, mappedCoords[2] );
				static_cast<data::Chunk &>( sliceChunk ).voxel<TYPE>( coords[0], coords[1] ) = chunk.voxel<TYPE>( coords[mapping[0]], coords[mapping[1]], coords[mapping[2]] );
			}
		}
	}
};


	
}} //end namespace



#endif //VAST_MEMORY_HANDLER_HPP