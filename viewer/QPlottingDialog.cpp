#include "QPlottingDialog.hpp"


namespace isis {
namespace viewer {
	
QPlottingDialog::QPlottingDialog(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	plottingUi.setupUi(this);
	plot = new QwtPlot( tr( "Timecourse" ), plottingUi.widget );
	plottingUi.gridLayout->addWidget(plot);
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


void QPlottingDialog::replotVoxelCoords(util::ivector4 coords)
{
	std::stringstream title;
	title << "Timecourse for " << coords[0] << " " << coords[1] << " " << coords[2];
	plot->setTitle( tr( m_Images.front()->getFileNames().front().c_str() ) );
	setWindowTitle( tr( title.str().c_str() ) );
	typedef ImageVector::iterator ImageIterator;
	typedef CurveVector::iterator CurveIterator;
	ImageIterator image ;
	CurveIterator curve ;
	
	for( image = m_Images.begin(), curve = m_Curves.begin(); image != m_Images.end(); image++, curve++)
	{
		QVector<double> timeSteps;
		QVector<double> intensityValues;
		util::Value<int16_t> vIntensity;
		for( size_t t = 0; t < (*image)->getImageSize()[3]; t++ ) {
			timeSteps.push_back(t);
			vIntensity = (*image)->getImage()->voxel<int16_t>(coords[0], coords[1], coords[2], t );
			intensityValues.push_back( vIntensity.as<double>());
		}
		(*curve)->setData(timeSteps, intensityValues);
		(*curve)->attach( plot );
	}
	plot->replot();
	
}


	
	
}}