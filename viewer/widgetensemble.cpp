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
 *  Created on: Mar 06, 2012
 *      Author: tuerke
 ******************************************************************/
#include "widgetensemble.hpp"
#include "widgetensemblecomponent.hpp"

namespace isis {
namespace viewer {

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
	if( std::find( m_imageList.begin(), m_imageList.end(), image ) == m_imageList.end()  ) {
		m_imageList.push_back( image );
		BOOST_FOREACH( std::vector<WidgetEnsembleComponent::Pointer>::reference ensembleComponent, *this ) {
			ensembleComponent->getWidgetInterface()->addImage( image );
			ensembleComponent->checkIfNeeded();
		}
	} else {
		LOG( Dev, warning ) << "Trying to add image " << image->getImageProperties().fileName << ". But this image already exists in ensemble";
	}

}

void WidgetEnsemble::removeImage ( const ImageHolder::Pointer image )
{
	ImageHolder::List::iterator iter = find( m_imageList.begin(), m_imageList.end(), image );
	if( iter != m_imageList.end() ) {
		m_imageList.erase( iter );
		BOOST_FOREACH( std::vector<WidgetEnsembleComponent::Pointer>::reference ensembleComponent, *this ) {
			ensembleComponent->getWidgetInterface()->removeImage( image );
			ensembleComponent->checkIfNeeded();
		}
	} else {
		LOG( Dev, warning )  << "Trying to remove image " << image->getImageProperties().fileName << " from ensemble. But this ensemble has no such image.";
	}

}

void WidgetEnsemble::insertComponent ( WidgetEnsembleComponent::Pointer component )
{
	push_back( component );
	component->getWidgetInterface()->setWidgetEnsemble( WidgetEnsemble::Pointer( this ) );
	m_layout->addWidget( component->getDockWidget(), 0, m_cols++ );
}

bool WidgetEnsemble::hasImage ( const ImageHolder::Pointer image ) const 
{
	return find( m_imageList.begin(), m_imageList.end(), image ) != m_imageList.end();
}


void WidgetEnsemble::setIsCurrent ( bool current )
{
	m_isCurrent = current;
	if( current ) {
		QPalette pal;
		pal.setColor( QPalette::Background, QColor( 119, 136, 153 ) );
		BOOST_FOREACH( std::vector<WidgetEnsembleComponent::Pointer>::reference ensembleComponent, *this ) {
			ensembleComponent->getFrame()->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
			ensembleComponent->getFrame()->setLineWidth( 1 );
			ensembleComponent->getFrame()->setPalette( pal );
			ensembleComponent->getFrame()->setAutoFillBackground( true );
			ensembleComponent->getWidgetInterface()->setCrossHairColor( QColor( 34, 139, 34 ) );
		}
	} else {
		BOOST_FOREACH( std::vector<WidgetEnsembleComponent::Pointer>::reference ensembleComponent, *this ) {
			ensembleComponent->getFrame()->setFrameStyle( 0 );
			ensembleComponent->getFrame()->setAutoFillBackground( false );
			ensembleComponent->getWidgetInterface()->setCrossHairColor( Qt::white );
		}
	}
}

void WidgetEnsemble::update( const ImageHolder::Pointer currentImage )
{
	//if this ensemble contains no image close it
	if( getImageList().empty() ) {
		getFrame()->close();
		return;
	}
	//if no image is visible make this ensemble invisible either
	bool visible = false;
	BOOST_FOREACH( ImageHolder::List::const_reference image, getImageList() ) {
		if( image->getImageProperties().isVisible ) {
			visible = true;
		}
	}
	getFrame()->setVisible(visible);
	//if this ensemble contains the current image make this the current ensemble either
	if( visible ) {
		setIsCurrent( std::find( getImageList().begin(), getImageList().end(), currentImage ) != getImageList().end() );
	}
}



}} // end namespace