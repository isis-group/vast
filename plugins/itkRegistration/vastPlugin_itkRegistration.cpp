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
 * vastPlugin_itkRegistration.cpp
 *
 * Description:
 *
 *  Created on: Apr 17, 2012
 *      Author: tuerke
 ******************************************************************/
#include "plugininterface.h"
#include "RegistrationDialog.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{

class itkRegistration : public PluginInterface
{
public:
	itkRegistration() : isInitialized( false ) {}
	virtual std::string getName() { return std::string( "itk/Registration" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "itk Registration plugin" ); }
	virtual QKeySequence getShortcut() { return QKeySequence( "I, R" ) ;}
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/itkLogo.jpg" ); }
	virtual bool isGUI() { return true; }
	virtual bool call() {
		if( !isInitialized ) {
			m_RegistrationDialgo = new RegistrationDialog( parentWidget, viewerCore );
			isInitialized = true;
		}

		if( viewerCore->hasImage() ) {
			m_RegistrationDialgo->show();
		} else {
			QMessageBox msg( parentWidget );
			msg.setText( "No image has been loaded or selected!" );
			msg.exec();
		}

		return true;
	};

	virtual ~itkRegistration() {};
private:
	RegistrationDialog *m_RegistrationDialgo;
	bool isInitialized;
};

}
}
}

isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::itkRegistration();
}