#include "QPlottingDialog.hpp"


namespace isis {
namespace viewer {
	
QPlottingDialog::QPlottingDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	plottingUi.setupUi(this);
	plot = new QwtPlot( tr( "Timecourse" ), plottingUi.widget );
	plottingUi.verticalLayout->addWidget(plot);
	plot->setAxisTitle(2, tr("Timestep") );
	plot->setAxisTitle(0, tr("Intensity") );
	plot->setBackgroundRole( QPalette::Light );
}

void QPlottingDialog::addImageHolder(boost::shared_ptr< ImageHolder > imageHolder)
{
	m_Images.push_back( imageHolder );
	m_Curves.push_back( new QwtPlotCurve( tr( imageHolder->getFileNames().front().c_str() ) ) );
}



	
	
}}