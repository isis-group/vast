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
#include "../viewer/common.hpp"
#include "../viewer/qviewercore.hpp"
#include "../viewer/nativeimageops.hpp"

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

	m_Interface.offset->setMaximum( std::numeric_limits<double>::max() );
	m_Interface.offset->setMinimum( -std::numeric_limits<double>::max() );
	m_Interface.scaling->setMinimum( 0.0 );
	m_Interface.scaling->setMaximum( 99999 );
	m_Interface.min->setMinimum( -std::numeric_limits<double>::max() );
	m_Interface.min->setMaximum( std::numeric_limits<double>::max() );
	m_Interface.max->setMinimum( -std::numeric_limits<double>::max() );
	m_Interface.max->setMaximum( std::numeric_limits<double>::max() );
	
}

void ScalingWidget::synchronize()
{

	if( m_ViewerCore->hasImage() ) {
		const ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();

		disconnect( m_Interface.min, SIGNAL( valueChanged( double ) ), this, SLOT( minChanged( double ) ) );
		disconnect( m_Interface.max, SIGNAL( valueChanged( double ) ), this, SLOT( maxChanged( double ) ) );
		disconnect( m_Interface.scaling, SIGNAL( valueChanged( double ) ), this, SLOT( scalingChanged( double ) ) );
		disconnect( m_Interface.offset, SIGNAL( valueChanged( double ) ), this, SLOT( offsetChanged( double ) ) );
		m_Interface.offset->setValue( image->getImageProperties().offset );
		m_Interface.scaling->setValue( image->getImageProperties().scaling );
		m_Interface.min->setValue( image->getImageProperties().scalingMinMax.first );
		m_Interface.max->setValue( image->getImageProperties().scalingMinMax.second );
		connect( m_Interface.min, SIGNAL( valueChanged( double ) ), this, SLOT( minChanged( double ) ) );
		connect( m_Interface.max, SIGNAL( valueChanged( double ) ), this, SLOT( maxChanged( double ) ) );
		connect( m_Interface.scaling, SIGNAL( valueChanged( double ) ), this, SLOT( scalingChanged( double ) ) );
		connect( m_Interface.offset, SIGNAL( valueChanged( double ) ), this, SLOT( offsetChanged( double ) ) );

		m_Interface.offset->setSingleStep( image->getImageProperties().extent / 100 );


		m_Interface.offsetSlider->setValue(image->getImageProperties().offset);
		m_Interface.offsetSlider->setMinimum(-image->getImageProperties().minMax.second->as<int>());
		m_Interface.offsetSlider->setMaximum(image->getImageProperties().minMax.second->as<int>());
		m_Interface.offsetDisp->display(image->getImageProperties().offset);
		

		// positive
		m_Interface.scaleSlider->setValue(100./image->getImageProperties().scaling);
		m_Interface.posVal->display    (image->getImageProperties().scalingMinMax.second);

	}
}

void ScalingWidget::maxChanged( double max )
{
	ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();
	image->getImageProperties().scalingMinMax.second =  max;
	setScalingOffset( operation::NativeImageOps::getScalingOffsetFromMinMax( std::make_pair<double, double>( image->getImageProperties().scalingMinMax.first, max ), image ) );
}

void ScalingWidget::minChanged( double min )
{
	ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();
	image->getImageProperties().scalingMinMax.first = min;
	setScalingOffset( operation::NativeImageOps::getScalingOffsetFromMinMax( std::make_pair<double, double>( min, image->getImageProperties().scalingMinMax.second ), image ) );
}

void ScalingWidget::offsetChanged( double offset )
{
	ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();
	setMinMax( operation::NativeImageOps::getMinMaxFromScalingOffset( std::make_pair<double, double>( image->getImageProperties().scaling, offset ), image ), image );
	applyScalingOffset( image->getImageProperties().scaling, offset, m_Interface.checkGlobal->isChecked() );
}

void ScalingWidget::scalingChanged( double scaling )
{
	ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();
	setMinMax(  operation::NativeImageOps::getMinMaxFromScalingOffset( std::make_pair<double, double>( scaling, image->getImageProperties().offset ), image ), image );
	applyScalingOffset( scaling, image->getImageProperties().offset , m_Interface.checkGlobal->isChecked() );

}

void ScalingWidget::setMinMax( std::pair< double, double > minMax, ImageHolder::Pointer image )
{
	image->getImageProperties().scalingMinMax.first = minMax.first;
	image->getImageProperties().scalingMinMax.second = minMax.second;
}

void ScalingWidget::setScalingOffset( std::pair< double, double > scalingOffset )
{
	applyScalingOffset( scalingOffset.first, scalingOffset.second, m_Interface.checkGlobal->isChecked() );
}


void ScalingWidget::reset()
{
	m_ViewerCore->getUICore()->getMainWindow()->resetScaling();
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

	m_ViewerCore->getUICore()->refreshUI();
}

void ScalingWidget::on_scaleSlider_valueChanged(int value)
{
	scalingChanged(100./value);
}
void ScalingWidget::on_offsetSlider_valueChanged(int offset)
{
	ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();
	std::pair< double, double > minmax=operation::NativeImageOps::getMinMaxFromScalingOffset( std::make_pair<double, double>( image->getImageProperties().scaling, offset ), image );
	applyScalingOffset( image->getImageProperties().scaling, offset, m_Interface.checkGlobal->isChecked() );
}


}
}
}
