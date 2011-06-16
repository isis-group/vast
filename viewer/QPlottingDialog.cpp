#include "QPlottingDialog.hpp"


namespace isis {
namespace viewer {
	
QPlottingDialog::QPlottingDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	plottingUi.setupUi(this);
	plot = new QwtPlot( tr( "Timecourse" ), plottingUi.widget );
	plottingUi.verticalLayout->addWidget(plot);
	plot->setAxisTitle(2, tr( "Timestep" ) );
	plot->setAxisTitle(0, tr("Intensity") );
	
	//just some tests
	QVector<double> testVecY;
	QVector<double> testVecX;
	testVecY.push_back(0.4);
	testVecY.push_back(0.2);
	testVecY.push_back(0.3);
	testVecX.push_back(0);
	testVecX.push_back(1);
	testVecX.push_back(2);
	
	m_Curves.push_back( new QwtPlotCurve(tr("gna") ) );
	m_Curves[0]->setData(testVecX, testVecY);
	m_Curves[0]->attach( plot );
	plot->replot();
	
	
}

void QPlottingDialog::addImageHolder(boost::shared_ptr< ImageHolder > imageHolder)
{
	m_Images.push_back( imageHolder );
	m_Curves.push_back( new QwtPlotCurve( tr( imageHolder->getFileNames().front().c_str() ) ) );
}



	
	
}}