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
#include <qwt_plot_scaleitem.h>
#include <qwt_scale_widget.h>

isis::viewer::plugin::PlotterDialog::PlotterDialog ( QWidget *parent, isis::viewer::QViewerCore *core )	: QDialog( parent ), m_ViewerCore( core )
{
	ui.setupUi( this );
	ui.timeCourseRadio->setChecked( true );
	plot = new Plot( tr( "Timecourse" ), ui.widget );
	grid = new QwtPlotGrid();
	plotMarker = new QwtPlotMarker();
	plotMarker->setLineStyle( QwtPlotMarker::VLine );
	ui.verticalLayout->addWidget( plot );
	plot->setAxisTitle( 2, tr( "Timestep" ) );
	plot->setAxisTitle( 0, tr( "Intensity" ) );
	plot->setBackgroundRole( QPalette::Light );
	plot->setFont( QFont( "", 2 ) );
	
	connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector3 ) ), this, ( SLOT( refresh( util::fvector3 ) ) ) );
	connect( m_ViewerCore, SIGNAL( emitUpdateScene() ), this, SLOT( updateScene() ) );
	connect( ui.comboAxis, SIGNAL( currentIndexChanged( int ) ), this, SLOT( updateScene() ) );
	connect( ui.timeCourseRadio, SIGNAL( clicked( bool ) ), this, SLOT( updateScene() ) );
	connect( ui.spectrumRadio, SIGNAL( clicked( bool ) ), this, SLOT( updateScene() ) );
	ui.comboAxis->addItem( "X" );
	ui.comboAxis->addItem( "Y" );
	ui.comboAxis->addItem( "Z" );
	ui.comboAxis->addItem( "time" );

	if( m_ViewerCore->hasImage() ) {
		refresh( m_ViewerCore->getCurrentImage()->getImageProperties().physicalCoords );
	}

	setMinimumHeight( 200 );
	setMaximumHeight( 500 );

	//TODO
	ui.spectrumRadio->setEnabled( false );
	ui.spectrumRadio->setToolTip( tr( "Not yet supported." ) );

}

void isis::viewer::plugin::PlotterDialog::showEvent ( QShowEvent * )
{
	if( m_ViewerCore->hasImage() ) {
		int i = 3;

		while ( m_ViewerCore->getCurrentImage()->getImageSize()[i] <= 1 && i >= 0 ) {
			i--;
		}

		ui.comboAxis->setCurrentIndex( i );
	}

	updateScene();
}

void isis::viewer::plugin::PlotterDialog::updateScene()
{
	if( m_ViewerCore->hasImage() ) {
		m_ViewerCore->getCurrentImage()->getImageProperties().physicalCoords = m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( m_ViewerCore->getCurrentImage()->getImageProperties().voxelCoords ) ;
		refresh( m_ViewerCore->getCurrentImage()->getImageProperties().physicalCoords );
	}
}

void isis::viewer::plugin::PlotterDialog::refresh ( isis::util::fvector3 physicalCoords )
{
	if( !ui.checkLock->isChecked() && isVisible() ) {
		m_CurrentPhysicalCoords = physicalCoords;
		plot->clear();
		BOOST_FOREACH( ImageHolder::Vector::const_reference image, m_ViewerCore->getImageVector() ) {
			const unsigned short axis = image->getISISImage()->mapScannerAxisToImageDimension( static_cast<isis::data::scannerAxis>( ui.comboAxis->currentIndex() ) );

			if( image->getImageSize()[axis] > 1 ) {
				QwtPlotCurve *curve = new QwtPlotCurve();
				curve->detach();

				util::ivector4 voxCoords = image->getISISImage()->getIndexFromPhysicalCoords( physicalCoords );
				image->correctVoxelCoords<3>( voxCoords );

				if( ui.timeCourseRadio->isChecked() ) {
					fillProfile( image, voxCoords, curve, axis );
				} else {
					fillSpectrum( image, voxCoords, curve, axis );
				}

				if( image.get() == m_ViewerCore->getCurrentImage().get() || m_ViewerCore->getMode() == ViewerCoreBase::statistical_mode ) {
					curve->attach( plot );
					plotMarker->setSpacing(11);
					plotMarker->attach( plot );
					curve->setPen( QPen( Qt::red ) );
				} else {
					if( image->getImageProperties().isVisible ) {
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
		plotMarker->detach();
	}
}




void isis::viewer::plugin::PlotterDialog::fillProfile ( boost::shared_ptr< isis::viewer::ImageHolder > image, const isis::util::ivector4 &voxCoords, QwtPlotCurve *curve, const unsigned short &axis )
{
	std::stringstream title;
	std::stringstream coordsAsString;
	float factor = 1;

	if( axis == 3 ) {
		title << "Timecourse for " << m_ViewerCore->getCurrentImage()->getImageProperties().fileName;
		coordsAsString << "<" <<  voxCoords[0] << "|" << voxCoords[1] << "|" << voxCoords[2] << ">";
		plot->setTitle( coordsAsString.str().c_str() );
		setWindowTitle( title.str().c_str() );


		if ( image->getISISImage()->hasProperty( "repetitionTime" ) ) {
			factor = ( float )image->getISISImage()->getPropertyAs<uint16_t>( "repetitionTime" ) / 1000;
			plot->setAxisTitle( 2, tr( "t / s" ) );
		} else {
			plot->setAxisTitle( 2, tr( "Repetition (missing TR)" ) );
		}
	} else {
		title << "Profile for " <<  m_ViewerCore->getCurrentImage()->getImageProperties().fileName;
		std::stringstream plotTitle;
		std::stringstream axisTitle;
		plotTitle << "Profile for axis " << ui.comboAxis->currentText().toStdString();
		axisTitle << ui.comboAxis->currentText().toStdString() << " / mm";
		plot->setTitle( plotTitle.str().c_str() );
		plot->setAxisTitle( 2, axisTitle.str().c_str() );
		setWindowTitle( title.str().c_str() );
	}

	plotMarker->setXValue( image->getISISImage()->getPhysicalCoordsFromIndex( voxCoords )[ui.comboAxis->currentIndex()] * factor );

	QVector<double> xValues;
	QVector<double> intensityValues;
	using namespace isis::data;
	util::ivector4 _coords = voxCoords;

	for ( size_t i = 0; i < image->getImageSize()[axis]; i++ ) {
		_coords[axis] = i;

		if( axis != 3 ) {
			xValues.push_back( image->getISISImage()->getPhysicalCoordsFromIndex( _coords )[ui.comboAxis->currentIndex()] );
		} else {
			xValues.push_back( factor * i );
		}
	}

	switch( image->getImageProperties().majorTypeID ) {
	case ValueArray<bool>::staticID:
		fillVector<bool>( intensityValues, _coords, image, axis );
		break;
	case ValueArray<int8_t>::staticID:
		fillVector<int8_t>( intensityValues, _coords, image, axis );
		break;
	case ValueArray<uint8_t>::staticID:
		fillVector<uint8_t>( intensityValues, _coords, image, axis );
		break;
	case ValueArray<int16_t>::staticID:
		fillVector<int16_t>( intensityValues, _coords, image, axis );
		break;
	case ValueArray<uint16_t>::staticID:
		fillVector<uint16_t>( intensityValues, _coords, image, axis );
		break;
	case ValueArray<int32_t>::staticID:
		fillVector<int32_t>( intensityValues, _coords, image, axis );
		break;
	case ValueArray<uint32_t>::staticID:
		fillVector<uint32_t>( intensityValues, _coords, image, axis );
		break;
	case ValueArray<int64_t>::staticID:
		fillVector<int64_t>( intensityValues, _coords, image, axis );
		break;
	case ValueArray<uint64_t>::staticID:
		fillVector<uint64_t>( intensityValues, _coords, image, axis );
		break;
	case ValueArray<float>::staticID:
		fillVector<float>( intensityValues, _coords, image, axis );
		break;
	case ValueArray<double>::staticID:
		fillVector<double>( intensityValues, _coords, image, axis );
		break;
	}

	curve->setData( xValues, intensityValues );
	

}

void isis::viewer::plugin::PlotterDialog::fillSpectrum ( boost::shared_ptr< isis::viewer::ImageHolder > image, const isis::util::ivector4 &voxCoords, QwtPlotCurve *curve, const unsigned short &axis )
{
	plot->setAxisTitle( 2, tr( "1 / Hz" ) );
	plot->setAxisTitle( 0, tr( "" ) );

	double powermin = 10000000, powermax = -10000000;
	fftw_plan plan;

	size_t n = image->getImageSize()[3];

	const size_t nc = ( n / 2 ) + 1;
	double *in = ( double * ) fftw_malloc( sizeof( double ) * n );
	fftw_complex *out = ( fftw_complex * ) fftw_malloc ( sizeof ( fftw_complex ) * nc );

	plan = fftw_plan_dft_r2c_1d( n, in, out, FFTW_ESTIMATE );
	util::ivector4 _coords = voxCoords;

	for( size_t i = 0; i < n; i++ ) {
		_coords[axis] = i;
		in[i] = image->getISISImage()->voxel<int16_t>( _coords[0], _coords[1], _coords[2], image->getImageProperties().timestep );
	}

	fftw_execute( plan );
	QVector<double> yVec( nc + 2 );
	QVector<double> xVec( nc + 2 );

	for ( int k = 1; k < static_cast<int>( nc ); k++ ) {
		yVec[k] = ( double )sqrt( out[k][0] * out[k][0] + out[k][1] * out[k][1] );

		if ( powermin > yVec[k] ) powermin = yVec[k];

		if ( powermax < yVec[k] ) powermax = yVec[k];

		xVec[k] = k;
	}

	yVec[0] = 0.0;
	yVec[nc] = powermin;
	xVec[nc] = nc;
	yVec[nc+1] = powermax;
	xVec[nc+1] = nc + 1;
	curve->setData( xVec, yVec );


}

void isis::viewer::plugin::PlotterDialog::setYScaleMax(double val)
{
	if(val == 0 && ui.yScaleMin->value()==0){
		plot->setAxisAutoScale(QwtPlot::yLeft);
	} else {
		plot->setAxisScale(QwtPlot::yLeft,ui.yScaleMin->value(),ui.yScaleMax->value());
	}
	plot->updateAxes();
	plot->replot();
}
void isis::viewer::plugin::PlotterDialog::setYScaleMin(double val)
{
	if(val == 0 && ui.yScaleMax->value()==0){
		plot->setAxisAutoScale(QwtPlot::yLeft);
	} else {
		plot->setAxisScale(QwtPlot::yLeft,ui.yScaleMin->value(),ui.yScaleMax->value());
	}
	plot->updateAxes();
	plot->replot();
}








