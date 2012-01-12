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
 * common.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "common.hpp"
#include <DataStorage/io_factory.hpp>
#include "imageholder.hpp"
#include <boost/assign.hpp>
#include <numeric>

namespace isis
{
namespace viewer
{

void setOrientationToIdentity( data::Image &image )
{
	image.setPropertyAs<isis::util::fvector4>( "rowVec", isis::util::fvector4( 1, 0, 0 ) );
	image.setPropertyAs<isis::util::fvector4>( "columnVec", isis::util::fvector4( 0, 1, 0 ) );
	image.setPropertyAs<isis::util::fvector4>( "sliceVec", isis::util::fvector4( 0, 0, -1 ) );
	image.setPropertyAs<isis::util::fvector4>( "indexOrigin", isis::util::fvector4( 0, 0, 0 ) );
	image.updateOrientationMatrices();
}
void checkForCaCp( boost::shared_ptr<ImageHolder> image )
{
	if( image->getISISImage()->hasProperty( "Vista/ca" ) ) {
		const util::fvector4 &rowVec = image->getISISImage()->getPropertyAs<util::fvector4>( "rowVec" );
		const util::fvector4 &columnVec = image->getISISImage()->getPropertyAs<util::fvector4>( "columnVec" );
		const util::fvector4 &sliceVec = image->getISISImage()->getPropertyAs<util::fvector4>( "sliceVec" );
		const util::fvector4 &voxelSize = image->getISISImage()->getPropertyAs<util::fvector4>( "voxelSize" );
		std::list<float> ca = util::stringToList<float>( image->getISISImage()->getPropertyAs<std::string>( "Vista/ca" ) );
		std::list<float>::const_iterator iter = ca.end();
		const util::fvector4 newIndexOrigin( -*--iter, -*--iter, -*--iter );
		const util::fvector4 transformedOrigin = util::fvector4(
					newIndexOrigin[0] * rowVec[0] * voxelSize[0] + newIndexOrigin[1] * columnVec[0] * voxelSize[1] + newIndexOrigin[2] * sliceVec[0] * voxelSize[2],
					newIndexOrigin[0] * rowVec[1] * voxelSize[0] + newIndexOrigin[1] * columnVec[1] * voxelSize[1] + newIndexOrigin[2] * sliceVec[1] * voxelSize[2],
					newIndexOrigin[0] * rowVec[2] * voxelSize[0] + newIndexOrigin[1] * columnVec[2] * voxelSize[1] + newIndexOrigin[2] * sliceVec[2] * voxelSize[2],
					newIndexOrigin[3] );
		image->getISISImage()->setPropertyAs<util::fvector4>( "indexOrigin", transformedOrigin );
		image->getISISImage()->updateOrientationMatrices();
		image->physicalCoords = image->getISISImage()->getPhysicalCoordsFromIndex( image->voxelCoords );
	}
}

std::string getFileFormatsAsString( image_io::FileFormat::io_modes mode, const std::string preSeparator, const std::string postSeparator )
{
	std::stringstream fileFormats;
	BOOST_FOREACH( isis::data::IOFactory::FileFormatList::const_reference formatList, isis::data::IOFactory::getFormats() ) {
		BOOST_FOREACH( std::list<util::istring>::const_reference format, formatList->getSuffixes( mode ) ) {
			fileFormats << preSeparator << format << postSeparator;
		}
	}
	fileFormats.str().erase( fileFormats.str().size() - 1 , 1 );
	return fileFormats.str();

}

std::list<util::istring> getFileFormatsAsList( image_io::FileFormat::io_modes mode )
{
	std::list<util::istring> retList;
	BOOST_FOREACH( isis::data::IOFactory::FileFormatList::const_reference formatList, isis::data::IOFactory::getFormats() ) {
		BOOST_FOREACH( std::list<util::istring>::const_reference format, formatList->getSuffixes( mode ) ) {
			retList.push_back( format.c_str() );
		}
	}
	return retList;
}

util::ivector4 get32BitAlignedSize( const util::ivector4 &origSize )
{
	util::ivector4 retSize;

	for ( size_t i = 0; i < 4; i++ ) {
		int m = origSize[i] % 4;

		if( m > 0 ) {
			retSize[i] = origSize[i] + 4 - m;
		} else {
			retSize[i] = origSize[i];
		}
	}

	return retSize;
}

std::list< std::string > getSupportedTypeList()
{
	return boost::assign::list_of<std::string>( "boolean" )( "s8bit" )( "u8bit" )( "s16bit" )( "u16bit" )( "s32bit" )( "u32bit" )( "s64bit" )( "u64bit" )( "float" )( "double" )( "color24" )( "color32" );
}

size_t getInternalImageRange()
{
    return std::numeric_limits<InternalImageType>::max() - std::numeric_limits<InternalImageType>::min();
}


}
}