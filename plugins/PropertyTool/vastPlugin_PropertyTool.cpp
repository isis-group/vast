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
 * vastPlugin_PropertyTool.cpp
 *
 * Description:
 *
 *  Created on: Jan 05, 2012
 *      Author: tuerke
 ******************************************************************/
#include "plugininterface.h"
#include "PropertyToolDialog.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{

class PropertyTool : public PluginInterface
{
public:
	PropertyTool() : isInitialized( false ) {}
	virtual std::string getName() { return std::string( "Property Tool" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "Allows you to show and modify the meta data of the image." ); }
	virtual QKeySequence getShortcut() { return QKeySequence( "P, T" ) ;}
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/properties.png" ); }
	virtual bool isGUI() { return true; }
	virtual bool call() {
		if( !isInitialized ) {
			m_PropertyToolDialog = new PropertyToolDialog( parentWidget, viewerCore );
			isInitialized = true;
		}

		if( viewerCore->hasImage() ) {
			m_PropertyToolDialog->show();
		} else {
			QMessageBox msg( parentWidget );
			msg.setText( "No image has been loaded or selected!" );
			msg.exec();
		}

		return true;
	};

	virtual ~PropertyTool() {};
private:
	PropertyToolDialog *m_PropertyToolDialog;
	bool isInitialized;
};

}
}
}

isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::PropertyTool();
}