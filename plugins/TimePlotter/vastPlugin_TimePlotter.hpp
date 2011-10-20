#ifndef TIMEPLOTTER_HPP
#define TIMEPLOTTER_HPP

#include "PluginInterface.h"
#include <QtGui>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "ui_plotting.h"

namespace isis {
namespace viewer {
namespace plugin {
	
class TimePlotter : public PluginInterface
{
public:
	TimePlotter();
	
	virtual std::string getName() { return std::string( "Plotter/TimePlotter" ) ; }
	virtual std::string getDescription() { return std::string( "Plots a timeseries" ); }
	virtual std::string getTooltip() { return std::string(""); }
	virtual bool isGUI() { return true; }
	virtual bool call();

	~TimePlotter();
private:
	
	class PlotterDialog : public QDialog
	{
	public:
		PlotterDialog( QWidget *p ) : QDialog(p) {};
	};
	
	PlotterDialog *m_PlotterDialog;
	
};
	
	
}}}

isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::TimePlotter();
}

#endif