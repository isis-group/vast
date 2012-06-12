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
 * vastPlugin_CorrelationPlotter.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "plugininterface.h"
#include "CorrelationPlotter.hpp"


namespace isis
{
namespace viewer
{
namespace plugin
{

class CorrelationPlotter : public PluginInterface
{
public:
	virtual std::string getName() { return std::string( "CorrelationPlotter" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "" ); }
	virtual QKeySequence getShortcut() { return QKeySequence( "C, P" ) ;}
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/Correlation.png" ); }
	virtual bool isGUI() { return true; }
	virtual bool call() {
		if( !m_CorrelationPlotterSet ) {
			m_CorrelationPlotter = new CorrelationPlotterDialog( parentWidget, viewerCore ) ;
			viewerCore->getUICore()->getMainWindow()->getInterface().topGridLayout->addWidget( m_CorrelationPlotter );
			m_CorrelationPlotterSet = true;
		}

		if( m_Visible ) {
			m_CorrelationPlotter->close();
		} else {
			m_CorrelationPlotter->show();
		}

		m_Visible = !m_Visible;
		return true;
	};
	CorrelationPlotter() : m_Visible( false ), m_CorrelationPlotterSet( false ) {}

	virtual ~CorrelationPlotter() {};
private:
	bool m_Visible;
	bool m_CorrelationPlotterSet;
	CorrelationPlotterDialog *m_CorrelationPlotter;


};




}
}
}


isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::CorrelationPlotter();
}