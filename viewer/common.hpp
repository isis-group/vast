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
 * common.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef VIEWER_COMMON_HPP_
#define VIEWER_COMMON_HPP_

#include <vector>
#include <DataStorage/chunk.hpp>
#include <DataStorage/image.hpp>
#include <boost/shared_ptr.hpp>
#include <DataStorage/common.hpp>
#include <CoreUtils/common.hpp>
#include <CoreUtils/types.hpp>
#include <DataStorage/io_interface.h>


namespace isis
{

struct ViewerLog {static const char *name() {return "Viewer";}; enum {use = _ENABLE_LOG};};
struct ViewerDebug {static const char *name() {return "ViewerDebug";}; enum {use = _ENABLE_DEBUG};};

namespace viewer
{
class ImageHolder;
typedef uint8_t InternalImageType;
typedef isis::util::color24 InternalImageColorType;

enum PlaneOrientation { axial, sagittal, coronal };
enum WidgetType { type_gl, type_qt };
enum InterpolationType { nn = 0, lin };

template<typename TYPE>
TYPE roundNumber( TYPE number, unsigned  short placesOfDec )
{
	return floor( number * pow( 10., placesOfDec ) + .5 ) / pow( 10., placesOfDec );
}

void setOrientationToIdentity( data::Image &image );
void checkForCaCp( boost::shared_ptr<ImageHolder> image );
std::string getFileFormatsAsString( image_io::FileFormat::io_modes mode, const std::string preSeparator, const std::string postSeparator = std::string( " " ) );
std::list<util::istring> getFileFormatsAsList( image_io::FileFormat::io_modes mode );
std::list<std::string> getSupportedTypeList() ;

util::ivector4 get32BitAlignedSize( const util::ivector4 &origSize );

typedef ViewerLog Runtime;
typedef ViewerDebug Debug;
template<typename HANDLE> void enable_log( LogLevel level )
{
	ENABLE_LOG( Runtime, HANDLE, level );
	ENABLE_LOG( Debug, HANDLE, level );
}

}
}
#include "imageholder.hpp"
#endif /* VIEWER_COMMON_HPP_ */
