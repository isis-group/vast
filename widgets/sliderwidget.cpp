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
 * sliderwidget.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "sliderwidget.hpp"
#include <boost/concept_check.hpp>
#include "imageholder.hpp"
#include "qviewercore.hpp"


namespace isis
{
namespace viewer
{
namespace widget
{

SliderWidget::SliderWidget( QWidget *parent, isis::viewer::QViewerCore *core )
	: QWidget( parent ),
	  m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	m_Interface.opacitySlider->setToolTip( "Opacity" );
	m_Interface.maxSlider->setToolTip( "Upper threshold" );
	m_Interface.minSlider->setToolTip( "Lower threshold" );
	m_Interface.opacitySlider->setMinimum( 0 );
	m_Interface.opacitySlider->setMaximum( 1000 );
	m_Interface.opacitySlider->setSliderPosition( 1000 );
	m_Interface.minSlider->setMinimum( 0 );
	m_Interface.minSlider->setMaximum( 1000 );
	m_Interface.minSlider->setSliderPosition( 1000 );
	m_Interface.maxSlider->setMinimum( 0 );
	m_Interface.maxSlider->setMaximum( 1000 );
	m_Interface.maxSlider->setSliderPosition( 1000 );

	connect( m_Interface.opacitySlider, SIGNAL( sliderMoved( int ) ), this, SLOT( opacityChanged( int ) ) );
	connect( m_Interface.maxSlider, SIGNAL( sliderMoved( int ) ), this, SLOT( upperThresholdChanged( int ) ) );
	connect( m_Interface.minSlider, SIGNAL( sliderMoved( int ) ), this, SLOT( lowerThresholdChanged( int ) ) );
	connect( m_Interface.checkGlobal, SIGNAL( toggled(bool)), this, SLOT( toggleGlobal(bool)));

}

void SliderWidget::toggleGlobal(bool global)
{
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>("zmapGlobal", global);
}

void SliderWidget::setSliderVisible( SliderWidget::SliderType slider , bool visible )
{
	switch( slider ) {
	case Opacity:
		m_Interface.opacitySlider->setVisible( visible );
		break;
	case UpperThreshold:
		m_Interface.maxSlider->setVisible( visible );
		break;
	case LowerThreshold:
		m_Interface.minSlider->setVisible( visible );
		break;
	}
}

double SliderWidget::norm( const double &min, const double &max, const int &pos )
{
	const double range = fabs( min ) + fabs( max );
	return ( range / 1000.0 ) * pos;

}

void SliderWidget::opacityChanged( int sliderPos )
{
	if( !m_Interface.checkGlobal->isChecked() ) {
		m_ViewerCore->getCurrentImage()->opacity = norm( 0.0, 1.0, sliderPos )  ;
	} else {
		BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) {
			if( m_ViewerCore->getMode() == ViewerCoreBase::zmap ) {
				if( image.second->imageType == ImageHolder::z_map ) {
					image.second->opacity = norm( 0.0, 1.0, sliderPos )  ;
				}
			} else {
				image.second->opacity = norm( 0.0, 1.0, sliderPos )  ;
			}
		}
	}

	m_ViewerCore->updateScene();
}

void SliderWidget::lowerThresholdChanged( int sliderPos )
{
	if ( !m_Interface.checkGlobal->isChecked() ) {
		m_ViewerCore->getCurrentImage()->lowerThreshold = norm( m_ViewerCore->getCurrentImage()->minMax.first->as<double>() , 0.0, 1000 - sliderPos ) * -1 ;
		m_ViewerCore->getCurrentImage()->updateColorMap();
	} else {
		BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) {
			if( image.second->imageType == ImageHolder::z_map ) {
				const double lT = norm( m_maxMin, 0.0, 1000 - sliderPos ) * -1;
				image.second->lowerThreshold = lT > image.second->minMax.first->as<double>() ? lT : image.second->minMax.first->as<double>();
				image.second->updateColorMap();
			}
		}
	}

	m_ViewerCore->updateScene();
}

void SliderWidget::upperThresholdChanged( int sliderPos )
{
	if( !m_Interface.checkGlobal->isChecked() ) {
		m_ViewerCore->getCurrentImage()->upperThreshold = norm( 0.0, m_ViewerCore->getCurrentImage()->minMax.second->as<double>(), 1000 - sliderPos ) ;
		m_ViewerCore->getCurrentImage()->updateColorMap();
	} else {
		BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) {
			if( image.second->imageType == ImageHolder::z_map ) {
				const double uT = norm( 0.0, m_maxMax, 1000 - sliderPos );
				image.second->upperThreshold = uT < image.second->minMax.second->as<double>() ? uT : image.second->minMax.second->as<double>() - std::numeric_limits<double>::round_error();
				image.second->updateColorMap();
			}
		}
	}

	m_ViewerCore->updateScene();
}

void SliderWidget::synchronize()
{
	if( m_ViewerCore->getMode() == ViewerCoreBase::zmap ) {
		m_Interface.zmapModeFrame->setVisible( true );
		BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
			if( image.second->imageType == ImageHolder::z_map ) {
				if( image.second->minMax.first->as<double>() < m_maxMin ) {
					m_maxMin = image.second->minMax.first->as<double>();
				}

				if( image.second->minMax.second->as<double>() > m_maxMax ) {
					m_maxMax = image.second->minMax.second->as<double>();
				}
			}
		}
	} else {
		m_Interface.zmapModeFrame->setVisible( false );
	}

	if( m_ViewerCore->hasImage() ) {
		QWidget::setVisible( true );
		m_Interface.opacitySlider->setSliderPosition( m_ViewerCore->getCurrentImage()->opacity * 1000 );

		if( m_ViewerCore->getCurrentImage()->imageType == ImageHolder::z_map ) {
			setSliderVisible( LowerThreshold, m_ViewerCore->getCurrentImage()->minMax.first->as<double>() < 0 );
			setSliderVisible( UpperThreshold, m_ViewerCore->getCurrentImage()->minMax.second->as<double>() > 0 );
			const unsigned short lowerThreshold = 1000 - abs( ( 1000 / m_ViewerCore->getCurrentImage()->minMax.first->as<double>() )
				* m_ViewerCore->getCurrentImage()->lowerThreshold );
			const unsigned short upperThreshold = 1000 - abs( ( 1000 / m_ViewerCore->getCurrentImage()->minMax.second->as<double>() )
			  * m_ViewerCore->getCurrentImage()->upperThreshold );
			m_Interface.minSlider->setSliderPosition( lowerThreshold );
			m_Interface.maxSlider->setSliderPosition( upperThreshold );			
			setSliderVisible( Opacity, true );
		} else if ( m_ViewerCore->getCurrentImage()->imageType == ImageHolder::anatomical_image ) {
			setSliderVisible( LowerThreshold, false );
			setSliderVisible( UpperThreshold, false );
			setSliderVisible( Opacity, true );
		}
	} else {
		QWidget::setVisible( false );
	}
	m_Interface.checkGlobal->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>("zmapGlobal"));

}



}
}
}
