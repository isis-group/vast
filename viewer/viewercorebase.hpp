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
 * viewercorebase.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef VIEWERCOREBASE_HPP
#define VIEWERCOREBASE_HPP

#include "datacontainer.hpp"
#include <map>
#include "pluginloader.hpp"
#include "widgetloader.hpp"


#ifdef _OPENMP
#include <omp.h>
#endif /*_OPENMP*/

namespace isis
{
namespace viewer
{

class ViewerCoreBase
{

public:

	enum Mode { standard, zmap };

	ViewerCoreBase( );

	std::string getVersion() const;

	virtual ImageHolder::ImageListType addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );
	virtual void setImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );
	virtual boost::shared_ptr<ImageHolder> addImage( const data::Image &image, const ImageHolder::ImageType &imageType );

	void setCurrentImage( const boost::shared_ptr<ImageHolder> image ) { m_CurrentImage = image; }

	boost::shared_ptr<ImageHolder> getCurrentImage();

	boost::shared_ptr<ImageHolder> getCurrentAnatomicalRefernce() const { return m_CurrentAnatomicalReference; }

	const DataContainer &getDataContainer() const { return m_DataContainer; }
	DataContainer &getDataContainer() { return m_DataContainer; }
	ImageHolder::ImageListType getImageList() const { return m_ImageList; }

	boost::shared_ptr<util::PropertyMap>  getOptionMap() { return m_OptionsMap; }

	bool hasImage() const { return getDataContainer().size() && m_CurrentImage.get(); }

	Mode getMode() const { return m_Mode; }


private:
	//this is the container which actually holds all the images
	DataContainer m_DataContainer;
	boost::shared_ptr<ImageHolder>  m_CurrentImage;
	void setCommonViewerOptions();

protected:
	boost::shared_ptr<util::PropertyMap> m_OptionsMap;
	//additional imagelist for finding purpose
	ImageHolder::ImageListType m_ImageList;

	boost::shared_ptr<ImageHolder> m_CurrentAnatomicalReference;
	Mode m_Mode;



};




}
} // end namespace



#endif