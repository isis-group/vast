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
 * datacontainer.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef DATACONTAINER_HPP
#define DATACONTAINER_HPP

#include "imageholder.hpp"
#include <boost/filesystem/path.hpp>

namespace isis
{
namespace viewer
{

/**
 * This class is a std::vector which holds all images as an object of ImageHolder.
 * Furthermore it provides some convinient functions to add images and getting the
 * pointer to the image data.
 */
class DataContainer : public std::map<std::string, boost::shared_ptr<ImageHolder> >
{
public:
	///simply adds an isis image to the vector
	boost::shared_ptr<ImageHolder> addImage( const data::Image &image, const ImageHolder::ImageType &imageType );

	boost::shared_ptr<ImageHolder> getImageByID( unsigned short id ) const;

	///returns a boost::shared_ptr_ptr of the images data. Actually this also is a convinient function.
	boost::shared_ptr<const void>
	getRawAdress( const boost::shared_ptr<ImageHolder> image, size_t timestep = 0 ) const {
		return image->getRawAdress( timestep );
	}


};


}
} // end namespace

#endif