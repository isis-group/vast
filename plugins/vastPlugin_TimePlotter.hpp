#include "PluginInterface.h"
#include <QtGui>

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