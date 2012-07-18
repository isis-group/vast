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
 * orienttionhandler.cpp
 *
 * Description:
 *
 *  Created on: Mar 28, 2012
 *      Author: tuerke
 ******************************************************************/

#include "geometrical.hpp"

namespace isis
{
namespace viewer
{
namespace geometrical
{

BoundingBoxType getPhysicalBoundingBox ( const ImageHolder::Pointer image, const unsigned short &border )
{
	ImageHolder::Vector images;
	images.push_back( image );
	return getPhysicalBoundingBox( images, border  );
}


BoundingBoxType getPhysicalBoundingBox ( const ImageHolder::Vector images, const unsigned short &border )
{
	BoundingBoxType retBox;

	for( unsigned short i = 0; i < 3; i++ ) {
		retBox[i].first = std::numeric_limits<float>::max();
		retBox[i].second = -std::numeric_limits<float>::max();
	}

	BOOST_FOREACH( ImageHolder::Vector::const_reference image, images ) {
		const util::ivector4 imageSize = image->getISISImage()->getSizeAsVector();
		const util::fvector3 mappedVoxelSize = image->getImageProperties().latchedOrientation.dot( image->getImageProperties().voxelSize );

		for( unsigned short i = 0; i < 2; i++ ) {
			for( unsigned short j = 0; j < 2; j++ ) {
				for( unsigned short k = 0; k < 2; k++ ) {
					const util::ivector4 currentCorner ( i * ( imageSize[0] + border ), j * ( imageSize[1] + border ), k * ( imageSize[2] + border ) );
					const util::fvector3 currentPhysicalCorner = image->getISISImage()->getPhysicalCoordsFromIndex( currentCorner );

					for ( unsigned short l = 0; l < 3; l++ ) {
						if( currentPhysicalCorner[l] < retBox[l].first ) {
							retBox[l].first = currentPhysicalCorner[l] - ( mappedVoxelSize[l] / 2. );
						}

						if( currentPhysicalCorner[l] > retBox[l].second ) {
							retBox[l].second = currentPhysicalCorner[l] + ( mappedVoxelSize[l] / 2. );
						}
					}
				}
			}
		}
	}
	return retBox;
}


}
}
}