#ifndef TIMEPLOTTER_HPP
#define TIMEPLOTTER_HPP

#include "plugininterface.h"
#include "PlotterDialog.hpp"
#include "common.hpp"
#include "viewercorebase.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{


class TimePlotter : public PluginInterface
{
public:

	virtual std::string getName() { return std::string( "TimePlotter" ) ; }
	virtual std::string getDescription() { return std::string( "Plots a timeseries" ); }
	virtual std::string getTooltip() { return std::string( "" ); }
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/graph.gif" ); }
	virtual bool isGUI() { return true; }
	virtual bool call();

	~TimePlotter() {};


private:

	PlotterDialog *m_PlotterDialog;

};


}
}
}

isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::TimePlotter();
}

#endif