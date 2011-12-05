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
	PlotterDialog( QWidget *parent, QViewerCore *core ) : QDialog( parent ), m_ViewerCore( core ) {
		ui.setupUi( this );
		plot = new QwtPlot( tr( "Timecourse" ), ui.widget );
		curve = new QwtPlotCurve();
		ui.verticalLayout->addWidget( plot );
		plot->setAxisTitle( 2, tr( "Timestep" ) );
		plot->setAxisTitle( 0, tr( "Intensity" ) );
		plot->setBackgroundRole( QPalette::Light );
		plot->setFont(QFont("",2 ) );
		connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, ( SLOT( refresh( util::fvector4 ) ) ) );

		if( m_ViewerCore->hasImage() ) {
			refresh( m_ViewerCore->getCurrentImage()->physicalCoords );
		}
		setMinimumHeight(200);
		setMaximumHeight(500);

	};
public Q_SLOTS:
	virtual void refresh( util::fvector4 physicalCoords ) {
		if( !ui.checkLock->isChecked() ) {
			boost::shared_ptr<ImageHolder> image = m_ViewerCore->getCurrentImage();
			if( !(image->getImageSize()[3] > 1) ) {
				BOOST_FOREACH( DataContainer::const_reference iRef, m_ViewerCore->getDataContainer() )
				{
					if ( iRef.second->getImageSize()[3] > 1 ) {
						image = iRef.second;
					}
				}
			}
			if( image->getImageSize()[3] > 1 ) {
				std::stringstream title;
				std::stringstream coordsAsString;
				util::ivector4 voxCoords = image->getISISImage()->getIndexFromPhysicalCoords( physicalCoords, true );

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
						fillVector<int16_t>( intensityValues, t, voxCoords, image  );
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
	QViewerCore *m_ViewerCore;
	template<typename TYPE>
	void fillVector( QVector<double> &iv, const size_t &t, const util::ivector4 &vox, boost::shared_ptr<ImageHolder> image ) {
		iv.push_back( image->getISISImage()->voxel<TYPE>( vox[0], vox[1], vox[2], t ) );
	}

};

}
}
}


#endif