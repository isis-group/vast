#ifndef PLOTTERDIALOG_HPP
#define PLOTTERDIALOG_HPP

#include <QtGui>
#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
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
	PlotterDialog( QWidget *parent, QViewerCore *core ) : QDialog( parent ), m_Core( core ) {
		ui.setupUi( this );
		plot = new QwtPlot( tr( "Timecourse" ), ui.widget );
		curve = new QwtPlotCurve();
		ui.verticalLayout->addWidget( plot );
		plot->setAxisTitle( 2, tr( "Timestep" ) );
		plot->setAxisTitle( 0, tr( "Intensity" ) );
		plot->setBackgroundRole( QPalette::Light );
		connect( m_Core, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, ( SLOT( refresh( util::fvector4 ) ) ) );

		if( m_Core->hasImage() ) {
			refresh( m_Core->getCurrentImage()->physicalCoords );
		}

	};
public Q_SLOTS:
	virtual void refresh( util::fvector4 physicalCoords ) {
		if( !ui.checkLock->isChecked() ) {
			boost::shared_ptr<ImageHolder> image = m_Core->getCurrentImage();

			if( image->getImageSize()[3] > 1 && image->isVisible ) {
				std::stringstream title;
				std::stringstream coordsAsString;
				util::ivector4 voxCoords = image->getISISImage()->getIndexFromPhysicalCoords( physicalCoords );

				if ( !image->isInsideImage( voxCoords ) ) return;

				title << "Timecourse for " << m_Core->getCurrentImage()->getFileNames().front();
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
					case ValuePtr<int8_t>::staticID:
						fillVector<int8_t>( intensityValues, t, voxCoords );
						break;
					case ValuePtr<uint8_t>::staticID:
						fillVector<uint8_t>( intensityValues, t, voxCoords );
						break;
					case ValuePtr<int16_t>::staticID:
						fillVector<int16_t>( intensityValues, t, voxCoords );
						break;
					case ValuePtr<uint16_t>::staticID:
						fillVector<uint16_t>( intensityValues, t, voxCoords );
						break;
					case ValuePtr<int32_t>::staticID:
						fillVector<int32_t>( intensityValues, t, voxCoords );
						break;
					case ValuePtr<uint32_t>::staticID:
						fillVector<uint32_t>( intensityValues, t, voxCoords );
						break;
					case ValuePtr<int64_t>::staticID:
						fillVector<int64_t>( intensityValues, t, voxCoords );
						break;
					case ValuePtr<uint64_t>::staticID:
						fillVector<uint64_t>( intensityValues, t, voxCoords );
						break;
					case ValuePtr<float>::staticID:
						fillVector<float>( intensityValues, t, voxCoords );
						break;
					case ValuePtr<double>::staticID:
						fillVector<double>( intensityValues, t, voxCoords );
						break;
					}
				}

				curve->setData( timeSteps, intensityValues );
				curve->attach( plot );
				curve->setPen( QPen( Qt::red ) );
				plot->replot();

			} else {
				curve->detach();
				plot->setTitle( QString( "Image has only one timestep!" ) );
			}
		}
	}
private:
	Ui::plottingDialog ui;
	QwtPlot *plot;
	QwtPlotCurve *curve;
	QViewerCore *m_Core;
	template<typename TYPE>
	void fillVector( QVector<double> &iv, const size_t &t, const util::ivector4 &vox ) {
		iv.push_back( m_Core->getCurrentImage()->getISISImage()->voxel<TYPE>( vox[0], vox[1], vox[2], t ) );
	}

};

}
}
}


#endif