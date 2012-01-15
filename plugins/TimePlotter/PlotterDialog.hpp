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
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include "ui_plotting.h"
#include <iostream>
#include "qviewercore.hpp"
#include "DataStorage/typeptr.hpp"

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
	PlotterDialog( QWidget *parent, QViewerCore *core ) : QDialog( parent ), m_ViewerCore( core ) {
		ui.setupUi( this );
		plot = new QwtPlot( tr( "Timecourse" ), ui.widget );
		grid = new QwtPlotGrid();
		plotMarker = new QwtPlotMarker();
		plotMarker->setSymbol( QwtSymbol( QwtSymbol::Cross, QBrush(), QPen(), QSize( 10, 10 ) ) );
		plotMarker->attach( plot );
		ui.verticalLayout->addWidget( plot );
		plot->setAxisTitle( 2, tr( "Timestep" ) );
		plot->setAxisTitle( 0, tr( "Intensity" ) );
		plot->setBackgroundRole( QPalette::Light );
		plot->setFont( QFont( "", 2 ) );
		connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, ( SLOT( refresh( util::fvector4 ) ) ) );
		connect( m_ViewerCore, SIGNAL( emitUpdateScene() ), this, SLOT( updateScene() ) );

		if( m_ViewerCore->hasImage() ) {
			refresh( m_ViewerCore->getCurrentImage()->physicalCoords );
		}

		setMinimumHeight( 200 );
		setMaximumHeight( 500 );

	};
public Q_SLOTS:
    void showEvent( QShowEvent * ) { updateScene(); }
	void updateScene() { refresh ( m_CurrentPhysicalCoords ); }

	virtual void refresh( util::fvector4 physicalCoords ) {
		m_CurrentPhysicalCoords = physicalCoords;
		plot->clear();

		if( !ui.checkLock->isChecked() && isVisible()) {
			BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
				if( image.second->getImageSize()[3] > 1 ) {
					QwtPlotCurve *curve = new QwtPlotCurve();
					curve->detach();
					std::stringstream title;
					std::stringstream coordsAsString;
					const util::ivector4 voxCoords = image.second->getISISImage()->getIndexFromPhysicalCoords( physicalCoords, true );

					title << "Timecourse for " << m_ViewerCore->getCurrentImage()->getFileNames().front();
					coordsAsString << voxCoords[0] << " : " << voxCoords[1] << " : " << voxCoords[2];
					plot->setTitle( coordsAsString.str().c_str() );
					setWindowTitle( title.str().c_str() );
					uint16_t repTime = 1;

					if ( image.second->getISISImage()->hasProperty( "repetitionTime" ) ) {
						repTime = ( float )image.second->getISISImage()->getPropertyAs<uint16_t>( "repetitionTime" ) / 1000;
						plot->setAxisTitle( 2, tr( "t / s" ) );
					} else {
						plot->setAxisTitle( 2, tr( "Repetition (missing TR)" ) );
					}

					QVector<double> timeSteps;
					QVector<double> intensityValues;
					using namespace isis::data;

					for ( size_t t = 0; t < image.second->getImageSize()[3]; t++ ) {
						timeSteps.push_back( t * repTime );

						switch( image.second->getISISImage()->getChunk( voxCoords[0], voxCoords[1], voxCoords[2], t ).getTypeID() ) {
						case ValuePtr<bool>::staticID:
							fillVector<bool>( intensityValues, t, voxCoords, image.second );
							break;
						case ValuePtr<int8_t>::staticID:
							fillVector<int8_t>( intensityValues, t, voxCoords, image.second );
							break;
						case ValuePtr<uint8_t>::staticID:
							fillVector<uint8_t>( intensityValues, t, voxCoords, image.second );
							break;
						case ValuePtr<int16_t>::staticID:
							fillVector<int16_t>( intensityValues, t, voxCoords, image.second  );
							break;
						case ValuePtr<uint16_t>::staticID:
							fillVector<uint16_t>( intensityValues, t, voxCoords, image.second );
							break;
						case ValuePtr<int32_t>::staticID:
							fillVector<int32_t>( intensityValues, t, voxCoords, image.second );
							break;
						case ValuePtr<uint32_t>::staticID:
							fillVector<uint32_t>( intensityValues, t, voxCoords, image.second );
							break;
						case ValuePtr<int64_t>::staticID:
							fillVector<int64_t>( intensityValues, t, voxCoords, image.second );
							break;
						case ValuePtr<uint64_t>::staticID:
							fillVector<uint64_t>( intensityValues, t, voxCoords, image.second );
							break;
						case ValuePtr<float>::staticID:
							fillVector<float>( intensityValues, t, voxCoords, image.second );
							break;
						case ValuePtr<double>::staticID:
							fillVector<double>( intensityValues, t, voxCoords, image.second );
							break;
						}
					}

					curve->setData( timeSteps, intensityValues );

					if( image.second.get() == m_ViewerCore->getCurrentImage().get() || m_ViewerCore->getMode() == ViewerCoreBase::zmap ) {
						curve->attach( plot );
						curve->setPen( QPen( Qt::red ) );
					} else {
						if( image.second->isVisible ) {
							curve->attach( plot );
						}

						QPen pen;
						pen.setBrush( QBrush( Qt::gray, Qt::Dense1Pattern ) );
						curve->setPen( pen );
					}


				} else {
					plot->setTitle( QString( "Image has only one timestep!" ) );
				}
			}
			plot->replot();
		}
	}
private:
	Ui::plottingDialog ui;
	QwtPlot *plot;
	QwtPlotGrid *grid;
	QwtPlotMarker *plotMarker;
	QViewerCore *m_ViewerCore;
	util::fvector4 m_CurrentPhysicalCoords;
	template<typename TYPE>
	void fillVector( QVector<double> &iv, const size_t &t, const util::ivector4 &vox, boost::shared_ptr<ImageHolder> image ) {
		iv.push_back( image->getISISImage()->voxel<TYPE>( vox[0], vox[1], vox[2], t ) );
	}

};

}
}
}


#endif