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
 * scalingWidget.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef SCALINGWIDGET_HPP
#define SCALINGWIDGET_HPP

#include <QDialog>
#include "ui_scalingDialog.h"
#include "../viewer/qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace ui
{


class ScalingWidget : public QDialog
{
	Q_OBJECT
public:
	ScalingWidget( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	void synchronize();
	void minChanged( double );
	void maxChanged( double );
	void scalingChanged( double );
	void offsetChanged( double );
	void reset();
	void applyScalingOffset( const double &scaling, const double &offset, bool global );
	void on_scaleSlider_valueChanged( int );
	void on_offsetSlider_valueChanged( int );


private:
	Ui::scalingOptionDialog m_Interface;
	QViewerCore *m_ViewerCore;

	void setMinMax( std::pair<double, double> minMax, boost::shared_ptr<ImageHolder> image ) ;
	void setScalingOffset( std::pair<double, double> scalingOffset ) ;
};

}
}
}

#endif
