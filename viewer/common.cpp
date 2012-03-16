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

#include <QDir>

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

std::map< util::istring, std::list< util::istring > > getDialectsAsMap ( image_io::FileFormat::io_modes mode )
{
	std::map<util::istring, std::list< util::istring > > retMap;
	BOOST_FOREACH( isis::data::IOFactory::FileFormatList::const_reference format, isis::data::IOFactory::getFormats() ) {
		std::list<util::istring> dialectList = util::stringToList<util::istring>(format->dialects("").c_str());
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

util::fvector4 mapCoordsToOrientation( const util::fvector4 &coords, const boost::numeric::ublas::matrix<float> &orientationMatrix, PlaneOrientation orientation, bool back, bool absolute )
{
	using namespace boost::numeric::ublas;
	matrix<float> transformMatrix = identity_matrix<float>( 4, 4 );
	matrix<float> finMatrix = identity_matrix<float>( 4, 4 );
	vector<float> vec = vector<float>( 4 );
	vector<float> finVec = vector<float>( 4 );

	for( size_t i = 0; i < 4 ; i++ ) {
		vec( i ) = coords[i];
	}

	switch ( orientation ) {
	case axial:
		/*setup axial matrix
		*-1  0  0
		* 0 -1  0
		* 0  0  1
		*/
		transformMatrix( 0, 0 ) = -1;
		transformMatrix( 1, 1 ) = 1;
		break;
	case sagittal:
		/*setup sagittal matrix
		* 0  1  0
		* 0  0  1
		* 1  0  0
		*/
		transformMatrix( 0, 0 ) = 0;
		transformMatrix( 2, 0 ) = 1;
		transformMatrix( 0, 1 ) = 1;
		transformMatrix( 2, 2 ) = 0;
		transformMatrix( 1, 2 ) = -1;
		transformMatrix( 1, 1 ) = 0;
		break;
	case coronal:
		/*setup coronal matrix
		* -1  0  0
		*  0  0  1
		*  0  1  0
		*/

		transformMatrix( 0, 0 ) = -1;
		transformMatrix( 1, 1 ) = 0;
		transformMatrix( 2, 2 ) = 0;
		transformMatrix( 2, 1 ) = 1;
		transformMatrix( 1, 2 ) = -1;
		break;
	case not_specified:
		LOG( Runtime, warning ) << "Can not transform to PlaneOrientation \"not_specified\"!";
		break;
	}
	

	if( back ) {
		finVec = prod( trans( prod(  transformMatrix, orientationMatrix ) ), vec );
	} else {
		finVec = prod( prod( transformMatrix, orientationMatrix ) , vec );
	}

	if( absolute ) {
		return util::fvector4( fabs( finVec( 0 ) ), fabs( finVec( 1 ) ), fabs( finVec( 2 ) ) , fabs( finVec( 3 ) ) );
	} else {
		return util::fvector4( finVec( 0 ), finVec( 1 ), finVec( 2 ), finVec( 3 ) );
	}

}


}
}