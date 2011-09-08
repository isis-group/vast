#include "QPlottingDialog.hpp"


namespace isis
{
namespace viewer
{

QPlottingDialog::QPlottingDialog( QWidget *parent, Qt::WindowFlags f )
	: QDialog( parent, f )
{
	plottingUi.setupUi( this );
	plot = new QwtPlot( tr( "Timecourse" ), plottingUi.widget );
	plottingUi.verticalLayout->addWidget( plot );
	plot->setAxisTitle( 2, tr( "Timestep" ) );
	plot->setAxisTitle( 0, tr( "Intensity" ) );
	plot->setBackgroundRole( QPalette::Light );
}

void QPlottingDialog::addImageHolder( boost::shared_ptr< ImageHolder > imageHolder )
{
	m_Images.push_back( imageHolder );
	m_Curves.push_back( new QwtPlotCurve( tr( imageHolder->getFileNames().front().c_str() ) ) );
}
void QPlottingDialog::setImageHolderList( std::list< boost::shared_ptr< ImageHolder > > imageHolderList )
{
	m_Images.clear();
	m_Curves.clear();
	BOOST_FOREACH(  ImageList::const_reference image, imageHolderList ) {
		if( image->getImageSize()[3] > 1 ) {
			m_Images.push_back( image );
			m_Curves.push_back( new QwtPlotCurve( tr( image->getFileNames().front().c_str() ) ) );
		}

	}
}





}
}