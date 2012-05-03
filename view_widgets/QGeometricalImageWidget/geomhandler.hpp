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
 * geomhandler.hpp
 *
 * Description:
 *
 *  Created on: Mar 27, 2012
 *      Author: tuerke
 ******************************************************************/
#ifndef VAST_GEOMHANDLER_HPP
#define VAST_GEOMHANDLER_HPP

#include "common.hpp"
#include "geometrical.hpp"
#include <QTransform>

namespace isis
{
namespace viewer
{
namespace widget
{
namespace _internal
{

const uint8_t rasteringFac = 50;

geometrical::BoundingBoxType getVoxelBoundingBox( const ImageHolder::Vector &images );

util::fvector4 getPhysicalBoundingBox( const ImageHolder::Vector &images, const PlaneOrientation &orientation, const bool &latched );

QTransform getQTransform( const ImageHolder::Pointer image, const PlaneOrientation &orientation, bool latched );

QTransform getTransform2ISISSpace( const PlaneOrientation &orientation, const util::fvector4 & );

util::Matrix4x4<qreal> getOrderedMatrix(  const boost::shared_ptr<ImageHolder> image, bool latched, bool inverse );

util::FixedMatrix<qreal, 2, 2> extract2DMatrix( const boost::shared_ptr<ImageHolder> image, const PlaneOrientation &orientation, bool latched, bool inverse );

util::fvector4 mapPhysicalCoords2Orientation( const util::fvector4 &coords, const PlaneOrientation &orientation );

void zoomBoundingBox( util::fvector4 &boundingBox, util::FixedVector<float, 2> &translation, const util::fvector4 &physCoord, const float &zoom, const PlaneOrientation &orientation, const bool &translate );
/*
template< typename TYPE >
static void extractSliceFromChunk( data::MemChunk<TYPE> &sliceChunk, const ImageHolder::Pointer image, const PlaneOrientation &orientation )
{
    const util::ivector4 mappedSize = mapCoordsToOrientation( image->getImageSize(), image->getImageProperties().latchedOrientation, orientation );
    const util::ivector4 mappedCoords = mapCoordsToOrientation( image->getImageProperties().voxelCoords, image->getImageProperties().latchedOrientation, orientation );
    const util::ivector4 mapping = mapCoordsToOrientation( util::ivector4( 0, 1, 2, 3 ), image->getImageProperties().latchedOrientation, orientation, true );
    const data::Chunk &chunk = image->getChunkVector()[image->getImageProperties().voxelCoords[dim_time]];
    //test for axial
    const util::Matrix4x4<float> mat = image->getImageProperties().orientation;
    util::fvector4 v1 = mat.getRow( 0 );
    util::fvector4 v2 = mat.getRow( 1 );

    const util::ivector4 voxelCoords = image->getImageProperties().voxelCoords;

    for ( util::ivector4::value_type y = 0; y < mappedSize[1]; y++ ) {
        for ( util::ivector4::value_type x = 0; x < mappedSize[0]; x++ ) {
            const util::ivector4 coords = v1 * x + v2 * y + voxelCoords;
            static_cast<data::Chunk &>( sliceChunk ).voxel<TYPE>( x, y ) = chunk.voxel<TYPE>( coords[0], coords[1], coords[2] );
        }
    }
}*/



}
}
}
}// end namespace


#endif // VAST_GEOMHANDLER_HPP