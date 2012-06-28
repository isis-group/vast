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
 * OptionWidget.hpp
 *
 * Description:
 *
 *  Created on: Mar 07, 2012
 ******************************************************************/

#ifndef VTK_OPTION_WIDGET_HPP
#define VTK_OPTION_WIDGET_HPP

#include "ui_option_widget.h"
#include "qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{

class VTKImageWidgetImplementation;

class OptionWidget : public QWidget
{
	Q_OBJECT
public:
	OptionWidget( QWidget *parent = 0 );
	void setWidget( VTKImageWidgetImplementation * );

public Q_SLOTS:
	void opacityGradientChanged( int );
	void croppingChanged();
	void mapperChanged( int );

private:
	Ui::vtkOptionWidget m_Interface;
	VTKImageWidgetImplementation *m_Widget;

};

}
}
} // end namespace

#include "VTKImageWidgetImplementation.hpp"

#endif //VTK_OPTION_WIDGET_HPP