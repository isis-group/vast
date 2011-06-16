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
	void replotVoxelCoords( util::ivector4 coords );
	
protected:
	Ui::plottingDialog plottingUi;
	
	QwtPlot* plot;
	
private:
	ImageVector m_Images;
	std::vector< QwtPlotCurve* > m_Curves;
	
		
};
	
}}





#endif
