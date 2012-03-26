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
 * vastPlugin_OrientationCorrection.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "plugininterface.h"
#include "OrientationCorrection.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{

class OrientationCorrection : public PluginInterface
{
public:
	OrientationCorrection() : isInitialized( false ) {}
	virtual std::string getName() { return std::string( "Orienation Correction" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "Allows you to correct the orientation of the image." ); }
	virtual QKeySequence getShortcut() { return QKeySequence( "O, C" ) ;}
	virtual bool isGUI() { return true; }
	virtual bool call() {
		if( !isInitialized ) {
			m_OrienatationCorrectionDialog = new OrientatioCorrectionDialog( parentWidget, viewerCore );
			isInitialized = true;
		}

		if( viewerCore->hasImage() ) {
			m_OrienatationCorrectionDialog->show();
		} else {
			QMessageBox msg( parentWidget );
			msg.setText( "No image has been loaded or selected!" );
			msg.exec();
		}

		return true;
	};

	virtual ~OrientationCorrection() {};
private:
	OrientatioCorrectionDialog *m_OrienatationCorrectionDialog;
	bool isInitialized;
};

}
}
}

isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::OrientationCorrection();
}