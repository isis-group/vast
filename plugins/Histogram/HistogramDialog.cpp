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
	
	if( m_ViewerCore->hasImage() ) {
		std::stringstream title;
		QwtDoubleRect zoomBase;
		title << "Histogram of " << boost::filesystem::path( m_ViewerCore->getCurrentImage()->getFileNames().front() ).filename();

		if( m_ViewerCore->getCurrentImage()->getImageSize()[3] > 1 ) {
			title << " (volume " << m_ViewerCore->getCurrentImage()->voxelCoords[3] << ")";
		}
		
		m_Plotter->setTitle( title.str().c_str() );
		double xData[256];
		BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
			if( !image.second->isRGB ) {
				const double scaling = image.second->getISISImage()->getScalingTo( isis::data::ValuePtr<InternalImageType>::staticID ).first->as<double>();
				const double offset = image.second->getISISImage()->getScalingTo( isis::data::ValuePtr<InternalImageType>::staticID ).second->as<double>();

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