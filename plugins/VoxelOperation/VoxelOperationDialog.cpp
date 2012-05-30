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
 * VoxelOperationDialog.cpp
 *
 * Description:
 *
 *  Created on: Apr 18, 2012
 *      Author: tuerke
 ******************************************************************/
#include "VoxelOperationDialog.hpp"
#include <uicore.hpp>

namespace isis
{
namespace viewer
{
namespace plugin
{

VoxelOperationDialog::VoxelOperationDialog ( QWidget *parent, QViewerCore *core )
	: QDialog( parent ),
	  m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	connect( m_Interface.calcButton, SIGNAL( pressed() ), this, SLOT( calculatePressed() ) );
	connect( m_Interface.helpButton, SIGNAL( pressed() ), this, SLOT( helpPressed() ) );
}

void VoxelOperationDialog::helpPressed()
{
	QDesktopServices::openUrl( QUrl( "http://muparser.sourceforge.net/mup_features.html" ) );
}


void VoxelOperationDialog::calculatePressed()
{
	if( m_ViewerCore->hasImage() ) {
		const std::string op = m_Interface.operationInput->text().toStdString();

		try {
			_internal::VoxelOp vop( op );
			const ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();
			m_Interface.calcButton->setEnabled( false );
			m_Interface.calcButton->setText( "Calculating..." );
			std::stringstream ss;
			ss << "Calculating " << op << "...";
			m_ViewerCore->getUICore()->toggleLoadingIcon( true, ss.str().c_str() );
			image->getISISImage()->foreachVoxel<double>( vop );
			m_ViewerCore->getUICore()->toggleLoadingIcon( false );
			util::slist voxelOpHistory;

			if( image->getPropMap().hasProperty( "VoxelOperation/opHistory" ) ) {
				voxelOpHistory = image->getPropMap().getPropertyAs<util::slist>( "VoxelOperation/opHistory" );
			}

			voxelOpHistory.push_back( op );
			image->getPropMap().setPropertyAs<util::slist>( "VoxelOperation/opHistory", voxelOpHistory );
			std::stringstream ss1;
			ss1 << "VoxelOperation: " << op;
			image->addChangedAttribute( ss1.str() );
			image->synchronize( );
			image->updateColorMap();
			m_ViewerCore->emitImageContentChanged( image );
			m_ViewerCore->getUICore()->refreshUI( true );
		} catch( mu::Parser::exception_type &e ) {
			QMessageBox msgBox;
			msgBox.setText( e.GetMsg().c_str() );
			msgBox.exec();

		}

		m_Interface.calcButton->setEnabled( true );
		m_Interface.calcButton->setText( "Calculate" );
	}

}


}
}
}