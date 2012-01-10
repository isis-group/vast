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
 * HistogramDialog.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "HistogramDialog.hpp"
#include <DataStorage/typeptr.hpp>



isis::viewer::plugin::HistogramDialog::HistogramDialog( QWidget *parent, isis::viewer::QViewerCore *core )
	: QDialog( parent ),
	  m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	m_Plotter = new QwtPlot( this );
	m_Zoomer =  new QwtPlotZoomer( QwtPlot::xBottom, QwtPlot::yLeft, QwtPicker::DragSelection, QwtPicker::AlwaysOn, m_Plotter->canvas() );
	m_Plotter->setBackgroundRole( QPalette::Light );
	m_Plotter->setAxisTitle( 2, tr( "Intensity" ) );
	m_Plotter->setAxisTitle( 0, tr( "#voxels" ) );
	m_Grid = new QwtPlotGrid();
	m_Grid->attach( m_Plotter );
	QPen pen;
	pen.setStyle( Qt::DashDotLine );
	m_Grid->setPen( pen );
	layout()->addWidget( m_Plotter );
	connect( m_ViewerCore, SIGNAL( emitUpdateScene() ), this, SLOT( paintHistogram() ) );
}


void isis::viewer::plugin::HistogramDialog::paintHistogram()
{
	m_Plotter->clear();
	if( m_ViewerCore->hasImage() && isVisible() ) {
		std::stringstream title;
		QwtDoubleRect zoomBase;
		title << "Histogram of " << boost::filesystem::path( m_ViewerCore->getCurrentImage()->getFileNames().front() ).leaf();

		if( m_ViewerCore->getCurrentImage()->getImageSize()[3] > 1 ) {
			title << " (volume " << m_ViewerCore->getCurrentImage()->voxelCoords[3] << ")";
		}
		
		m_Plotter->setTitle( title.str().c_str() );
		double xData[256];
		BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
			if( !image.second->isRGB ) {
				const double scaling = image.second->scalingToInternalType.first->as<double>();
				const double offset = image.second->scalingToInternalType.second->as<double>();

				for( unsigned short i = 0; i < 256; i++ ) {
					xData[i] = ( double )( i - offset ) / scaling ;
				}

				QwtPlotCurve *curve = new QwtPlotCurve();
				curve->detach();

				if ( image.second.get() == m_ViewerCore->getCurrentImage().get() ) {
					curve->attach( m_Plotter );
					curve->setPen( QPen( Qt::red ) );
				} else {
					if( image.second->isVisible ) {
						curve->attach( m_Plotter );
					}

					QPen pen;
					pen.setBrush( QBrush( Qt::gray, Qt::Dense1Pattern ) );
					curve->setPen( pen );
				}

				const uint16_t timestep = image.second->getImageSize()[3] > 1 ? image.second->voxelCoords[3] : 0;

				curve->setData( xData, image.second->histogramVector[timestep], 256 );
				
				zoomBase.setLeft( image.second->minMax.first->as<int>() < zoomBase.left() ? image.second->minMax.first->as<int>() * 1.1 : zoomBase.left() );
				zoomBase.setWidth( image.second->extent > zoomBase.width() ? image.second->extent * 1.1 : zoomBase.width() );
				zoomBase.setTop( curve->maxYValue() > zoomBase.top() ? curve->maxYValue() * 1.1 : zoomBase.top() );
				zoomBase.setBottom( curve->minYValue() < zoomBase.bottom() ? curve->minYValue() : zoomBase.bottom() );
			}
		}
		m_Zoomer->setZoomBase(zoomBase);
		m_Plotter->replot();
	}
}
void isis::viewer::plugin::HistogramDialog::showEvent( QShowEvent * )
{
	paintHistogram();
}
