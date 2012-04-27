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
 * scalingWidget.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "scalingWidget.hpp"
#include "common.hpp"
#include "qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace ui
{

ScalingWidget::ScalingWidget( QWidget *parent, isis::viewer::QViewerCore *core )
	: QDialog( parent ),
	  m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	//TODO
	m_Interface.autoButton->setVisible( false );
	connect( m_Interface.min, SIGNAL( valueChanged( double ) ), this, SLOT( minChanged( double ) ) );
	connect( m_Interface.max, SIGNAL( valueChanged( double ) ), this, SLOT( maxChanged( double ) ) );
	connect( m_Interface.scaling, SIGNAL( valueChanged( double ) ), this, SLOT( scalingChanged( double ) ) );
	connect( m_Interface.offset, SIGNAL( valueChanged( double ) ), this, SLOT( offsetChanged( double ) ) );
	connect( m_Interface.resetButton, SIGNAL( clicked() ), this, SLOT( reset() ) );
	connect( m_Interface.autoButton, SIGNAL( clicked() ), this, SLOT( autoScale() ) );
}

void ScalingWidget::synchronize()
{
	m_Interface.offset->setMaximum( std::numeric_limits<double>::max() );
	m_Interface.offset->setMinimum( -std::numeric_limits<double>::max() );
	m_Interface.scaling->setMinimum( 0.0 );
	m_Interface.scaling->setMaximum( 100 );
	m_Interface.min->setMinimum( -std::numeric_limits<double>::max() );
	m_Interface.min->setMaximum( std::numeric_limits<double>::max() );
	m_Interface.max->setMinimum( -std::numeric_limits<double>::max() );
	m_Interface.max->setMaximum( std::numeric_limits<double>::max() );

	if( m_ViewerCore->hasImage() ) {
		const ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();

		disconnect( m_Interface.min, SIGNAL( valueChanged( double ) ), this, SLOT( minChanged( double ) ) );
		disconnect( m_Interface.max, SIGNAL( valueChanged( double ) ), this, SLOT( maxChanged( double ) ) );
		disconnect( m_Interface.scaling, SIGNAL( valueChanged( double ) ), this, SLOT( scalingChanged( double ) ) );
		disconnect( m_Interface.offset, SIGNAL( valueChanged( double ) ), this, SLOT( offsetChanged( double ) ) );
		m_Interface.offset->setValue( image->getImageProperties().offset );
		m_Interface.scaling->setValue( image->getImageProperties().scaling );
		std::pair<double, double> minMax = getMinMaxFromScalingOffset( std::make_pair<double, double>(
											   image->getImageProperties().scaling, image->getImageProperties().offset ), image );
		m_Interface.min->setValue( minMax.first );
		m_Interface.max->setValue( minMax.second );
		connect( m_Interface.min, SIGNAL( valueChanged( double ) ), this, SLOT( minChanged( double ) ) );
		connect( m_Interface.max, SIGNAL( valueChanged( double ) ), this, SLOT( maxChanged( double ) ) );
		connect( m_Interface.scaling, SIGNAL( valueChanged( double ) ), this, SLOT( scalingChanged( double ) ) );
		connect( m_Interface.offset, SIGNAL( valueChanged( double ) ), this, SLOT( offsetChanged( double ) ) );

		m_Interface.offset->setSingleStep( image->getImageProperties().extent / 100 );
	}
}

void ScalingWidget::maxChanged( double max )
{
	ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();
	image->getPropMap().setPropertyAs<double>( "scalingMaxValue", max );
	setScalingOffset( getScalingOffsetFromMinMax( std::make_pair<double, double>( image->getPropMap().getPropertyAs<double>( "scalingMinValue" ), max ), image ) );
}


void ScalingWidget::minChanged( double min )
{
	ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();
	image->getPropMap().setPropertyAs<double>( "scalingMinValue", min );
	setScalingOffset( getScalingOffsetFromMinMax( std::make_pair<double, double>( min, image->getPropMap().getPropertyAs<double>( "scalingMaxValue" ) ), image ) );
}

void ScalingWidget::offsetChanged( double offset )
{
	ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();
	applyScalingOffset( image->getImageProperties().scaling, offset, m_Interface.checkGlobal->isChecked() );
	setMinMax( getMinMaxFromScalingOffset( std::make_pair<double, double>( image->getImageProperties().scaling, offset ), image ), image );
}

void ScalingWidget::scalingChanged( double scaling )
{
	ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();
	applyScalingOffset( scaling, image->getImageProperties().offset , m_Interface.checkGlobal->isChecked() );
	setMinMax( getMinMaxFromScalingOffset( std::make_pair<double, double>( scaling, image->getImageProperties().offset ), image ), image );

}

void ScalingWidget::setMinMax( std::pair< double, double > minMax, ImageHolder::Pointer image )
{
	image->getPropMap().setPropertyAs<double>( "scalingMinValue", minMax.first );
	image->getPropMap().setPropertyAs<double>( "scalingMaxValue", minMax.second );
	m_ViewerCore->updateScene();
	synchronize();
}

void ScalingWidget::setScalingOffset( std::pair< double, double > scalingOffset )
{
	applyScalingOffset( scalingOffset.first, scalingOffset.second, m_Interface.checkGlobal->isChecked() );
	synchronize();
}


void ScalingWidget::autoScale()
{
	//TODO
	//  ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();
	//  std::pair<double, double> scalingOffset = image->scaling;
	//  setScalingOffset( scalingOffset );
	//  setMinMax( getMinMaxFromScalingOffset( scalingOffset, image ), image );
}

void ScalingWidget::reset()
{
	m_ViewerCore->getUICore()->getMainWindow()->resetScaling();
}

std::pair< double, double > ScalingWidget::getMinMaxFromScalingOffset( const std::pair< double, double >& scalingOffset, ImageHolder::Pointer image )
{
	std::pair<double, double> retMinMax;
	const double min = image->getImageProperties().minMax.first->as<double>();
// 	const double max = image->getImageProperties().minMax.second->as<double>();
	const double extent = image->getImageProperties().extent;
	const double offset = scalingOffset.second;
	const double scaling = scalingOffset.first;
	retMinMax.first = min + offset;
	retMinMax.second = (min + offset) + extent / scaling;
	return retMinMax;
}

std::pair< double, double > ScalingWidget::getScalingOffsetFromMinMax( const std::pair< double, double >& minMax, ImageHolder::Pointer image )
{
	std::pair<double, double> retScalingOffset;
	const double extent = image->getImageProperties().extent;
	const double _extent = minMax.second - minMax.first;
	const double min = image->getImageProperties().minMax.first->as<double>();
	retScalingOffset.first = extent / _extent;
	retScalingOffset.second = minMax.first - min ;
	return retScalingOffset;
}



void ScalingWidget::applyScalingOffset( const double &scaling, const double &offset, bool global )
{
	if( global ) {
		BOOST_FOREACH( ImageHolder::Vector::const_reference image, m_ViewerCore->getImageVector() ) {
			image->getImageProperties().scaling = scaling;
			image->getImageProperties().offset = offset;
			image->updateColorMap();
		}
		m_ViewerCore->updateScene();
	} else {
		if( m_ViewerCore->hasImage() ) {
			m_ViewerCore->getCurrentImage()->getImageProperties().scaling = scaling;
			m_ViewerCore->getCurrentImage()->getImageProperties().offset = offset;
			m_ViewerCore->getCurrentImage()->updateColorMap();
			m_ViewerCore->updateScene();
		}
	}
}


}
}
}