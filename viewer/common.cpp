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

#include <QDir>

namespace isis
{
namespace viewer
{

void setOrientationToIdentity( data::Image &image )
{
	image.setPropertyAs<isis::util::fvector3>( "rowVec", isis::util::fvector3( 1, 0, 0 ) );
	image.setPropertyAs<isis::util::fvector3>( "columnVec", isis::util::fvector3( 0, 1, 0 ) );
	image.setPropertyAs<isis::util::fvector3>( "sliceVec", isis::util::fvector3( 0, 0, -1 ) );
	image.setPropertyAs<isis::util::fvector3>( "indexOrigin", isis::util::fvector3( 0, 0, 0 ) );
	image.updateOrientationMatrices();
}
void checkForCaCp( boost::shared_ptr<ImageHolder> image )
{
	if( image->getISISImage()->hasProperty( "Vista/ca" ) ) {
		const util::fvector3 &rowVec = image->getISISImage()->getPropertyAs<util::fvector3>( "rowVec" );
		const util::fvector3 &columnVec = image->getISISImage()->getPropertyAs<util::fvector3>( "columnVec" );
		const util::fvector3 &sliceVec = image->getISISImage()->getPropertyAs<util::fvector3>( "sliceVec" );
		const util::fvector3 &voxelSize = image->getISISImage()->getPropertyAs<util::fvector3>( "voxelSize" );
		std::list<float> ca = util::stringToList<float>( image->getISISImage()->getPropertyAs<std::string>( "Vista/ca" ) );
		std::list<float>::const_iterator iter = ca.end();
		const util::fvector3 newIndexOrigin( -*--iter, -*--iter, -*--iter );
		const util::fvector3 transformedOrigin = util::fvector3(
					newIndexOrigin[0] * rowVec[0] * voxelSize[0] + newIndexOrigin[1] * columnVec[0] * voxelSize[1] + newIndexOrigin[2] * sliceVec[0] * voxelSize[2],
					newIndexOrigin[0] * rowVec[1] * voxelSize[0] + newIndexOrigin[1] * columnVec[1] * voxelSize[1] + newIndexOrigin[2] * sliceVec[1] * voxelSize[2],
					newIndexOrigin[0] * rowVec[2] * voxelSize[0] + newIndexOrigin[1] * columnVec[2] * voxelSize[1] + newIndexOrigin[2] * sliceVec[2] * voxelSize[2] );
		image->getISISImage()->setPropertyAs<util::fvector3>( "indexOrigin", transformedOrigin );
		image->updateOrientation();
		image->getImageProperties().physicalCoords = image->getISISImage()->getPhysicalCoordsFromIndex( image->getImageProperties().voxelCoords );
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
	if(fileFormats.str().empty())
		return "";
	else {
		fileFormats.str().erase( fileFormats.str().size() - 1 , 1 );
		return fileFormats.str();
	}

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

std::map< util::istring, std::list< util::istring > > getDialectsAsMap ( image_io::FileFormat::io_modes mode )
{
	std::map<util::istring, std::list< util::istring > > retMap;
	BOOST_FOREACH( isis::data::IOFactory::FileFormatList::const_reference format, isis::data::IOFactory::getFormats() ) {
		std::list<util::istring> dialectList = util::stringToList<util::istring>( format->dialects( "" ).c_str() );
		BOOST_FOREACH( std::list< util::istring>::const_reference suffix, format->getSuffixes( mode ) ) {
			retMap[suffix.c_str()]  = dialectList;
		}
	}
	return retMap;
}



std::list< std::string > getSupportedTypeList()
{
	return boost::assign::list_of<std::string>( "boolean" )( "s8bit" )( "u8bit" )( "s16bit" )( "u16bit" )( "s32bit" )( "u32bit" )( "s64bit" )( "u64bit" )( "float" )( "double" )( "color24" )( "color32" );
}

std::string getCrashLogFilePath()
{
	const std::string homePath = QDir::homePath().toStdString();
	std::stringstream logFilePath;
	logFilePath << homePath << "/vastCrashLog.log";
	return logFilePath.str();
}
util::fvector4 mapCoordsToOrientation ( const util::fvector4& coords, const isis::util::Matrix3x3< float >& orientationMatrix, PlaneOrientation orientation, bool back, bool absolute )
{
	util::fvector3 ret = mapCoordsToOrientation( util::fvector3( coords[0], coords[1], coords[2] ), orientationMatrix, orientation, back, absolute );
	return util::fvector4( ret[0], ret[1], ret[2] );
}


util::fvector3 mapCoordsToOrientation( const util::fvector3 &coords, const util::Matrix3x3<float> &orientationMatrix, PlaneOrientation orientation, bool back, bool absolute )
{
	util::Matrix3x3<float> transformMatrix = util::IdentityMatrix<float, 3>();
	util::fvector3 retVec;

	switch ( orientation ) {
	case axial:
		/*setup axial matrix
		*-1  0  0
		* 0 -1  0
		* 0  0  1
		*/
		transformMatrix.elem( 0, 0 ) = -1;
		transformMatrix.elem( 1, 1 ) = 1;
		break;
	case sagittal:
		/*setup sagittal matrix
		* 0  1  0
		* 0  0  1
		* 1  0  0
		*/
		transformMatrix.elem( 0, 0 ) = 0;
		transformMatrix.elem( 0, 2 ) = 1;
		transformMatrix.elem( 1, 0 ) = 1;
		transformMatrix.elem( 2, 2 ) = 0;
		transformMatrix.elem( 2, 1 ) = -1;
		transformMatrix.elem( 1, 1 ) = 0;
		break;
	case coronal:
		/*setup coronal matrix
		* -1  0  0
		*  0  0  1
		*  0  1  0
		*/

		transformMatrix.elem( 0, 0 ) = -1;
		transformMatrix.elem( 1, 1 ) = 0;
		transformMatrix.elem( 2, 2 ) = 0;
		transformMatrix.elem( 1, 2 ) = 1;
		transformMatrix.elem( 2, 1 ) = -1;
		break;
	case not_specified:
		LOG( Runtime, warning ) << "Can not transform to PlaneOrientation \"not_specified\"!";
		break;
	}

	if( back ) {
		retVec = transformMatrix.dot( orientationMatrix ).transpose().dot( coords );
	} else {
		retVec = transformMatrix.dot( orientationMatrix ).dot( coords );
	}

	if( absolute ) {
		return util::fvector3( fabs( retVec[0] ), fabs( retVec[1] ), fabs( retVec[2] )  );
	} else {
		return retVec;
	}

}


}
}