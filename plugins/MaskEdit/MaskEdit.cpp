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
 * MaskEdit.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "MaskEdit.hpp"
#include <DataStorage/image.hpp>
#include "uicore.hpp"
#include "common.hpp"
#include <CoreUtils/vector.hpp>


namespace isis
{
namespace viewer
{
namespace plugin
{

MaskEditDialog::MaskEditDialog( QWidget *parent, QViewerCore *core )
	: QDialog( parent ),
	  m_ViewerCore( core ),
	  m_Radius( 2 ),
	  m_CreateMaskDialog( new CreateMaskDialog( parent, this ) )
{
	m_Interface.setupUi( this );
	m_Interface.pickColor->setEnabled( false );
	m_Interface.radius->setEnabled( false );
	m_Interface.radius->setMaximum( 500 );
	m_Interface.radius->setValue( m_Radius );

	m_Interface.colorEdit->setMinimum( -std::numeric_limits<double>::max() );
	m_Interface.colorEdit->setMaximum( std::numeric_limits<double>::max() );

	util::Singletons::get<color::Color, 10>().addColormap( ":/common/maskeditLUT" );
	connect( m_Interface.newMask, SIGNAL( clicked() ), this, SLOT( createEmptyMask() ) ) ;
	connect( m_Interface.radius, SIGNAL( valueChanged( int ) ), SLOT( radiusChange( int ) ) );
	connect( m_Interface.editCurrentImage, SIGNAL( clicked() ), this, SLOT( editCurrentImage() ) );
	connect( m_Interface.pickColor, SIGNAL( clicked( bool ) ), this, SLOT( pickColorClicked() ) );
	connect( m_Interface.closeButton, SIGNAL( clicked( bool ) ), this, SLOT( close() ) );
	connect( m_Interface.paint, SIGNAL( clicked( bool ) ), this, SLOT( paintClicked() ) );
	connect( m_Interface.cut, SIGNAL( clicked( bool ) ), this, SLOT( cutClicked() ) );

}

void MaskEditDialog::cutClicked()
{
	if( m_CurrentMask )
		m_Interface.colorEdit->setValue( m_CurrentMask->getImageProperties().minMax.first->as<double>() );
}

void MaskEditDialog::paintClicked()
{
	if( m_CurrentMask )
		m_Interface.colorEdit->setValue( m_CurrentMask->getImageProperties().minMax.second->as<double>() );
}


void MaskEditDialog::pickColorClicked()
{
	if( m_Interface.pickColor->isChecked() ) {
		BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, *m_CurrentWidgetEnsemble ) {
			ensembleComponent->getWidgetInterface()->setMouseCursorIcon( QIcon( ":/common/colorpicker.png" ) );
		}
		m_ViewerCore->setShowCrosshair( false );
		m_ViewerCore->updateScene();
	} else {
		paintToggled();
	}
}


void MaskEditDialog::paintToggled()
{
	BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, *m_CurrentWidgetEnsemble ) {
		ensembleComponent->getWidgetInterface()->setMouseCursorIcon( QIcon( ":/common/paintCrosshair.png" ) );
	}

	m_ViewerCore->setShowCrosshair( false );
	m_ViewerCore->updateScene();
}



void MaskEditDialog::radiusChange( int r )
{
	m_Radius = r;
}


void MaskEditDialog::showEvent( QShowEvent * )
{
	connect( m_ViewerCore, SIGNAL ( emitOnWidgetMoved( util::fvector3, Qt::MouseButton ) ), this, SLOT( physicalCoordChanged( util::fvector3, Qt::MouseButton ) ) );
	connect( m_ViewerCore, SIGNAL ( emitOnWidgetClicked( util::fvector3, Qt::MouseButton ) ), this, SLOT( physicalCoordChanged( util::fvector3, Qt::MouseButton ) ) );

	if( !m_CurrentMask ) {
		m_Interface.radius->setEnabled( false );
	} else {
		m_Interface.radius->setEnabled( true );
	}

}


void MaskEditDialog::physicalCoordChanged( util::fvector3 physCoord, Qt::MouseButton mouseButton )
{
	if( mouseButton == Qt::LeftButton && m_ViewerCore->hasImage() ) {
		if( !m_Interface.pickColor->isChecked() ) {
			if( m_CurrentMask ) {
				switch( m_CurrentMask->getImageProperties().majorTypeID ) {
				case isis::data::ValueArray<bool>::staticID:
					manipulateVoxel<bool>( physCoord, m_CurrentMask );
					break;
				case isis::data::ValueArray<int8_t>::staticID:
					manipulateVoxel<int8_t>( physCoord, m_CurrentMask );
					break;
				case isis::data::ValueArray<uint8_t>::staticID:
					manipulateVoxel<uint8_t>( physCoord, m_CurrentMask );
					break;
				case isis::data::ValueArray<int16_t>::staticID:
					manipulateVoxel<int16_t>( physCoord, m_CurrentMask );
					break;
				case isis::data::ValueArray<uint16_t>::staticID:
					manipulateVoxel<uint16_t>( physCoord, m_CurrentMask );
					break;
				case isis::data::ValueArray<int32_t>::staticID:
					manipulateVoxel<int32_t>( physCoord, m_CurrentMask );
					break;
				case isis::data::ValueArray<uint32_t>::staticID:
					manipulateVoxel<uint32_t>( physCoord, m_CurrentMask );
					break;
				case isis::data::ValueArray<int64_t>::staticID:
					manipulateVoxel<int64_t>( physCoord, m_CurrentMask );
					break;
				case isis::data::ValueArray<uint64_t>::staticID:
					manipulateVoxel<uint64_t>( physCoord, m_CurrentMask );
					break;
				case isis::data::ValueArray<double>::staticID:
					manipulateVoxel<double>( physCoord, m_CurrentMask );
					break;
				case isis::data::ValueArray<float>::staticID:
					manipulateVoxel<float>( physCoord, m_CurrentMask );
					break;
				default:
					LOG( Runtime, error ) << "Unknown type ID " << m_CurrentMask->getImageProperties().majorTypeID << " when trying to paint mask";
					break;
				}

				m_ViewerCore->emitImageContentChanged( m_CurrentMask );
				m_ViewerCore->updateScene();
			}
		} else {
			m_Interface.colorEdit->setValue(  m_CurrentMask->getImageProperties().voxelValue );
		}
	}

}

void MaskEditDialog::editCurrentImage()
{
	if( m_ViewerCore->hasImage() ) {
		m_Interface.pickColor->setEnabled( true );
		m_CurrentMask = m_ViewerCore->getCurrentImage();
		m_Interface.radius->setEnabled( true );
		BOOST_FOREACH( WidgetEnsemble::Vector::reference ensemble, m_ViewerCore->getUICore()->getEnsembleList() ) {
			if( ensemble->isCurrent() ) {
				m_CurrentWidgetEnsemble = ensemble;
				BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, *ensemble ) {
					ensembleComponent->getWidgetInterface()->setMouseCursorIcon( QIcon( ":/common/paintCrosshair.png" ) );
					ensembleComponent->getWidgetInterface()->setEnableCrosshair( false );
				}
			}
		}
		m_Interface.colorEdit->setValue( m_CurrentMask->getImageProperties().minMax.second->as<double>() );
		m_ViewerCore->updateScene();
	}
}


void MaskEditDialog::createEmptyMask()
{
	if( m_ViewerCore->hasImage() ) {
		m_CreateMaskDialog->show();
		m_Interface.pickColor->setEnabled( true );
		m_ViewerCore->getUICore()->refreshUI();
	} else {
		QMessageBox msgBox;
		msgBox.setIcon( QMessageBox::Critical );
		msgBox.setText( "No image loaded. Can not create any image." );
		msgBox.exec();
	}

}

void MaskEditDialog::closeEvent( QCloseEvent * )
{
	disconnect( m_ViewerCore, SIGNAL ( emitOnWidgetMoved( util::fvector3, Qt::MouseButton ) ), this, SLOT( physicalCoordChanged( util::fvector3, Qt::MouseButton ) ) );
	disconnect( m_ViewerCore, SIGNAL ( emitOnWidgetClicked( util::fvector3, Qt::MouseButton ) ), this, SLOT( physicalCoordChanged( util::fvector3, Qt::MouseButton ) ) );
	BOOST_FOREACH( WidgetEnsemble::Vector::reference ensemble, m_ViewerCore->getUICore()->getEnsembleList() ) {
		BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, *ensemble ) {
			ensembleComponent->getWidgetInterface()->setMouseCursorIcon( QIcon() );
			ensembleComponent->getWidgetInterface()->setEnableCrosshair( true );
		}
	}

}


}
}
}