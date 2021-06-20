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
#include <isis/core/io_factory.hpp>
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
	image.setValueAs<isis::util::fvector3>( "rowVec", {1, 0, 0} );
	image.setValueAs<isis::util::fvector3>( "columnVec", { 0, 1, 0 } );
	image.setValueAs<isis::util::fvector3>( "sliceVec", { 0, 0, -1 } );
	image.setValueAs<isis::util::fvector3>( "indexOrigin", { 0, 0, 0 } );
	// updateOrientationMatrices(); @todo call me
}
void checkForCaCp( std::shared_ptr<ImageHolder> image )
{
	if( image->getISISImage()->hasProperty( "Vista/ca" ) ) {
		const auto &rowVec = image->getISISImage()->getValueAs<util::fvector3>( "rowVec" );
		const auto &columnVec = image->getISISImage()->getValueAs<util::fvector3>( "columnVec" );
		const auto &sliceVec = image->getISISImage()->getValueAs<util::fvector3>( "sliceVec" );
		const auto &voxelSize = image->getISISImage()->getValueAs<util::fvector3>( "voxelSize" );
		auto ca = util::stringToList<float>( image->getISISImage()->getValueAs<std::string>( "Vista/ca" ) );
		auto iter = ca.end();
		const util::fvector3 newIndexOrigin{ -*--iter, -*--iter, -*--iter };

		const util::fvector3 transformedOrigin =
				rowVec * newIndexOrigin[0] * voxelSize[0] +
				columnVec * newIndexOrigin[1] * voxelSize[1] +
				sliceVec * newIndexOrigin[2] * voxelSize[2];

		image->getISISImage()->setValueAs( "indexOrigin", transformedOrigin );
		image->updateOrientation();
		image->getImageProperties().physicalCoords = image->getPhysicalCoordsFromIndex( image->getImageProperties().voxelCoords );
	}
}

std::string getFileFormatsAsString( image_io::FileFormat::io_modes mode, const std::string preSeparator, const std::string postSeparator )
{
	std::stringstream fileFormats;
	for( const auto &formatList: isis::data::IOFactory::getFormats() ) {
		for( const auto &format: formatList->getSuffixes( mode ) ) {
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
	for( const auto &formatList: isis::data::IOFactory::getFormats() ) {
		for( const auto &format: formatList->getSuffixes( mode ) ) {
			retList.push_back( format.c_str() );
		}
	}
	return retList;
}

std::map< util::istring, std::list< util::istring > > getDialectsAsMap ( image_io::FileFormat::io_modes mode )
{
	std::map<util::istring, std::list< util::istring > > retMap;
	for( const auto  &format: isis::data::IOFactory::getFormats() ) {
		auto dialectList = format->dialects( );
		for( const auto &suffix: format->getSuffixes( mode ) ) {
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

util::fvector3 mapCoordsToOrientation( const util::ivector3 &coords, const util::Matrix3x3<float> &orientationMatrix, PlaneOrientation orientation, bool back, bool absolute )
{
	util::Matrix3x3<float> transformMatrix = util::identityMatrix<float, 3>();
	util::fvector3 retVec;

	switch ( orientation ) {
	case axial:
		/*setup axial matrix
		*-1  0  0
		* 0 -1  0
		* 0  0  1
		*/
		transformMatrix[0][0] = -1;
		transformMatrix[1][1] = 1;
		break;
	case sagittal:
		/*setup sagittal matrix
		* 0  1  0
		* 0  0  1
		* 1  0  0
		*/
		transformMatrix[0][0] = 0;
		transformMatrix[0][2] = 1;
		transformMatrix[1][0] = 1;
		transformMatrix[2][2] = 0;
		transformMatrix[2][1] = -1;
		transformMatrix[1][1] = 0;
		break;
	case coronal:
		/*setup coronal matrix
		* -1  0  0
		*  0  0  1
		*  0  1  0
		*/

		transformMatrix[0][0] = -1;
		transformMatrix[1][1] = 0;
		transformMatrix[2][2] = 0;
		transformMatrix[1][2] = 1;
		transformMatrix[2][1] = -1;
		break;
	case not_specified:
		LOG( Runtime, warning ) << "Can not transform to PlaneOrientation \"not_specified\"!";
		break;
	}

	const auto transform = transformMatrix * orientationMatrix;
	if( back ) {
		retVec = util::transpose(transform) * coords ;
	} else {
		retVec = transform * coords;
	}

	if( absolute ) {
		return { fabsf( retVec[0] ), fabsf( retVec[1] ), fabsf( retVec[2] ) };
	} else {
		return retVec;
	}

}


}
}