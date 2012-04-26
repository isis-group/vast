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

}

void OptionWidget::setWidget ( VTKImageWidgetImplementation* widget)
{
	m_Widget = widget;
	connect( m_Interface.anterior, SIGNAL( pressed()), m_Widget, SLOT( showAnterior() ) );
	connect( m_Interface.inferior, SIGNAL( pressed()), m_Widget, SLOT( showInferior() ) );
	connect( m_Interface.posterior, SIGNAL( pressed()), m_Widget, SLOT( showPosterior() ) );
	connect( m_Interface.superior, SIGNAL( pressed()), m_Widget, SLOT( showSuperior() ) );
	connect( m_Interface.right, SIGNAL( pressed()), m_Widget, SLOT( showRight() ) );
	connect( m_Interface.left, SIGNAL( pressed()), m_Widget, SLOT( showLeft() ) );
	connect( m_Interface.opacityGradient, SIGNAL( valueChanged(int)), this, SLOT( opacityGradientChanged(int)));
}

void OptionWidget::opacityGradientChanged ( int value )
{
	m_Widget->setOpacityGradientFactor( value / (double)m_Interface.opacityGradient->maximum());
	m_Widget->updateScene();
}


}
}
} // end namespace