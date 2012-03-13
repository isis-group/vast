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
 * QOrienationHandler.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef QORIENTATIONHANDLER_HPP
#define QORIENTATIONHANDLER_HPP

#include <QTransform>
#include "imageholder.hpp"
#include "common.hpp"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/concept_check.hpp>
#include <math.h>

namespace isis
{
namespace viewer
{
namespace widget
{
class QOrientationHandler
{

public:
	typedef  util::FixedVector<float, 6 > ViewPortType;

	static ViewPortType getViewPort( const float &zoom, const boost::shared_ptr< ImageHolder > image, const size_t &w, const size_t &h, PlaneOrientation orientation, unsigned short border = 0 );
	static QTransform getTransform( const ViewPortType &viewPort, const boost::shared_ptr< ImageHolder > image, PlaneOrientation orientation );

	static util::ivector4 convertWindow2VoxelCoords( const ViewPortType &viewPort, const boost::shared_ptr< ImageHolder > image, const size_t &x, const size_t &y, const size_t &slice, PlaneOrientation orientation );
	static std::pair<int, int> convertVoxel2WindowCoords( const ViewPortType &viewPort, const boost::shared_ptr< ImageHolder > image, PlaneOrientation orientation );
};


}
}
}// end namespace


#endif