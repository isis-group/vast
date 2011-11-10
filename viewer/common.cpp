#include "common.hpp"
#include <DataStorage/io_factory.hpp>

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

std::list<std::string> getFileFormatsAsList( image_io::FileFormat::io_modes mode )
{
	std::list<std::string> retList;
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

}
}