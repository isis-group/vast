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
 * datacontainer.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "datacontainer.hpp"

namespace isis
{
namespace viewer
{

boost::shared_ptr<ImageHolder> DataContainer::addImage( const data::Image &image, const ImageHolder::ImageType &imageType )
{
	std::string fileName;

	if( image.hasProperty( "source" ) ) {
		fileName = image.getPropertyAs<std::string>( "source" );
	} else {
		boost::filesystem::path path = image.getChunk( 0 ).getPropertyAs<std::string>( "source" );
		fileName = path.branch_path().string();
	}

	std::string newFileName = fileName;

	if( find( fileName ) != end() ) {
		size_t index = 0;

		while ( find( newFileName ) != end() ) {
			std::stringstream ss;
			ss << fileName << " (" << ++index << ")";
			newFileName = ss.str();
		}
	}

	boost::shared_ptr<ImageHolder>  tmpHolder = boost::shared_ptr<ImageHolder> ( new ImageHolder ) ;
	tmpHolder->setImage( image, imageType, newFileName );
	tmpHolder->setID( size()  );
	insert( std::make_pair<std::string, boost::shared_ptr<ImageHolder> >( newFileName, tmpHolder ) );
	return tmpHolder;
}

boost::shared_ptr< ImageHolder > DataContainer::getImageByID( short unsigned int id ) const
{
	BOOST_FOREACH( DataContainer::const_reference image, *this ) {
		if( image.second->getID() == id ) {
			return image.second;
		}
	}
	LOG( Runtime, warning ) << "There is no image with id " << id << " !";
	return boost::shared_ptr<ImageHolder>();
}


}
} // end namespace