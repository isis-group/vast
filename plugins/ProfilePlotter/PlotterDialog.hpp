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
 * PlotterDialog.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef PLOTTERDIALOG_HPP
#define PLOTTERDIALOG_HPP

#include <QtGui>
#include <QWidget>
#include "Plot.hpp"
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_scaleitem.h>
#include <qwt_symbol.h>
#include "ui_plotting.h"
#include <iostream>
#include "qviewercore.hpp"
#include "DataStorage/valuearray.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{

class PlotterDialog : public QDialog
{
	Q_OBJECT
public:
	PlotterDialog( QWidget *parent, QViewerCore *core );
public Q_SLOTS:
	void showEvent( QShowEvent * );
	void updateScene();
	void setYScaleMax( double );
	void setYScaleMin( double );

	virtual void refresh( util::fvector3 physicalCoords );

private:
	Ui::plottingDialog ui;
	Plot *plot;
	QwtPlotGrid *grid;
	QwtPlotMarker *plotMarker;
	QViewerCore *m_ViewerCore;
	util::fvector3 m_CurrentPhysicalCoords;

	void fillProfile( boost::shared_ptr<ImageHolder> image, const util::ivector4 &voxCoords, QwtPlotCurve *curve, const unsigned short &axis );
	void fillSpectrum(  boost::shared_ptr<ImageHolder> image, const util::ivector4 &voxCoords, QwtPlotCurve *curve, const unsigned short &axis );

	template<typename TYPE>
	void fillVector( QVector<double> &iv, const util::ivector4 &vox, const boost::shared_ptr<ImageHolder> image, const unsigned short &axis ) {
		util::ivector4 _coords = vox;
		isis::data::TypedImage<TYPE> tImage( *image->getISISImage() );

		for ( size_t i = 0; i < image->getImageSize()[axis]; i++ ) {
			_coords[axis] = i;
			iv.push_back( static_cast<isis::data::Image &> ( tImage ).voxel<TYPE>( _coords[0], _coords[1], _coords[2], _coords[3] ) );
		}
	}

	template<typename TYPE>
	void fillVectorForFFTW( double *n, const unsigned short &axis ) {
	}

};

}
}
}


#endif