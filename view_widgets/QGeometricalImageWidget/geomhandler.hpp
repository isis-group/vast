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

const uint8_t rasteringFac = 100;

util::fvector4 getPhysicalBoundingBox( const ImageHolder::Vector &images, const PlaneOrientation &orientation );

QTransform getQTransform( const ImageHolder::Pointer image, const PlaneOrientation &orientation, const bool &latched );

QTransform getTransform2ISISSpace( const PlaneOrientation &orientation, const util::fvector4 &bb );

util::Matrix3x3<qreal> getOrderedMatrix(  const boost::shared_ptr<ImageHolder> image, bool inverse, const bool &latched );

util::FixedMatrix<qreal, 2, 2> extract2DMatrix( const boost::shared_ptr<ImageHolder> image, const PlaneOrientation &orientation, bool inverse, const bool &latched );

util::fvector3 mapPhysicalCoords2Orientation( const util::fvector3 &coords, const PlaneOrientation &orientation );

void zoomBoundingBox( util::fvector4 &boundingBox, util::FixedVector<float, 2> &translation, const util::fvector3 &physCoord, const float &zoom, const PlaneOrientation &orientation, const bool &translate );

}
}
}
}// end namespace


#endif // VAST_GEOMHANDLER_HPP