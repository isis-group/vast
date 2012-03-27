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
 * geomhandler.cpp
 *
 * Description:
 *
 *  Created on: Mar 27, 2012
 *      Author: tuerke
 ******************************************************************/

#include "geomhandler.hpp"
#include <Adapter/qmatrixconversion.hpp>

namespace isis {
namespace viewer {
namespace widget {
namespace _internal {

util::fvector4 getPhysicalBoundingBox ( const ImageHolder::Vector images, const PlaneOrientation &orientation )
{
	BoundingBoxType currentBoundingBox;
	BOOST_FOREACH( ImageHolder::Vector::const_reference image, images )
	{
		BoundingBoxType tmpBoundingBox;
		const util::ivector4 imageSize = image->getISISImage()->getSizeAsVector();
		for( unsigned short i = 0; i<2;i++ ) {
			for( unsigned short j = 0; j<2;j++ ) {
				for( unsigned short k = 0; k<2;k++ ) {
					const util::ivector4 currentCorner ( i * imageSize[0], j * imageSize[1], k * imageSize[2] );
					const util::fvector4 currentPhysicalCorner = image->getISISImage()->getPhysicalCoordsFromIndex(currentCorner);
					for ( unsigned short l = 0; l < 3; l++ ) {
						if( currentPhysicalCorner[l] < currentBoundingBox[l].first ) {
							currentBoundingBox[l].first = currentPhysicalCorner[l];
						}
						if( currentPhysicalCorner[l] > currentBoundingBox[l].second ) {
							currentBoundingBox[l].second = currentPhysicalCorner[l];
						}
					}
				}
			}
		}
	}
	util::fvector4 ret;
	switch(orientation ) {
		case axial:
			ret = util::fvector4( 	currentBoundingBox[0].first,
									currentBoundingBox[1].first,
									fabs( currentBoundingBox[0].second - currentBoundingBox[0].first ),
									fabs( currentBoundingBox[1].second - currentBoundingBox[1].first ) );
			break;
		case sagittal:
			ret = util::fvector4(	currentBoundingBox[1].first,
									currentBoundingBox[2].first,
									fabs(  currentBoundingBox[1].second - currentBoundingBox[1].first ),
									fabs(  currentBoundingBox[2].second - currentBoundingBox[2].first ) );
			break;
		case coronal:
			ret = util::fvector4(	currentBoundingBox[0].first,
									currentBoundingBox[2].first,
									fabs(  currentBoundingBox[0].second - currentBoundingBox[0].first ),
									fabs(  currentBoundingBox[2].second - currentBoundingBox[2].first ) );
			break;
	}
	return ret;
}

QTransform getQTransform ( const ImageHolder::Pointer image, const PlaneOrientation& orientation )
{
	image->getImageProperties().orientation;
	QTransform retTransform;
	return retTransform.translate(-79.5,-99.5);
	
	
	
}

	

}}}}