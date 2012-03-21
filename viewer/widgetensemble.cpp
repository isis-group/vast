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
 * widgetensemble.cpp
 *
 * Description:
 *
 *  Created on: Mar 06, 2011
 *      Author: tuerke
 ******************************************************************/

#include "widgetensemble.hpp"

namespace isis {
namespace viewer {
	
WidgetEnsembleComponent::WidgetEnsembleComponent ( QFrame* frame, QDockWidget* dockWidget, QWidget* placeHolder, widget::WidgetInterface* widgetImplementation )
	: m_frame( frame ),
	m_dockWidget( dockWidget ),
	m_placeHolder( placeHolder ),
	m_widgetImplementation( widgetImplementation ),
	m_hasCurrentImage( false ),
	m_needed(true)
{}

bool WidgetEnsembleComponent::checkIfNeeded()
{
	bool needed = false;
	BOOST_FOREACH( ImageHolder::List::const_reference image, getWidgetInterface()->getImageList() ) {
		const util::ivector4 mappedSize = mapCoordsToOrientation( image->getImageSize(), image->getImageProperties().latchedOrientation, getWidgetInterface()->getPlaneOrientation() );
		if( mappedSize[0] > 1 && mappedSize[1] > 1 ) {
			needed = true;
		}
	}
	m_needed = needed;
	return needed;
}

WidgetEnsemble::WidgetEnsemble()
	: m_frame( new QFrame() ),
	m_layout( new QGridLayout() ),
	m_cols(0)
{
	m_frame->setLayout( m_layout );
	m_layout->setContentsMargins(0,0,0,0);
	m_frame->setContentsMargins(0,0,0,0);
}

void WidgetEnsemble::addImage ( const ImageHolder::Pointer image )
{
	if( find( m_imageList.begin(), m_imageList.end(), image ) == m_imageList.end()  ) {
		m_imageList.push_back( image );
		BOOST_FOREACH( std::vector<WidgetEnsembleComponent>::reference ensembleComponent, *this ) {
			ensembleComponent.getWidgetInterface()->addImage( image );
			ensembleComponent.checkIfNeeded();
		}
	} else {
		LOG( Dev, warning ) << "Trying to add image " << image->getFileNames().front() << ". But this image already exists in ensemble";
	}

}

void WidgetEnsemble::removeImage ( const ImageHolder::Pointer image )
{
	ImageHolder::List::iterator iter = find( m_imageList.begin(), m_imageList.end(), image );
	if( iter != m_imageList.end() ) {
		m_imageList.erase( iter );
		BOOST_FOREACH( std::vector<WidgetEnsembleComponent>::reference ensembleComponent, *this ) {
			ensembleComponent.getWidgetInterface()->removeImage( image );
			ensembleComponent.checkIfNeeded();
		}
	} else {
		LOG( Dev, warning )  << "Trying to remove image " << image->getFileNames().front() << " from ensemble. But this ensemble has no such image.";
	}

}


void WidgetEnsemble::insertComponent ( WidgetEnsembleComponent component )
{
	push_back( component );
	m_layout->addWidget( component.getDockWidget(), 0, m_cols++ );
}



}} // end namespace