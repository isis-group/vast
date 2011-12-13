#ifndef HISTOGRAMDIALOG_HPP
#define HISTOGRAMDIALOG_HPP

#include "ui_histogramDialog.h"
#include "qviewercore.hpp"
#include "qwt_plot_curve.h"
#include "qwt_plot.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_zoomer.h"
#include "qwt_plot_picker.h"

namespace isis {
namespace viewer {
namespace plugin {
	

class HistogramDialog : public QDialog
{
	Q_OBJECT
public:
	HistogramDialog( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	void paintHistogram();
	void showEvent( QShowEvent * );
private:
	Ui::histogramDialog m_Interface;
	QViewerCore *m_ViewerCore;
	QwtPlot *m_Plotter;
	QwtPlotGrid *m_Grid;
	QwtPlotZoomer *m_Zoomer;
	
	
	
};
	
	
}}}



#endif