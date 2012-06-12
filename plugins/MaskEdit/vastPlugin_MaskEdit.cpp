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
 * vastPlugin_MaskEdit.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "plugininterface.h"
#include "MaskEdit.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{


class MaskEdit : public PluginInterface
{
public:
	virtual std::string getName() { return std::string( "MaskEdit" ) ; }
	virtual std::string getDescription() { return std::string( "" ); }
	virtual std::string getTooltip() { return std::string( "Modifying and creating masks" ); }
	virtual QKeySequence getShortcut() { return QKeySequence( "M, E" ) ;}
	virtual bool isGUI() { return true; }
	virtual QIcon *getToolbarIcon() { return new QIcon( ":/common/maskEdit.png" ); }
	MaskEdit() :  m_MaskEditSet( false ) {}

	virtual bool call() {
		if( !m_MaskEditSet ) {
			m_MaskEdit = new MaskEditDialog( parentWidget, viewerCore ) ;
			viewerCore->getUICore()->getMainWindow()->getInterface().topGridLayout->addWidget( m_MaskEdit );
			m_MaskEditSet = true;
		}

		m_MaskEdit->show();
		return true;
	};

	virtual ~MaskEdit() {};
private:
	MaskEditDialog *m_MaskEdit;
	bool m_MaskEditSet;

};


}
}
}


isis::viewer::plugin::PluginInterface *loadPlugin()
{
	return new isis::viewer::plugin::MaskEdit();
}