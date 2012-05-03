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
 * widgetensemblecomponent.cpp
 *
 * Description:
 *
 *  Created on: Mar 21, 2012
 *      Author: tuerke
 ******************************************************************/

#include "widgetensemblecomponent.hpp"
#include "widgetinterface.h"

namespace isis
{
namespace viewer
{

WidgetEnsembleComponent::WidgetEnsembleComponent ( QFrame *frame, QDockWidget *dockWidget, QWidget *placeHolder, widget::WidgetInterface *widgetImplementation )
	: m_frame( frame ),
	  m_dockWidget( dockWidget ),
	  m_placeHolder( placeHolder ),
	  m_widgetImplementation( widgetImplementation ),
	  m_needed( true )
{
	dockWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
}

bool WidgetEnsembleComponent::checkIfNeeded()
{
	bool needed = false;
	BOOST_FOREACH( ImageHolder::Vector::const_reference image, getWidgetInterface()->getWidgetEnsemble()->getImageVector() ) {
		const util::ivector4 mappedSize = mapCoordsToOrientation( image->getImageSize(), image->getImageProperties().latchedOrientation, getWidgetInterface()->getPlaneOrientation() );

		if( mappedSize[0] > 1 && mappedSize[1] > 1 ) {
			needed = true;
		}
	}
	m_needed = needed;
	m_dockWidget->setVisible( m_needed );
	return needed;
}

}
}