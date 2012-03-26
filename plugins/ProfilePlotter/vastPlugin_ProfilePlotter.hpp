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
 * Author: Erik Türke, tuerke@cbs.mpg.de
 *
 * vastPlugin_TimePlotter.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef PROFILEPLOTTER_HPP
#define PROFILEPLOTTER_HPP

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


class ProfilePlotter : public PluginInterface
{
public:
	virtual QKeySequence getShortcut() { return QKeySequence( "T, P" ) ;}
	virtual std::string getName() { return std::string( "Profile Plotter" ) ; }
	virtual std::string getDescription() { return std::string( "Plots profiles along X,Y,Z and time" ); }
	virtual std::string getTooltip() { return std::string( "" ); }
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/graph.gif" ); }
	virtual bool isGUI() { return true; }
	virtual bool call();

	~ProfilePlotter() {};


private:

	PlotterDialog *m_PlotterDialog;

};


}
}
}

isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::ProfilePlotter();
}

#endif