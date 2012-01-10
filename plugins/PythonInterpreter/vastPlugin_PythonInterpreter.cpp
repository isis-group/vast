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
 * vastPlugin_PythonInterpreter.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "plugininterface.h"
#include "PythonInterpreterDialog.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{

class PythonInterpreter : public PluginInterface
{
public:
	PythonInterpreter() : isInitialized( false ) {};
	virtual std::string getName() { return std::string( "Python Interpreter" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "" ); }
	virtual QKeySequence getShortcut() { return QKeySequence( "P, I" ) ;}
	virtual bool isGUI() { return true; }
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/pythonInterpreter.png" ); }
	virtual bool call() {
		if( !isInitialized ) {
			m_Dialog = new PyhtonInterpreterDialog( parentWidget, viewerCore );
			isInitialized = true;
		}

		m_Dialog->show();
		return true;
	}

private:
	PyhtonInterpreterDialog *m_Dialog;
	bool isInitialized;

};



}
}
}



isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::PythonInterpreter();
}