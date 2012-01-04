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
 * voxelInformationWidget.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "voxelInformationWidget.hpp"
#include <imageholder.hpp>
#include <qviewercore.hpp>

namespace isis
{
namespace viewer
{
namespace widget
{

VoxelInformationWidget::VoxelInformationWidget( QWidget *parent, QViewerCore *core )
	: QWidget( parent ),
	  m_ViewerCore( core ),
	  isConnected( false )
{
	m_Interface.setupUi( this );
	m_Interface.frame_4->setMaximumHeight( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "maxOptionWidgetHeight" ) );
	m_Interface.frame_4->setMinimumHeight( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "minOptionWidgetHeight" ) );
	m_Interface.rowBox->setMinimum( 0 );
	m_Interface.columnBox->setMinimum( 0 );
	m_Interface.sliceBox->setMinimum( 0 );
	m_Interface.upperHalfColormapLabel->setMaximumHeight( 20 );
	m_Interface.playButton->setIcon( QIcon( ":/common/play.png" ) );
	m_tThread = new TimePlayThread( m_ViewerCore, &m_Interface );

}

void VoxelInformationWidget::disconnectSignals()
{
	disconnect( m_Interface.rowBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	disconnect( m_Interface.columnBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	disconnect( m_Interface.sliceBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	disconnect( m_Interface.xBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	disconnect( m_Interface.yBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	disconnect( m_Interface.zBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );

}

void VoxelInformationWidget::reconnectSignals()
{
	connect( m_Interface.rowBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.columnBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.sliceBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.xBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	connect( m_Interface.yBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	connect( m_Interface.zBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
}


void VoxelInformationWidget::connectSignals()
{
	connect( m_ViewerCore, SIGNAL( emitVoxelCoordChanged( util::ivector4 ) ), this, SLOT( synchronizePos( util::ivector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( synchronizePos( util::fvector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitUpdateScene() ), this, SLOT( updateLowerUpperThreshold() ) );
	connect( m_Interface.rowBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.columnBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.sliceBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.xBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	connect( m_Interface.yBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	connect( m_Interface.zBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	connect( m_Interface.timestepSlider, SIGNAL( sliderMoved( int ) ), m_ViewerCore, SLOT( timestepChanged( int ) ) );
	connect( m_Interface.timestepSpinBox, SIGNAL( valueChanged( int ) ), m_ViewerCore, SLOT( timestepChanged( int ) ) );
	connect( m_Interface.timestepSlider, SIGNAL( sliderMoved( int ) ), m_Interface.timestepSpinBox, SLOT( setValue( int ) ) );
	connect( m_Interface.timestepSpinBox, SIGNAL( valueChanged( int ) ), m_Interface.timestepSlider, SLOT( setValue( int ) ) );
	connect( m_tThread, SIGNAL( finished() ), this, SLOT( timePlayFinished() ) );
	connect( m_Interface.playButton, SIGNAL( clicked() ), this, SLOT( playTimecourse() ) );
	isConnected = true;
}

void VoxelInformationWidget::updateLowerUpperThreshold()
{
	if( m_ViewerCore->hasImage() ) {
		m_Interface.lowerThresholdLabel->setText( QString::number( m_ViewerCore->getCurrentImage()->lowerThreshold, 'g', 4 ) );
		m_Interface.upperThresholdLabel->setText( QString::number( m_ViewerCore->getCurrentImage()->upperThreshold, 'g', 4 ) );
	}
}

void VoxelInformationWidget::playTimecourse()
{
	if( m_ViewerCore->hasImage() ) {
		if( m_tThread->isRunning() ) {
			m_tThread->terminate();
			m_Interface.playButton->setIcon( QIcon( ":/common/play.png" ) );
		} else {
			if ( QApplication::keyboardModifiers() == Qt::ControlModifier ) {
				m_tThread->setStartStop( 0, m_ViewerCore->getCurrentImage()->getImageSize()[3] );
			} else {
				m_tThread->setStartStop( m_Interface.timestepSlider->value(), m_ViewerCore->getCurrentImage()->getImageSize()[3] );
			}

			m_Interface.playButton->setIcon( QIcon( ":/common/pause.png" ) );
			m_tThread->start();
		}
	}
}

void VoxelInformationWidget::timePlayFinished()
{
	m_Interface.playButton->setIcon( QIcon( ":/common/play.png" ) );
}

void VoxelInformationWidget::physPosChanged()
{
	util::fvector4 physicalCoord ( m_Interface.xBox->text().toFloat(),
								   m_Interface.yBox->text().toFloat(),
								   m_Interface.zBox->text().toFloat() );
	m_ViewerCore->physicalCoordsChanged( physicalCoord );
}

void VoxelInformationWidget::voxPosChanged()
{
	util::ivector4 voxelCoords( m_Interface.rowBox->text().toInt(),
								m_Interface.columnBox->text().toInt(),
								m_Interface.sliceBox->text().toInt(),
								m_ViewerCore->getCurrentImage()->voxelCoords[3] );
	m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( voxelCoords ) ) ;

}



void VoxelInformationWidget::synchronize()
{
	if( m_ViewerCore->hasImage() ) {
		const boost::shared_ptr<ImageHolder> image = m_ViewerCore->getCurrentImage();
		disconnectSignals();
		m_Interface.colormapFrame->setVisible( !image->isRGB );

		if( image->imageType == ImageHolder::structural_image ) {
			m_Interface.colormapGrid->addWidget( m_Interface.labelMin, 0, 0 );
			m_Interface.colormapGrid->addWidget( m_Interface.upperHalfColormapLabel, 0, 1 );
			m_Interface.colormapGrid->addWidget( m_Interface.labelMax, 0, 2 );
			m_Interface.lowerHalfColormapLabel->setVisible( false );
			m_Interface.lowerThresholdLabel->setVisible( false );
			m_Interface.upperThresholdLabel->setVisible( false );
			m_Interface.upperHalfColormapLabel->adjustSize();
			QSize size = m_Interface.upperHalfColormapLabel->size();
			m_Interface.upperHalfColormapLabel->setPixmap(
				util::Singletons::get<color::Color, 10>().getIcon( image->lut, size.width(), size.height() - 10 ).pixmap( size.width(), size.height() - 10 ) );
		} else if ( image->imageType == ImageHolder::z_map ) {
			m_Interface.colormapGrid->addWidget( m_Interface.upperThresholdLabel, 0, 0 );
			m_Interface.colormapGrid->addWidget( m_Interface.upperHalfColormapLabel, 0, 1 );
			m_Interface.colormapGrid->addWidget( m_Interface.labelMax, 0, 2 );
			m_Interface.colormapGrid->addWidget( m_Interface.lowerThresholdLabel, 1, 0 );
			m_Interface.colormapGrid->addWidget( m_Interface.lowerHalfColormapLabel, 1, 1 );
			m_Interface.colormapGrid->addWidget( m_Interface.labelMin, 1, 2 );
			m_Interface.lowerThresholdLabel->setVisible( true );
			m_Interface.upperThresholdLabel->setVisible( true );
			m_Interface.lowerThresholdLabel->setText( QString::number( image->lowerThreshold, 'g', 4 ) );
			m_Interface.upperThresholdLabel->setText( QString::number( image->upperThreshold, 'g', 4 ) );
			m_Interface.upperHalfColormapLabel->adjustSize();
			QSize size = m_Interface.upperHalfColormapLabel->size();
			m_Interface.upperHalfColormapLabel->setPixmap(
				util::Singletons::get<color::Color, 10>().getIcon( image->lut, size.width(), size.height() - 10, color::Color::upper_half ).pixmap( size.width(), size.height() - 10 ) );
			m_Interface.lowerHalfColormapLabel->setPixmap(
				util::Singletons::get<color::Color, 10>().getIcon( image->lut, size.width(), size.height() - 10, color::Color::lower_half, true ).pixmap( size.width(), size.height() - 10 ) );

			if( image->minMax.first->as<double>() >= 0 ) {
				m_Interface.lowerHalfColormapLabel->setVisible( false );
				m_Interface.lowerThresholdLabel->setVisible( false );
				m_Interface.labelMin->setVisible( false );
			} else {
				m_Interface.lowerHalfColormapLabel->setVisible( true );
				m_Interface.lowerThresholdLabel->setVisible( true );
				m_Interface.labelMin->setVisible( true );
			}

			if( image->minMax.second->as<double>() <= 0 ) {
				m_Interface.upperHalfColormapLabel->setVisible( false );
				m_Interface.upperThresholdLabel->setVisible( false );
				m_Interface.labelMax->setVisible( false );
			} else {
				m_Interface.upperHalfColormapLabel->setVisible( true );
				m_Interface.upperThresholdLabel->setVisible( true );
				m_Interface.labelMax->setVisible( true );

			}
		}

		if( !image->isRGB ) {
			m_Interface.labelMin->setText( QString::number( image->minMax.first->as<double>(), 'g', 4 ) );
			m_Interface.labelMax->setText( QString::number( image->minMax.second->as<double>(), 'g', 4 ) );
		}

		const util::ivector4 outerCorner = util::ivector4( image->getImageSize()[0] - 1, image->getImageSize()[1] - 1, image->getImageSize()[2] - 1 );

		m_Interface.rowBox->setMaximum( outerCorner[0] );

		m_Interface.columnBox->setMaximum( outerCorner[1] );

		m_Interface.sliceBox->setMaximum( outerCorner[2] );

		const util::fvector4 physicalBegin = image->getISISImage()->getPhysicalCoordsFromIndex( util::ivector4() );

		const util::fvector4 physicalEnd = image->getISISImage()->getPhysicalCoordsFromIndex( outerCorner );

		m_Interface.xBox->setMinimum( physicalBegin[0] < physicalEnd[0] ? physicalBegin[0] : physicalEnd[0] );

		m_Interface.xBox->setMaximum( physicalBegin[0] > physicalEnd[0] ? physicalBegin[0] : physicalEnd[0] );

		m_Interface.yBox->setMinimum( physicalBegin[1] < physicalEnd[1] ? physicalBegin[1] : physicalEnd[1] );

		m_Interface.yBox->setMaximum( physicalBegin[1] > physicalEnd[1] ? physicalBegin[1] : physicalEnd[1] );

		m_Interface.zBox->setMinimum( physicalBegin[2] < physicalEnd[2] ? physicalBegin[2] : physicalEnd[2] );

		m_Interface.zBox->setMaximum( physicalBegin[2] > physicalEnd[2] ? physicalBegin[2] : physicalEnd[2] );

		synchronizePos( image->voxelCoords );

		util::fvector4 voxelSpacing;

		if( image->getISISImage()->hasProperty( "voxelGap" ) ) {
			voxelSpacing = image->getISISImage()->getPropertyAs<util::fvector4>( "voxelSize" ) + image->getISISImage()->getPropertyAs<util::fvector4>( "voxelGap" );
		} else {
			voxelSpacing = image->getISISImage()->getPropertyAs<util::fvector4>( "voxelSize" );
		}

		boost::numeric::ublas::vector<float> vSpacing = boost::numeric::ublas::vector<float>( 4 );

		for( unsigned short i = 0; i < 4; i++ ) {
			vSpacing( i ) = voxelSpacing[i];
		}

		boost::numeric::ublas::vector<float> transformedVec = boost::numeric::ublas::prod( image->getNormalizedImageOrientation(), vSpacing );
		m_Interface.xBox->setSingleStep( fabs( transformedVec( 0 ) ) );
		m_Interface.yBox->setSingleStep( fabs( transformedVec( 1 ) ) );
		m_Interface.zBox->setSingleStep( fabs( transformedVec( 2 ) ) );

		if( !isConnected ) {
			connectSignals();
		}

		if( m_ViewerCore->getCurrentImage()->getImageSize()[3] > 1 ) {
			std::stringstream tooltip;
			tooltip << "Number of timesteps: " << m_ViewerCore->getCurrentImage()->getImageSize()[3];
			m_Interface.timeStepFrame->setVisible( true );
			m_Interface.timestepSlider->setMaximum( m_ViewerCore->getCurrentImage()->getImageSize()[3] - 1 );
			m_Interface.timestepSlider->setMinimum( 0 );
			m_Interface.timestepSpinBox->setMaximum( m_ViewerCore->getCurrentImage()->getImageSize()[3] - 1 );
			m_Interface.timestepSpinBox->setMinimum( 0 );
			m_Interface.timeStepFrame->setToolTip( tooltip.str().c_str() );
			m_Interface.timestepSlider->setToolTip( tooltip.str().c_str() );
			m_Interface.timestepSpinBox->setToolTip( tooltip.str().c_str() );
		} else {
			m_Interface.timeStepFrame->setVisible( false );
		}

		reconnectSignals();
	}
}

void VoxelInformationWidget::synchronizePos( util::fvector4 physicalCoords )
{
	synchronizePos( m_ViewerCore->getCurrentImage()->getISISImage()->getIndexFromPhysicalCoords( physicalCoords, true ) );
}

void VoxelInformationWidget::synchronizePos( util::ivector4 voxelCoords )
{
	boost::shared_ptr<ImageHolder> image = m_ViewerCore->getCurrentImage();
	const std::string typeName = image->getISISImage()->getChunk( voxelCoords[0], voxelCoords[1], voxelCoords[2], voxelCoords[3], false ).getTypeName();
	m_Interface.intensityValue->setToolTip( typeName.substr( 0, typeName.length() - 1 ).c_str() );

	switch( image->getISISImage()->getChunk( voxelCoords[0], voxelCoords[1], voxelCoords[2], voxelCoords[3], false ).getTypeID() ) {
	case isis::data::ValuePtr<bool>::staticID:
		displayIntensity<bool>( voxelCoords );
		break;
	case isis::data::ValuePtr<int8_t>::staticID:
		displayIntensity<int8_t>( voxelCoords );
		break;
	case isis::data::ValuePtr<uint8_t>::staticID:
		displayIntensity<uint8_t>( voxelCoords );
		break;
	case isis::data::ValuePtr<int16_t>::staticID:
		displayIntensity<int16_t>( voxelCoords );
		break;
	case isis::data::ValuePtr<uint16_t>::staticID:
		displayIntensity<uint16_t>( voxelCoords );
		break;
	case isis::data::ValuePtr<int32_t>::staticID:
		displayIntensity<int32_t>( voxelCoords );
		break;
	case isis::data::ValuePtr<uint32_t>::staticID:
		displayIntensity<uint32_t>( voxelCoords );
		break;
	case isis::data::ValuePtr<int64_t>::staticID:
		displayIntensity<int64_t>( voxelCoords );
		break;
	case isis::data::ValuePtr<uint64_t>::staticID:
		displayIntensity<uint64_t>( voxelCoords );
		break;
	case isis::data::ValuePtr<float>::staticID:
		displayIntensity<float>( voxelCoords );
		break;
	case isis::data::ValuePtr<double>::staticID:
		displayIntensity<double>( voxelCoords );
		break;
	case isis::data::ValuePtr<util::color24>::staticID:
		displayIntensityColor<util::color24>( voxelCoords );
		break;
	case isis::data::ValuePtr<util::color48>::staticID:
		displayIntensityColor<util::color48> ( voxelCoords );
		break;
	}

	disconnectSignals();
	m_Interface.rowBox->setValue( voxelCoords[0] );
	m_Interface.columnBox->setValue( voxelCoords[1] );
	m_Interface.sliceBox->setValue( voxelCoords[2] );
	const util::fvector4 physCoords = image->getISISImage()->getPhysicalCoordsFromIndex( voxelCoords );
	m_Interface.xBox->setValue( physCoords[0] );
	m_Interface.yBox->setValue( physCoords[1] );
	m_Interface.zBox->setValue( physCoords[2] );
	reconnectSignals();

}



}
}
}