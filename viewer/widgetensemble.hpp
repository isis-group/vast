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
 *  Created on: Mar 06, 2011
 *      Author: tuerke
 ******************************************************************/

#ifndef VAST_WIDGET_ENSEMBLE_HPP
#define VAST_WIDGET_ENSEMBLE_HPP

#include <QFrame>
#include <QDockWidget>
#include <map>
#include <list>
#include "widgetinterface.h"


namespace isis
{
namespace viewer
{

class WidgetEnsembleComponent
{

public:
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

	bool hasCurrentImage() const { return m_hasCurrentImage; }

	void setHasCurrentImage( bool hasCurrentImage ) { m_hasCurrentImage = hasCurrentImage; }


private:
	QFrame *m_frame;
	QDockWidget *m_dockWidget;
	QWidget *m_placeHolder;
	widget::WidgetInterface *m_widgetImplementation;
	bool m_hasCurrentImage;
};

class WidgetEnsemble : public std::vector< WidgetEnsembleComponent >
{
public:
	WidgetEnsemble();

	QFrame *getFrame() { return m_frame; }
	const QFrame *getFrame() const { return m_frame; }

	QGridLayout *getLayout() { return m_layout; }
	const QGridLayout *getLayout() const { return m_layout; }

	void insertComponent( WidgetEnsembleComponent component );

private:
	QFrame *m_frame;
	QGridLayout *m_layout;
	unsigned short m_cols;
	void push_back( const value_type &value ) { std::vector< WidgetEnsembleComponent >::push_back( value ); }
};

typedef std::list< WidgetEnsemble > WidgetEnsembleListType;
typedef std::map< widget::WidgetInterface *, WidgetEnsembleComponent > WidgetMap;


}
} //end namespace


#endif //VAST_WIDGET_ENSEMBLE_HPP