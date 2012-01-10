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


util::fvector4 QOrientationHandler::mapCoordsToOrientation( const util::fvector4 &coords, const boost::shared_ptr< ImageHolder > image, PlaneOrientation orientation, bool back, bool absolute )
{
	using namespace boost::numeric::ublas;
	matrix<float> transformMatrix = identity_matrix<float>( 4, 4 );
	matrix<float> finMatrix = identity_matrix<float>( 4, 4 );
	vector<float> vec = vector<float>( 4 );
	vector<float> finVec = vector<float>( 4 );

	for( size_t i = 0; i < 4 ; i++ ) {
		vec( i ) = coords[i];
	}

	switch ( orientation ) {
	case axial:
		/*setup axial matrix
		*-1  0  0
		* 0 -1  0
		* 0  0  1
		*/
		transformMatrix( 0, 0 ) = -1;
		transformMatrix( 1, 1 ) = 1;
		break;
	case sagittal:
		/*setup sagittal matrix
		* 0  1  0
		* 0  0  1
		* 1  0  0
		*/
		transformMatrix( 0, 0 ) = 0;
		transformMatrix( 2, 0 ) = 1;
		transformMatrix( 0, 1 ) = 1;
		transformMatrix( 2, 2 ) = 0;
		transformMatrix( 1, 2 ) = -1;
		transformMatrix( 1, 1 ) = 0;
		break;
	case coronal:
		/*setup coronal matrix
		* -1  0  0
		*  0  0  1
		*  0  1  0
		*/

		transformMatrix( 0, 0 ) = -1;
		transformMatrix( 1, 1 ) = 0;
		transformMatrix( 2, 2 ) = 0;
		transformMatrix( 2, 1 ) = 1;
		transformMatrix( 1, 2 ) = -1;
		break;
	}

	if( back ) {
		finVec = prod( trans( prod(  transformMatrix, image->latchedOrientation ) ), vec );
	} else {
		finVec = prod( prod( transformMatrix, image->latchedOrientation ) , vec );
	}

	if( absolute ) {
		return util::fvector4( fabs( finVec( 0 ) ), fabs( finVec( 1 ) ), fabs( finVec( 2 ) ) , fabs( finVec( 3 ) ) );
	} else {
		return util::fvector4( finVec( 0 ), finVec( 1 ), finVec( 2 ), finVec( 3 ) );
	}

}


QOrientationHandler::ViewPortType QOrientationHandler::getViewPort(  const float &zoom, const boost::shared_ptr< ImageHolder > image, const size_t &w, const size_t &h, PlaneOrientation orientation, unsigned short border )
{
	ViewPortType viewPort;
	const util::ivector4 mappedSize = QOrientationHandler::mapCoordsToOrientation( image->getImageSize(), image, orientation );
	const util::fvector4 mappedScaling = QOrientationHandler::mapCoordsToOrientation( image->voxelSize, image, orientation );
	const util::fvector4 physSize = mappedScaling * mappedSize;
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
	const util::ivector4 mappedSize = QOrientationHandler::mapCoordsToOrientation( image->getImageSize(), image, orientation );
	const util::fvector4 flipVec = QOrientationHandler::mapCoordsToOrientation( util::fvector4( 1, 1, 1 ), image, orientation, false, false );
	const util::ivector4 mappedVoxelCoords = QOrientationHandler::mapCoordsToOrientation( image->voxelCoords, image, orientation );
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
	const util::ivector4 mappedSize = QOrientationHandler::mapCoordsToOrientation( image->getImageSize(), image, orientation, false, false );
	const size_t voxCoordX = ( x - viewPort[2] ) / viewPort[0];
	const size_t voxCoordY = ( y - viewPort[3] ) / viewPort[1];
	util::ivector4 coords =  util::ivector4( voxCoordX, voxCoordY, slice );

	#pragma omp parallel for

	for ( short i = 0; i < 2; i++ ) {
		coords[i] = mappedSize[i] < 0 ? abs( mappedSize[i] ) - coords[i] - 1 : coords[i];
		coords[i] = coords[i] < 0 ? 0 : coords[i];
		coords[i] = coords[i] >= abs( mappedSize[i] ) ? abs( mappedSize[i] ) - 1 : coords[i];
	}

	coords[3] = image->voxelCoords[3];
	return QOrientationHandler::mapCoordsToOrientation( coords, image, orientation, true );
}

std::pair< int, int > QOrientationHandler::convertVoxel2WindowCoords( const ViewPortType &viewPort, const boost::shared_ptr< ImageHolder > image, PlaneOrientation orientation )
{
	const util::ivector4 mappedVoxelCoords = QOrientationHandler::mapCoordsToOrientation( image->voxelCoords, image, orientation );
	const float halfVoxelX = viewPort[0] / 2;
	const float halfVoxelY = viewPort[1] / 2;
	const float x = mappedVoxelCoords[0] * viewPort[0] + viewPort[2] + halfVoxelX;
	const float y = mappedVoxelCoords[1] * viewPort[1] + viewPort[3] + halfVoxelY;
	return std::make_pair<int, int>( round( x ), round( y ) );
}

}
} // end namespace