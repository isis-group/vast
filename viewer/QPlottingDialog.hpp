#ifndef QPLOTTINGDIALOG_HPP
#define QPLOTTINGDIALOG_HPP

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "ui_plotting.h"
#include "ImageHolder.hpp"

namespace isis {
namespace viewer {
	
class QPlottingDialog : public QDialog
{
	Q_OBJECT;
	
	typedef std::vector<boost::shared_ptr<ImageHolder> > ImageVector;
	typedef std::vector< QwtPlotCurve* > CurveVector;
	
public:
	explicit QPlottingDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

public Q_SLOTS:
	void addImageHolder( boost::shared_ptr<ImageHolder> imageHolder );
	
public:
	template<typename TYPE>
	void replotVoxelCoords( util::ivector4 coords ) {
		if( !plottingUi.checkBox->isChecked() ) {
			std::stringstream title;
			std::stringstream coordsAsString;
			title << "Timecourse for " << m_Images.front()->getFileNames().front();
			coordsAsString << coords[0] << " : " << coords[1] << " : " << coords[2];
			plot->setTitle( tr( coordsAsString.str().c_str() ) );
			setWindowTitle( tr( title.str().c_str() ) );
			typedef ImageVector::iterator ImageIterator;
			typedef CurveVector::iterator CurveIterator;
			ImageIterator image ;
			CurveIterator curve ;
			
			for( image = m_Images.begin(), curve = m_Curves.begin(); image != m_Images.end(); image++, curve++)
			{
				uint16_t repTime = 1;
				if( (*image)->getImage()->hasProperty("repetitionTime") ) {
					repTime = (float)(*image)->getImage()->getPropertyAs<uint16_t>("repetitionTime") / 1000;
					plot->setAxisTitle(2, tr( "t / s" ) );
				}
				QVector<double> timeSteps;
				QVector<double> intensityValues;
				util::Value<TYPE> intens;
				for( size_t t = 0; t < (*image)->getImageSize()[3]; t++ ) {
					timeSteps.push_back( t * repTime );
					intens = (*image)->getImage()->voxel<TYPE>(coords[0], coords[1], coords[2], t );
					intensityValues.push_back( intens );
				}
				(*curve)->setData(timeSteps, intensityValues);
				(*curve)->attach( plot );
			}
			plot->replot();
		}
	}
	
protected:
	Ui::plottingDialog plottingUi;
	
	QwtPlot* plot;
	
private:
	ImageVector m_Images;
	std::vector< QwtPlotCurve* > m_Curves;
	
		
};
	
}}





#endif
