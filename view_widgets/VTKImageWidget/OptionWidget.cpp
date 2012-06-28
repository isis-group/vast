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
 * OptionWidget.cpp
 *
 * Description:
 *
 *  Created on: Mar 07, 2012
 ******************************************************************/

#include "OptionWidget.hpp"
#include "VTKImageWidgetImplementation.hpp"


namespace isis
{
namespace viewer
{
namespace widget
{

OptionWidget::OptionWidget ( QWidget *parent )
	: QWidget ( parent )
{
	m_Interface.setupUi( this );
	m_Interface.mapperType->addItem( "Texture" );
	m_Interface.mapperType->addItem( "Fixed Ray Cast" );
	m_Interface.mapperType->addItem( "GPU Volume" );
}

void OptionWidget::setWidget ( VTKImageWidgetImplementation *widget )
{
	m_Widget = widget;
	connect( m_Interface.anterior, SIGNAL( pressed() ), m_Widget, SLOT( showAnterior() ) );
	connect( m_Interface.inferior, SIGNAL( pressed() ), m_Widget, SLOT( showInferior() ) );
	connect( m_Interface.posterior, SIGNAL( pressed() ), m_Widget, SLOT( showPosterior() ) );
	connect( m_Interface.superior, SIGNAL( pressed() ), m_Widget, SLOT( showSuperior() ) );
	connect( m_Interface.right, SIGNAL( pressed() ), m_Widget, SLOT( showRight() ) );
	connect( m_Interface.left, SIGNAL( pressed() ), m_Widget, SLOT( showLeft() ) );
	connect( m_Interface.opacityGradient, SIGNAL( valueChanged( int ) ), this, SLOT( opacityGradientChanged( int ) ) );
	connect( m_Interface.resetCamera, SIGNAL( pressed() ), m_Widget, SLOT( resetCamera() ) );
	connect( m_Interface.croppingX1, SIGNAL( valueChanged( int ) ), this, SLOT( croppingChanged() ) );
	connect( m_Interface.croppingX2, SIGNAL( valueChanged( int ) ), this, SLOT( croppingChanged() ) );
	connect( m_Interface.croppingY1, SIGNAL( valueChanged( int ) ), this, SLOT( croppingChanged() ) );
	connect( m_Interface.croppingY2, SIGNAL( valueChanged( int ) ), this, SLOT( croppingChanged() ) );
	connect( m_Interface.croppingZ1, SIGNAL( valueChanged( int ) ), this, SLOT( croppingChanged() ) );
	connect( m_Interface.croppingZ2, SIGNAL( valueChanged( int ) ), this, SLOT( croppingChanged() ) );
	connect( m_Interface.shade, SIGNAL( clicked( bool ) ), m_Widget, SLOT( setShade( bool ) ) );
	connect( m_Interface.mapperType, SIGNAL( activated( int ) ), this, SLOT( mapperChanged( int ) ) );
	m_Interface.mapperType->setCurrentIndex( m_Widget->getMapper() );
}

void OptionWidget::opacityGradientChanged ( int value )
{
	m_Widget->setOpacityGradientFactor( value / ( double )m_Interface.opacityGradient->maximum() );
	m_Widget->updateScene();
}

void OptionWidget::croppingChanged()
{
	double cropping[6];
	cropping[1] = m_Interface.croppingX1->value();
	cropping[0] = m_Interface.croppingX2->value();
	cropping[2] = m_Interface.croppingY1->value();
	cropping[3] = m_Interface.croppingY2->value();
	cropping[5] = m_Interface.croppingZ1->value();
	cropping[4] = m_Interface.croppingZ2->value();
	m_Widget->setCropping( cropping );
}

void OptionWidget::mapperChanged ( int mapper )
{
	m_Widget->setMapper( mapper );
}


}
}
} // end namespace