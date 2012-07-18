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
 * QOrientationHandler.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "QOrientationHandler.hpp"

namespace isis
{

namespace viewer
{
namespace widget
{

QOrientationHandler::ViewPortType QOrientationHandler::getViewPort(  const float &zoom, const boost::shared_ptr< ImageHolder > image, const size_t &w, const size_t &h, PlaneOrientation orientation, unsigned short border )
{
	ViewPortType viewPort;
	const util::ivector4 size = image->getImageSize();
	const util::fvector3 mappedSize = mapCoordsToOrientation( util::fvector3(size[0], size[1], size[2]), image->getImageProperties().latchedOrientation, orientation );
	const util::fvector3 mappedScaling = mapCoordsToOrientation( image->getImageProperties().voxelSize, image->getImageProperties().latchedOrientation, orientation );
	const util::fvector3 physSize = mappedScaling * mappedSize;
	const float scalew = ( w - border * 2 ) / float( physSize[0] );
	const  float scaleh = ( h - border * 2 ) / float( physSize[1] );

	float scale = scaleh < scalew ? scaleh : scalew;

	viewPort[0] =  mappedScaling[0] * zoom * scale;
	viewPort[1] =  mappedScaling[1] * zoom * scale;

	const float offsetX = ( ( w - border * 2 ) - ( mappedSize[0] * viewPort[0] ) ) / 2 + border;
	const float offsetY = ( ( h - border * 2 ) - ( mappedSize[1] * viewPort[1] ) ) / 2 + border;
	viewPort[2] = offsetX ;
	viewPort[3] = offsetY ;
	viewPort[4] = round( mappedSize[0] * viewPort[0] );
	viewPort[5] = round( mappedSize[1] * viewPort[1] );
	return viewPort;
}


QTransform QOrientationHandler::getTransform( const ViewPortType &viewPort, const boost::shared_ptr< ImageHolder > image, PlaneOrientation orientation )
{
	const util::ivector4 mappedSize = mapCoordsToOrientation( image->getImageSize(), image->getImageProperties().latchedOrientation, orientation );
	const util::fvector4 flipVec = mapCoordsToOrientation( util::fvector4( 1, 1, 1 ), image->getImageProperties().latchedOrientation, orientation, false, false );
	const util::ivector4 mappedVoxelCoords = mapCoordsToOrientation( image->getImageProperties().voxelCoords, image->getImageProperties().latchedOrientation, orientation );
	QTransform retTransform;
	retTransform.setMatrix( flipVec[0], 0, 0,
							0, flipVec[1], 0,
							0, 0, 1 );

	//calculate crosshair dependent translation
	retTransform.translate( flipVec[0] * viewPort[2], flipVec[1] * viewPort[3] );
	retTransform.scale( viewPort[0], viewPort[1] );
	retTransform.translate(  flipVec[0] < 0 ? -mappedSize[0] : 0, flipVec[1] < 0 ? -mappedSize[1] : 0 );

	return retTransform;

}

util::ivector4 QOrientationHandler::convertWindow2VoxelCoords( const ViewPortType &viewPort, const boost::shared_ptr< ImageHolder > image, const size_t &x, const size_t &y, const size_t &slice, PlaneOrientation orientation )
{
	const util::ivector4 mappedSize = mapCoordsToOrientation( image->getImageSize(), image->getImageProperties().latchedOrientation, orientation, false, false );
	const size_t voxCoordX = ( x - viewPort[2] ) / viewPort[0];
	const size_t voxCoordY = ( y - viewPort[3] ) / viewPort[1];
	util::ivector4 coords =  util::ivector4( voxCoordX, voxCoordY, slice );

	for ( short i = 0; i < 2; i++ ) {
		coords[i] = mappedSize[i] < 0 ? abs( mappedSize[i] ) - coords[i] - 1 : coords[i];
		coords[i] = coords[i] < 0 ? 0 : coords[i];
		coords[i] = coords[i] >= abs( mappedSize[i] ) ? abs( mappedSize[i] ) - 1 : coords[i];
	}

	coords[3] = image->getImageProperties().voxelCoords[3];
	return mapCoordsToOrientation( coords, image->getImageProperties().latchedOrientation, orientation, true );
}

std::pair< int, int > QOrientationHandler::convertVoxel2WindowCoords( const ViewPortType &viewPort, const boost::shared_ptr< ImageHolder > image, PlaneOrientation orientation )
{
	const util::ivector4 mappedVoxelCoords = mapCoordsToOrientation( image->getImageProperties().voxelCoords, image->getImageProperties().latchedOrientation, orientation );
	const float halfVoxelX = viewPort[0] / 2;
	const float halfVoxelY = viewPort[1] / 2;
	const float x = mappedVoxelCoords[0] * viewPort[0] + viewPort[2] + halfVoxelX;
	const float y = mappedVoxelCoords[1] * viewPort[1] + viewPort[3] + halfVoxelY;
	return std::make_pair<int, int>( round( x ), round( y ) );
}

}
}
} // end namespace