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
	image.setPropertyAs<isis::util::fvector4>( "sliceVec", isis::util::fvector4( 0, 0, 1 ) );
	image.setPropertyAs<isis::util::fvector4>( "indexOrigin", isis::util::fvector4( 0, 0, 0 ) );
}

std::string getFileFormatsAsString( const std::string preSeparator, const std::string postSeparator )
{
	std::stringstream fileFormats;
	BOOST_FOREACH( isis::data::IOFactory::FileFormatList::const_reference formatList, isis::data::IOFactory::getFormats() ) {
		BOOST_FOREACH( std::list<util::istring>::const_reference format, formatList->getSuffixes() ) {
			fileFormats << preSeparator << format << postSeparator;
		}
	}
	fileFormats.str().erase( fileFormats.str().size() -1 , 1);
	return fileFormats.str();

}


}
}