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
 * HistogramDialog.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef HISTOGRAMDIALOG_HPP
#define HISTOGRAMDIALOG_HPP

#include "ui_histogramDialog.h"
#include "qviewercore.hpp"
#include "qwt_plot_curve.h"
#include "qwt_plot.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_zoomer.h"
#include "qwt_plot_picker.h"

namespace isis
{
namespace viewer
{
namespace plugin
{


class HistogramDialog : public QDialog
{
	Q_OBJECT
public:
	HistogramDialog( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	void paintHistogram();
	void showEvent( QShowEvent * );
private:
	Ui::histogramDialog m_Interface;
	QViewerCore *m_ViewerCore;
	QwtPlot *m_Plotter;
	QwtPlotGrid *m_Grid;
	QwtPlotZoomer *m_Zoomer;
	unsigned short m_length;



};


}
}
}



#endif