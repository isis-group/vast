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

#include "pluginloader.hpp"
#include "widgetloader.hpp"
#include "widgetinterface.h"
#include "settings.hpp"

#include <map>
#include <boost/uuid/uuid.hpp>
#include <boost/signals2.hpp>

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

	enum Mode { default_mode, statistical_mode };

	ViewerCoreBase( );

	static std::string getVersion();

	virtual ImageHolder::List addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );
	virtual ImageHolder::Pointer addImage( const data::Image &image, const ImageHolder::ImageType &imageType );

	const boost::shared_ptr<Settings> getSettings() const { return m_Settings; }
	boost::shared_ptr<Settings> getSettings() { return m_Settings; }
	
	void setCurrentImage( const ImageHolder::Pointer image ) { m_CurrentImage = image; emitCurrentImageChanged( image ); }

	ImageHolder::Pointer getCurrentImage() const;

	ImageHolder::Pointer getCurrentAnatomicalRefernce() const { return m_CurrentAnatomicalReference; }

	bool hasImage() const { return m_ImageList.size() && m_CurrentImage.get(); }

	widget::WidgetInterface *getWidget( const std::string &identifier ) throw( std::runtime_error & );
	const util::PropertyMap *getWidgetProperties( const std::string &identifier ) ;

	virtual void setMode( const Mode &mode ) { m_Mode = mode; }
	virtual Mode getMode() const { return m_Mode; }

	ImageHolder::List &getImageList() { return m_ImageList; }
	const ImageHolder::List &getImageList() const { return m_ImageList; }

	ImageHolder::Map &getImageMap() { return m_ImageMap; }
	const ImageHolder::Map &getImageMap() const { return m_ImageMap; }

	//signals
	boost::signals2::signal<void (ImageHolder::Pointer)> emitAddImage;
	boost::signals2::signal<void (ImageHolder::Pointer)> emitCurrentImageChanged;

private:
	//this is the container which holds all the images
	ImageHolder::List m_ImageList;
	//this map associates all the images with an uuid
	ImageHolder::Map m_ImageMap;
	ImageHolder::Pointer  m_CurrentImage;
	void initOMP();

protected:
	boost::shared_ptr<Settings> m_Settings;

	ImageHolder::Pointer m_CurrentAnatomicalReference;
	Mode m_Mode;



};




}
} // end namespace



#endif