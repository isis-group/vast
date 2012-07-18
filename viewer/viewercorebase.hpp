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
#include <boost/signals2.hpp>

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

	virtual ImageHolder::Vector addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );
	virtual ImageHolder::Pointer addImage( const data::Image &image, const ImageHolder::ImageType &imageType );

	bool removeImage( const ImageHolder::Pointer image );

	const boost::shared_ptr<Settings> getSettings() const { return m_Settings; }
	boost::shared_ptr<Settings> getSettings() { return m_Settings; }

	void setCurrentImage( const ImageHolder::Pointer image ) { m_CurrentImage = image; emitCurrentImageChanged( image ); }

	ImageHolder::Pointer getCurrentImage() const;

	ImageHolder::Pointer getCurrentAnatomicalRefernce() const { return m_CurrentAnatomicalReference; }

	bool hasImage() const { return m_imageVector.size() && m_CurrentImage.get(); }

	widget::WidgetInterface *getWidget( const std::string &identifier ) throw( std::runtime_error & );
	const util::PropertyMap *getWidgetProperties( const std::string &identifier ) ;

	bool hasWidget( const std::string &identifier );

	virtual void setMode( const Mode &mode ) { m_Mode = mode; }
	virtual Mode getMode() const { return m_Mode; }

	ImageHolder::Vector &getImageVector() { return m_imageVector; }
	const ImageHolder::Vector &getImageVector() const { return m_imageVector; }

	ImageHolder::Map &getImageMap() { return m_ImageMap; }
	const ImageHolder::Map &getImageMap() const { return m_ImageMap; }

	//signals
	boost::signals2::signal<void ( const ImageHolder::Pointer )> emitAddImage;
	boost::signals2::signal<void ( const ImageHolder::Pointer )> emitCurrentImageChanged;
	boost::signals2::signal<void ( const ImageHolder::Pointer )> emitImageContentChanged;
	boost::signals2::signal<void ( const util::fvector3 & )> emitGlobalPhysicalCoordsChanged;
	boost::signals2::signal<void ( const util::ivector4 & )> emitGlobalVoxelCoordsChanged;
	boost::signals2::signal<void ( const size_t & )> emitGlobalTimestepChanged;
	boost::signals2::signal<void ()> emitRefreshAllWidgets;

private:
	//this is the container which holds all the images
	ImageHolder::Vector m_imageVector;
	//this map associates all the images with their filenames
	ImageHolder::Map m_ImageMap;
	ImageHolder::Pointer  m_CurrentImage;

protected:
	boost::shared_ptr<Settings> m_Settings;

	ImageHolder::Pointer m_CurrentAnatomicalReference;
	Mode m_Mode;



};




}
} // end namespace



#endif