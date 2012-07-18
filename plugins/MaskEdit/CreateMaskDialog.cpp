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
 * CreateMaskDialog.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "CreateMaskDialog.hpp"


namespace isis
{
namespace viewer
{
namespace plugin
{

CreateMaskDialog::CreateMaskDialog( QWidget *parent, MaskEditDialog *maskEditDialog )
	: QDialog( parent ),
	  m_MaskEditDialog( maskEditDialog )
{
	m_Interface.setupUi( this );
	connect( m_Interface.createButton, SIGNAL( clicked() ), this, SLOT( createMask() ) );
	m_Interface.maskName->setText( "mask1" );
	m_Interface.xRes->setMinimum( 0.01 );
	m_Interface.yRes->setMinimum( 0.01 );
	m_Interface.zRes->setMinimum( 0.01 );
	m_Interface.xRes->setMaximum( 10 );
	m_Interface.yRes->setMaximum( 10 );
	m_Interface.zRes->setMaximum( 10 );
}

void CreateMaskDialog::showEvent( QShowEvent * )
{
	if( m_MaskEditDialog->m_ViewerCore->hasImage() ) {
		m_Interface.maskType->clear();
		BOOST_FOREACH( std::list<std::string>::const_reference type, isis::viewer::getSupportedTypeList() ) {
			m_Interface.maskType->addItem( type.c_str() );
		}

		if( m_MaskEditDialog->m_ViewerCore->hasImage() ) {
			const util::fvector3 &voxelSize = m_MaskEditDialog->m_ViewerCore->getCurrentImage()->getISISImage()->getPropertyAs<util::fvector3>( "voxelSize" ) ;
			m_Interface.xRes->setValue( voxelSize[0] );
			m_Interface.yRes->setValue( voxelSize[1] );
			m_Interface.zRes->setValue( voxelSize[2] );
		} else {
			m_Interface.xRes->setValue( 3 );
			m_Interface.yRes->setValue( 3 );
			m_Interface.zRes->setValue( 3 );
		}
	}
}


void CreateMaskDialog::createMask()
{
	if( m_MaskEditDialog->m_ViewerCore->hasImage() ) {
		boost::shared_ptr<ImageHolder> refImage = m_MaskEditDialog->m_ViewerCore->getCurrentImage();
		std::string dataType = m_Interface.maskType->currentText().toStdString();
		dataType.append( "*" );
		boost::shared_ptr<ImageHolder> maskImage;

		switch ( isis::util::getTransposedTypeMap( false, true ).at( dataType ) ) {
		case isis::data::ValueArray<bool>::staticID:
			maskImage = _createEmptyMask<bool>( refImage );
			break;
		case isis::data::ValueArray<int8_t>::staticID:
			maskImage = _createEmptyMask<int8_t>( refImage );
			break;
		case isis::data::ValueArray<uint8_t>::staticID:
			maskImage = _createEmptyMask<uint8_t>( refImage );
			break;
		case isis::data::ValueArray<int16_t>::staticID:
			maskImage = _createEmptyMask<int16_t>( refImage );
			break;
		case isis::data::ValueArray<uint16_t>::staticID:
			maskImage = _createEmptyMask<uint16_t>( refImage );
			break;
		case isis::data::ValueArray<int32_t>::staticID:
			maskImage = _createEmptyMask<int32_t>( refImage );
			break;
		case isis::data::ValueArray<uint32_t>::staticID:
			maskImage = _createEmptyMask<uint32_t>( refImage );
			break;
		case isis::data::ValueArray<int64_t>::staticID:
			maskImage = _createEmptyMask<int64_t>( refImage );
			break;
		case isis::data::ValueArray<uint64_t>::staticID:
			maskImage = _createEmptyMask<uint64_t>( refImage );
			break;
		case isis::data::ValueArray<float>::staticID:
			maskImage = _createEmptyMask<float>( refImage );
			break;
		case isis::data::ValueArray<double>::staticID:
			maskImage = _createEmptyMask<double>( refImage );
			break;
		default:
			LOG( Runtime, error )  << "Unknown type " << dataType << " !";
			return;
		}

		m_MaskEditDialog->m_CurrentMask = maskImage;
		m_MaskEditDialog->m_CurrentMask->getImageProperties().extent = m_MaskEditDialog->m_CurrentMask->getImageProperties().minMax.second->as<double>() -  m_MaskEditDialog->m_CurrentMask->getImageProperties().minMax.first->as<double>();
		m_MaskEditDialog->m_CurrentMask->getImageProperties().opacity = 0.5;
		m_MaskEditDialog->m_CurrentMask->getImageProperties().lut = "maskeditLUT";
		m_MaskEditDialog->m_CurrentMask->updateColorMap();
		m_MaskEditDialog->m_CurrentMask->updateOrientation();
		BOOST_FOREACH( WidgetEnsemble::Vector::reference ensemble, m_MaskEditDialog->m_ViewerCore->getUICore()->getEnsembleList() ) {
			ImageHolder::Vector iList = ensemble->getImageVector();

			if( std::find( iList.begin(), iList.end(), refImage ) != iList.end() ) {
				m_MaskEditDialog->m_CurrentWidgetEnsemble = ensemble;
				ensemble->addImage( m_MaskEditDialog->m_CurrentMask );
				BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, *ensemble ) {
					ensembleComponent->getWidgetInterface()->setMouseCursorIcon( QIcon( ":/common/paintCrosshair.png" ) );
				}
			}
		}

		m_MaskEditDialog->m_ViewerCore->setCurrentImage( m_MaskEditDialog->m_CurrentMask );
		m_MaskEditDialog->m_ViewerCore->setShowCrosshair( false );
		m_MaskEditDialog->m_ViewerCore->updateScene();
		m_MaskEditDialog->m_ViewerCore->getUICore()->refreshUI();
		m_MaskEditDialog->m_Interface.radius->setEnabled( true );
		m_MaskEditDialog->m_Interface.colorEdit->setValue( m_MaskEditDialog->m_CurrentMask->getImageProperties().minMax.second->as<double>()  );
	}

	close();
}



}
}
}