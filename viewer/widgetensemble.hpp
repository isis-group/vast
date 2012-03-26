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
 * widgetensemble.hpp
 *
 * Description:
 *
 *  Created on: Mar 06, 2012
 *      Author: tuerke
 ******************************************************************/

#ifndef VAST_WIDGET_ENSEMBLE_HPP
#define VAST_WIDGET_ENSEMBLE_HPP

#include <map>
#include <list>
#include <boost/signals2.hpp>
#include "widgetensemblecomponent.hpp"

namespace isis
{
namespace viewer
{

class ViewerCoreBase;

class WidgetEnsemble : public std::vector< WidgetEnsembleComponent::Pointer >
{
public:
	typedef boost::shared_ptr< WidgetEnsemble > Pointer;
	typedef std::vector< Pointer > Vector;

	WidgetEnsemble();

	QFrame *getFrame() { return m_frame; }
	const QFrame *getFrame() const { return m_frame; }

	QGridLayout *getLayout() { return m_layout; }
	const QGridLayout *getLayout() const { return m_layout; }

	void insertComponent( WidgetEnsembleComponent::Pointer component );

	void addImage( const ImageHolder::Pointer image );
	void removeImage( const ImageHolder::Pointer image );

	bool hasImage( const ImageHolder::Pointer image ) const;

	const ImageHolder::Vector &getImageList() const { return m_imageList; }
	ImageHolder::Vector &getImageList() { return m_imageList; }

	void setIsCurrent( bool current );
	bool isCurrent() const { return m_isCurrent; }

	void update( const ViewerCoreBase * );

	//signals
	boost::signals2::signal<void ( ImageHolder::Pointer ) > emitAddImage;
	boost::signals2::signal<void ( ImageHolder::Pointer ) > emitRemoveImage;
	boost::signals2::signal<void () > emitCheckIfNeeded;

private:
	QFrame *m_frame;
	QGridLayout *m_layout;
	unsigned short m_cols;
	void push_back( const value_type &value ) { std::vector< WidgetEnsembleComponent::Pointer >::push_back( value ); }
	ImageHolder::Vector m_imageList;
	bool m_isCurrent;
};



}
} //end namespace

#include "viewercorebase.hpp"

#endif //VAST_WIDGET_ENSEMBLE_HPP