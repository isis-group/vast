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
 * common.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#pragma once

#include <vector>
#include <isis/core/chunk.hpp>
#include <isis/core/image.hpp>
#include <isis/core/common.hpp>
#include <isis/core/types.hpp>
#include <isis/core/io_interface.h>
#include <isis/core/matrix.hpp>

namespace isis
{
struct ViewerLog {static constexpr char name[]="Viewer";      static constexpr bool use = _ENABLE_LOG;};
struct ViewerDev {static constexpr char name[]="ViewerDebug"; static constexpr bool use = _ENABLE_DEBUG;};

namespace viewer
{

class ImageHolder;
typedef uint8_t InternalImageType;
typedef isis::util::color24 InternalImageColorType;

enum PlaneOrientation { axial, sagittal, coronal, not_specified };
enum InterpolationType { nn = 0, lin };

const unsigned short dim_time = 3;


template<typename TYPE>
TYPE roundNumber( TYPE number, unsigned  short placesOfDec )
{
	return floor( number * pow( 10., placesOfDec ) + .5 ) / pow( 10., placesOfDec );
}

void setOrientationToIdentity( data::Image &image );
void checkForCaCp( std::shared_ptr<ImageHolder> image );
std::string getFileFormatsAsString( image_io::FileFormat::io_modes mode, const std::string preSeparator, const std::string postSeparator = std::string( " " ) );

std::list<util::istring> getFileFormatsAsList( image_io::FileFormat::io_modes mode );
std::map<util::istring, std::list< util::istring > > getDialectsAsMap( image_io::FileFormat::io_modes mode );
std::list<std::string> getSupportedTypeList() ;


typedef ViewerLog Runtime;
typedef ViewerDev Dev;

template<typename T, int C> std::array<T, C> mapCoordsToOrientation( const std::array<T, C> &coords, const util::Matrix3x3<float> &orientationMatrix, PlaneOrientation orientation, bool back = false, bool absolute = true ){
	const auto mapped=mapCoordsToOrientation<float,3>({coords[0],coords[1],coords[2]},orientationMatrix,orientation,back,absolute);
	std::array<T, C> ret;
	std::copy(std::begin(mapped),std::end(mapped),std::begin(ret));
	std::fill(std::begin(ret)+3,std::end(ret),0);
	return ret;
}

util::fvector3 mapCoordsToOrientation( const util::fvector3 &coords, const util::Matrix3x3<float> &orientationMatrix, PlaneOrientation orientation, bool back=false, bool absolute = true );

std::string getCrashLogFilePath();

}
}
#include "imageholder.hpp"
