/****************************************************************
 *
 * <Copyright information>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Author: Erik Tuerke, tuerke@cbs.mpg.de
 *
 * vastPlugin_Histogram.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
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
	Histogram() : isInitialized( false ) {}
	virtual QKeySequence getShortcut() { return QKeySequence( "I, H" ) ;}
	virtual std::string getName() { return std::string( "Histogram" ) ; }
	virtual std::string getDescription() { return std::string( "Plots the image histogram" ); }
	virtual std::string getTooltip() { return std::string( "" ); }
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/histogram.gif" ); }
	virtual bool isGUI() { return true; }
	virtual bool call() {
		if( !isInitialized ) {
			histogramDialog = new HistogramDialog( parentWidget, viewerCore );
			isInitialized = true;
		}

		if( viewerCore->hasImage() ) {
			histogramDialog->show();
		} else {
			QMessageBox msg( parentWidget );
			msg.setText( "No image has been loaded or selected!" );
			msg.exec();
		}

		return true;
	};

	virtual ~Histogram() {};


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