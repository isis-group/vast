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
 * vastPlugin_TimePlotter.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "vastPlugin_ProfilePlotter.hpp"



namespace isis
{
namespace viewer
{
namespace plugin
{


bool ProfilePlotter::call()
{
	m_PlotterDialog = new PlotterDialog( parentWidget, viewerCore );

	if( viewerCore->hasImage() ) {
		m_PlotterDialog->show();
	} else {
		QMessageBox msg( parentWidget );
		msg.setText( "No image has been loaded or selected!" );
		msg.exec();
	}

	return true;
}




}
}
}