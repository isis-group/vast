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
#include "imageholder.hpp"
#include "color.hpp"

namespace isis
{
namespace viewer
{

WidgetEnsemble::WidgetEnsemble()
	: m_hasOptionWidget( false ),
	  m_frame( new QFrame() ),
	  m_layout( new QHBoxLayout() ),
	  m_cols( 0 )
{
	m_frame->setLayout( m_layout );
	m_layout->setContentsMargins( 0, 0, 0, 0 );
	m_frame->setContentsMargins( 0, 0, 0, 0 );
}

void WidgetEnsemble::addImage ( const ImageHolder::Pointer image )
{
	//if image is not in list
	if( std::find( m_imageVector.begin(), m_imageVector.end(), image ) == m_imageVector.end()  ) {
		if( image->getImageProperties().imageType == ImageHolder::structural_image ) {
			m_imageVector.insert( m_imageVector.begin(), image );
		} else {
			m_imageVector.push_back( image );
		}

		emitAddImage( image );
		emitCheckIfNeeded();
	} else {
		LOG( Dev, warning ) << "Trying to add image " << image->getImageProperties().fileName << ". But this image already exists in ensemble";
	}

}

void WidgetEnsemble::close()
{
	disconnectFromViewer();
	BOOST_FOREACH( reference component, *this ) {
		component->getPlaceHolder()->close();
		component->getFrame()->close();
		component->getDockWidget()->close();
	}
	m_imageVector.clear();

	if( hasOptionWidget() ) {
		m_optionWidget->close();
	}

	m_frame->close();

}


void WidgetEnsemble::removeImage ( const ImageHolder::Pointer image )
{
	const ImageHolder::Vector::iterator iter = std::find( m_imageVector.begin(), m_imageVector.end(), image );

	if( iter != m_imageVector.end() ) {
		m_imageVector.erase( iter );
		emitRemoveImage( image );
		emitCheckIfNeeded();
	} else {
		LOG( Dev, warning )  << "Trying to remove image " << image->getImageProperties().fileName << " from ensemble. But this ensemble has no such image.";
	}

}

void WidgetEnsemble::insertComponent ( WidgetEnsembleComponent::Pointer component )
{
	push_back( component );
	emitAddImage.connect( boost::bind( &widget::WidgetInterface::addImage, component->getWidgetInterface(), _1 ) );
	emitRemoveImage.connect( boost::bind( &widget::WidgetInterface::removeImage, component->getWidgetInterface(), _1 ) );
	emitCheckIfNeeded.connect( boost::bind( &WidgetEnsembleComponent::checkIfNeeded, component.get() ) );
	m_layout->addWidget( component->getDockWidget() );
}

bool WidgetEnsemble::hasImage ( const ImageHolder::Pointer image ) const
{
	return find( m_imageVector.begin(), m_imageVector.end(), image ) != m_imageVector.end();
}


void WidgetEnsemble::setIsCurrent ( bool current )
{
	m_isCurrent = current;

	if( current ) {
		QPalette pal;
		pal.setColor( QPalette::Background, color::currentEnsemble );
		BOOST_FOREACH( std::vector<WidgetEnsembleComponent::Pointer>::reference ensembleComponent, *this ) {
			ensembleComponent->getFrame()->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
			ensembleComponent->getFrame()->setLineWidth( 1 );
			ensembleComponent->getFrame()->setPalette( pal );
			ensembleComponent->getFrame()->setAutoFillBackground( true );
			ensembleComponent->getWidgetInterface()->setCrossHairColor( color::currentImage );
		}
	} else {
		BOOST_FOREACH( std::vector<WidgetEnsembleComponent::Pointer>::reference ensembleComponent, *this ) {
			ensembleComponent->getFrame()->setFrameStyle( 0 );
			ensembleComponent->getFrame()->setAutoFillBackground( false );
			ensembleComponent->getWidgetInterface()->setCrossHairColor( Qt::white );
		}
	}
}
void WidgetEnsemble::setOptionWidget ( QWidget *optionWidget )
{
	m_optionWidget = optionWidget;
	m_hasOptionWidget = optionWidget;
	m_layout->addWidget( m_optionWidget, 0, Qt::AlignLeft );

}

void WidgetEnsemble::connectToViewer()
{
	BOOST_FOREACH( reference component, *this ) {
		component->getWidgetInterface()->connectSignals();
	}
}

void WidgetEnsemble::disconnectFromViewer()
{
	BOOST_FOREACH( reference component, *this ) {
		component->getWidgetInterface()->disconnectSignals();
	}
}


void WidgetEnsemble::update( const ViewerCoreBase *core )
{
	if( core->hasImage() ) {
		ImageHolder::Pointer currentImage = core->getCurrentImage();
		const ImageHolder::Vector::const_iterator currentImageIterator = std::find( m_imageVector.begin(), m_imageVector.end(), currentImage );
		//if no image is visible make this ensemble invisible either
		bool visible = false;
		BOOST_FOREACH( ImageHolder::Vector::const_reference image, m_imageVector ) {
			if( image->getImageProperties().isVisible ) {
				visible = true;
			}
		}
		getFrame()->setVisible( visible );

		if( hasOptionWidget() ) {
			getOptionWidget()->setVisible( visible );
		}

		//if this ensemble contains the current image make this the current ensemble either
		if( visible ) {
			setIsCurrent( currentImageIterator != m_imageVector.end() );
			//we do not want to see the option widget if the ensemble is not current
		}

		//resort the ensembles image list -> current image has to be the top image
		ImageHolder::Vector tmpList;

		for( ImageHolder::Vector::const_iterator iter = m_imageVector.begin(); iter != m_imageVector.end(); iter++ ) {
			if( iter != currentImageIterator ) {
				tmpList.push_back( *iter );
			}
		}

		if( currentImageIterator != m_imageVector.end() ) {
			tmpList.push_back( currentImage );
		}

		m_imageVector = tmpList;
	}
}

const ImageHolder::Pointer WidgetEnsemble::getFirstImageOfType ( const ImageHolder::ImageType &imageType ) const
{
	BOOST_FOREACH( const ImageHolder::Vector::const_reference image, getImageVector() ) {
		if( image->getImageProperties().imageType == imageType ) {
			return image;
		}
	}
	LOG( Dev, warning ) << "getFirstImageOfType: Could not find any image of type " << imageType;
	return ImageHolder::Pointer();
}


}
} // end namespace