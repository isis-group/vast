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
		
#include "PlotterDialog.hpp"

#include <fftw3.h>

isis::viewer::plugin::PlotterDialog::PlotterDialog ( QWidget* parent, isis::viewer::QViewerCore* core ) 
	: QDialog( parent ), m_ViewerCore( core ) {
	ui.setupUi( this );
	ui.timeCourseRadio->setChecked( true );
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

}

void isis::viewer::plugin::PlotterDialog::refresh ( isis::util::fvector4 physicalCoords )
{
	m_CurrentPhysicalCoords = physicalCoords;
	plot->clear();

	if( !ui.checkLock->isChecked() && isVisible()) {
		BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
			if( image.second->getImageSize()[3] > 1 ) {
				QwtPlotCurve *curve = new QwtPlotCurve();
				curve->detach();
				
				const util::ivector4 voxCoords = image.second->getISISImage()->getIndexFromPhysicalCoords( physicalCoords, true );
				if( ui.timeCourseRadio->isChecked() ) {
					fillTimeCourse( image.second, voxCoords, curve );
				} else {
					fillSpectrum( image.second, voxCoords, curve );
				}
				
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




void isis::viewer::plugin::PlotterDialog::fillTimeCourse ( boost::shared_ptr< isis::viewer::ImageHolder > image, const isis::util::ivector4& voxCoords, QwtPlotCurve* curve )
{
	std::stringstream title;
	std::stringstream coordsAsString;
	title << "Timecourse for " << m_ViewerCore->getCurrentImage()->getFileNames().front();
	coordsAsString << voxCoords[0] << " : " << voxCoords[1] << " : " << voxCoords[2];
	plot->setTitle( coordsAsString.str().c_str() );
	setWindowTitle( title.str().c_str() );
	uint16_t repTime = 1;

	if ( image->getISISImage()->hasProperty( "repetitionTime" ) ) {
		repTime = ( float )image->getISISImage()->getPropertyAs<uint16_t>( "repetitionTime" ) / 1000;
		plot->setAxisTitle( 2, tr( "t / s" ) );
	} else {
		plot->setAxisTitle( 2, tr( "Repetition (missing TR)" ) );
	}

	QVector<double> timeSteps;
	QVector<double> intensityValues;
	using namespace isis::data;

	for ( size_t t = 0; t < image->getImageSize()[3]; t++ ) {
		timeSteps.push_back( t * repTime );

		switch( image->getISISImage()->getChunk( voxCoords[0], voxCoords[1], voxCoords[2], t ).getTypeID() ) {
		case ValuePtr<bool>::staticID:
			fillVector<bool>( intensityValues, t, voxCoords, image );
			break;
		case ValuePtr<int8_t>::staticID:
			fillVector<int8_t>( intensityValues, t, voxCoords, image );
			break;
		case ValuePtr<uint8_t>::staticID:
			fillVector<uint8_t>( intensityValues, t, voxCoords, image );
			break;
		case ValuePtr<int16_t>::staticID:
			fillVector<int16_t>( intensityValues, t, voxCoords, image );
			break;
		case ValuePtr<uint16_t>::staticID:
			fillVector<uint16_t>( intensityValues, t, voxCoords, image );
			break;
		case ValuePtr<int32_t>::staticID:
			fillVector<int32_t>( intensityValues, t, voxCoords, image );
			break;
		case ValuePtr<uint32_t>::staticID:
			fillVector<uint32_t>( intensityValues, t, voxCoords, image );
			break;
		case ValuePtr<int64_t>::staticID:
			fillVector<int64_t>( intensityValues, t, voxCoords, image );
			break;
		case ValuePtr<uint64_t>::staticID:
			fillVector<uint64_t>( intensityValues, t, voxCoords, image );
			break;
		case ValuePtr<float>::staticID:
			fillVector<float>( intensityValues, t, voxCoords, image );
			break;
		case ValuePtr<double>::staticID:
			fillVector<double>( intensityValues, t, voxCoords, image );
			break;
		}
	}

	curve->setData( timeSteps, intensityValues );

}

void isis::viewer::plugin::PlotterDialog::fillSpectrum ( boost::shared_ptr< isis::viewer::ImageHolder > image, const isis::util::ivector4& voxCoords, QwtPlotCurve* curve )
{
	plot->setAxisTitle( 2, tr( "1 / Hz" ) );
	plot->setAxisTitle( 0, tr( "" ) );
	
	double powermin=10000000, powermax=-10000000;
	fftw_plan plan;

	size_t n = image->getImageSize()[3];
	
	const size_t nc = (n / 2 ) + 1;
	double *in = (double *) fftw_malloc(sizeof(double) * n);
	fftw_complex *out = (fftw_complex *) fftw_malloc (sizeof (fftw_complex ) * nc);
	
	plan = fftw_plan_dft_r2c_1d(n, in, out, FFTW_ESTIMATE);
	
	for( size_t t = 0; t < n; t++ ) {
		in[t] = image->getISISImage()->voxel<int16_t>(voxCoords[0], voxCoords[1], voxCoords[2], t );
	}
	
	fftw_execute(plan); 
	QVector<double> yVec(nc + 2);
	QVector<double> xVec(nc + 2);
	for (int k = 1; k < static_cast<int>(nc); k++) {
      yVec[k] = (double)sqrt(out[k][0] * out[k][0] + out[k][1] * out[k][1]);
	  if (powermin>yVec[k]) powermin=yVec[k];
	  if (powermax<yVec[k]) powermax=yVec[k];
	  xVec[k] = k;
	}
	yVec[0] = 0.0;
	yVec[nc] = powermin;
	xVec[nc] = nc;
	yVec[nc+1] = powermax;
	xVec[nc+1] = nc + 1;
	curve->setData(xVec,yVec);
	
	
}









