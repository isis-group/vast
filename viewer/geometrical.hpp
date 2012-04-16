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
 * orienttionhandler.hpp
 *
 * Description:
 *
 *  Created on: Mar 28, 2012
 *      Author: tuerke
 ******************************************************************/
#ifndef VAST_GEOMETRICAL_HPP
#define VAST_GEOMETRICAL_HPP

#include <CoreUtils/vector.hpp>


namespace isis
{
namespace viewer
{
class ImageHolder;
namespace geometrical
{

///l:r, a:p, s:i
typedef util::FixedVector<std::pair<float, float>, 3 > BoundingBoxType;

namespace _internal
{
}

BoundingBoxType getPhysicalBoundingBox( const boost::shared_ptr<ImageHolder> image, const unsigned short &border = 0 );

BoundingBoxType getPhysicalBoundingBox ( const std::vector<boost::shared_ptr<ImageHolder> > images, const unsigned short &border = 0 );

}
}
}
#include "imageholder.hpp"

#endif // VAST_GEOMETRICAL_HPP