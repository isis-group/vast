#ifndef TIMEPLOTTER_HPP
#define TIMEPLOTTER_HPP

#include "plugininterface.h"
#include "HistogramDialog.hpp"
#include "common.hpp"
#include "viewercorebase.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{


class Histogram : public PluginInterface
{
public:
	Histogram() : isInitialized(false) {}
	virtual QKeySequence getShortcut() { return QKeySequence( "I, H" ) ;}
	virtual std::string getName() { return std::string( "Histogram" ) ; }
	virtual std::string getDescription() { return std::string( "Plots the image histogram" ); }
	virtual std::string getTooltip() { return std::string( "" ); }
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/Histogram.png" ); }
	virtual bool isGUI() { return true; }
	virtual bool call(){
		if( !isInitialized ) {
			histogramDialog = new HistogramDialog( parentWidget, viewerCore );
			isInitialized = true;
		}
		histogramDialog->show();
		return true;
	};

	virtual ~Histogram(){};


private:
	bool isInitialized;
	HistogramDialog *histogramDialog;
};

}
}
}

isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::Histogram();
}

#endif