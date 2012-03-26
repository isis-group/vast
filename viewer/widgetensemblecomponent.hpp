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
 * widgetensemblecomponent.hpp
 *
 * Description:
 *
 *  Created on: Mar 21, 2012
 *      Author: tuerke
 ******************************************************************/
#ifndef VAST_WIDGETENSEMBLECOMPONENT_HPP
#define VAST_WIDGETENSEMBLECOMPONENT_HPP

#include <QFrame>
#include <QDockWidget>
#include <boost/shared_ptr.hpp>
#include <map>


namespace isis
{
namespace viewer
{
namespace widget
{
class WidgetInterface;
}

class WidgetEnsembleComponent
{

public:
	typedef boost::shared_ptr< WidgetEnsembleComponent > Pointer;
	///map to associate a WidgetEnsembleComponent with a WidgetInterface raw pointer
	typedef std::map< widget::WidgetInterface *, Pointer > Map;

	WidgetEnsembleComponent( QFrame *frame, QDockWidget *dockWidget, QWidget *placeHolder, widget::WidgetInterface *widgetImplementation );

	// == operator to hold them in a std::map
	bool operator==( const WidgetEnsembleComponent &other ) const { return m_widgetImplementation == other.m_widgetImplementation; }

	QFrame *getFrame() { return m_frame; }
	const QFrame *getFrame() const { return m_frame; }

	QDockWidget *getDockWidget() { return m_dockWidget; }
	const QDockWidget *getDockWidget() const { return m_dockWidget; }

	QWidget *getPlaceHolder() { return m_placeHolder; }
	const QWidget *getPlaceHolder() const { return m_placeHolder; }

	widget::WidgetInterface *getWidgetInterface() { return m_widgetImplementation; }
	const widget::WidgetInterface *getWidgetInterface() const { return m_widgetImplementation; }

	bool isNeeded() const { return m_needed; }

	bool checkIfNeeded();

private:
	QFrame *m_frame;
	QDockWidget *m_dockWidget;
	QWidget *m_placeHolder;
	widget::WidgetInterface *m_widgetImplementation;
	bool m_needed;
};

}
} // end namespace

#include "widgetinterface.h"
#endif //VAST_WIDGETENSEMBLECOMPONENT_HPP