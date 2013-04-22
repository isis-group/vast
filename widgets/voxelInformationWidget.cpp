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
#include "../viewer/imageholder.hpp"
#include "../viewer/qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace ui
{

VoxelInformationWidget::VoxelInformationWidget( QWidget *parent, QViewerCore *core )
	: QWidget( parent ),
	  m_ViewerCore( core ),
	  isConnected( false ),
	  m_UpperHalfColormapLabel( new QLabel( this ) ),
	  m_sepWidget( new QWidget( this ) ),
	  m_LowerHalfColormapLabel( new QLabel( this ) ),
	  m_LabelMin( new QLabel ( this ) ),
	  m_LabelMax( new QLabel( this ) ),
	  m_LowerThreshold( new QLabel( this ) ),
	  m_UpperThreshold( new QLabel( this ) )
{
	m_Interface.setupUi( this );
	m_LayoutLeft = new QVBoxLayout( m_Interface.leftFrame );
	m_LayoutRight = new QVBoxLayout( m_Interface.rightFrame );
	m_LayoutLeft->setContentsMargins( 2, 0, 0, 0 );
	m_LayoutRight->setContentsMargins( 2, 0, 0, 0 );
	m_LayoutLeft->setAlignment( Qt::AlignVCenter );
	m_LayoutRight->setAlignment( Qt::AlignVCenter );
	m_LowerThreshold->setAlignment( Qt::AlignRight );
	m_UpperThreshold->setAlignment( Qt::AlignRight );
	m_LabelMax->setAlignment( Qt::AlignLeft );

	m_Interface.frame_4->setMaximumHeight( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "maxOptionWidgetHeight" ) );
	m_Interface.frame_4->setMinimumHeight( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "minOptionWidgetHeight" ) );
	m_Interface.columnSpin->setMinimum( 0 );
	m_Interface.rowSpin->setMinimum( 0 );
	m_Interface.sliceBox->setMinimum( 0 );
	m_UpperHalfColormapLabel->setMaximumHeight( 20 );
	m_Interface.playButton->setIcon( QIcon( ":/common/play.png" ) );
	m_tThread = new TimePlayThread( this, m_ViewerCore, &m_Interface );
	QVBoxLayout *layout = new QVBoxLayout( );
	layout->setContentsMargins( 5, 0, 5, 0 );
	layout->addWidget( m_UpperHalfColormapLabel );
	layout->addWidget( m_sepWidget );
	layout->addWidget( m_LowerHalfColormapLabel );
	m_Interface.colormapButton->setLayout( layout );
	m_Interface.colormapButton->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
	m_Interface.colormapButton->setMinimumHeight( 10 );

}

void VoxelInformationWidget::disconnectSignals()
{
	disconnect( m_Interface.columnSpin, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	disconnect( m_Interface.rowSpin, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	disconnect( m_Interface.sliceBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	disconnect( m_Interface.xBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	disconnect( m_Interface.yBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	disconnect( m_Interface.zBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );

}

void VoxelInformationWidget::reconnectSignals()
{
	connect( m_Interface.columnSpin, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.rowSpin, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.sliceBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.xBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	connect( m_Interface.yBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	connect( m_Interface.zBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
}


void VoxelInformationWidget::connectSignals()
{
	connect( m_ViewerCore, SIGNAL( emitVoxelCoordChanged( util::ivector4 ) ), this, SLOT( synchronizePos( util::ivector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector3 ) ), this, SLOT( synchronizePos( util::fvector3 ) ) );
	connect( m_ViewerCore, SIGNAL( emitUpdateScene() ), this, SLOT( updateLowerUpperThreshold() ) );
	connect( m_Interface.columnSpin, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.rowSpin, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.sliceBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.xBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	connect( m_Interface.yBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	connect( m_Interface.zBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged() ) );
	connect( m_Interface.timestepSlider, SIGNAL( sliderMoved( int ) ), this, SLOT( timeStepChanged(int) ) );
	connect( m_Interface.timestepSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( timeStepChanged(int) ) );
	connect( m_Interface.timestepSlider, SIGNAL( sliderMoved( int ) ), this, SLOT( timeStepChanged(int) ) );
	connect( m_Interface.timestepSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( timeStepChanged(int) ) );
	connect( m_tThread, SIGNAL( finished() ), this, SLOT( timePlayFinished() ) );
	connect( m_Interface.playButton, SIGNAL( clicked() ), this, SLOT( playTimecourse() ) );
	connect( m_Interface.colormapButton, SIGNAL( clicked() ), this, SLOT( onLUTMenuClicked() ) );
	isConnected = true;
}

void VoxelInformationWidget::timeStepChanged ( int ts )
{
	disconnect( m_Interface.timestepSlider, SIGNAL( sliderMoved( int ) ), this, SLOT( timeStepChanged(int) ) );
	disconnect( m_Interface.timestepSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( timeStepChanged(int) ) );
	disconnect( m_Interface.timestepSlider, SIGNAL( sliderMoved( int ) ), this, SLOT( timeStepChanged(int) ) );
	disconnect( m_Interface.timestepSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( timeStepChanged(int) ) );
	m_ViewerCore->timestepChanged(ts);
	m_Interface.timestepSlider->setValue(ts);
	m_Interface.timestepSpinBox->setValue(ts);
	connect( m_Interface.timestepSlider, SIGNAL( sliderMoved( int ) ), this, SLOT( timeStepChanged(int) ) );
	connect( m_Interface.timestepSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( timeStepChanged(int) ) );
	connect( m_Interface.timestepSlider, SIGNAL( sliderMoved( int ) ), this, SLOT( timeStepChanged(int) ) );
	connect( m_Interface.timestepSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( timeStepChanged(int) ) );

}


void VoxelInformationWidget::updateLowerUpperThreshold()
{
	if( m_ViewerCore->hasImage() ) {
		m_LowerThreshold->setText( QString::number( m_ViewerCore->getCurrentImage()->getImageProperties().lowerThreshold, 'g', 4 ) );
		m_UpperThreshold->setText( QString::number( m_ViewerCore->getCurrentImage()->getImageProperties().upperThreshold, 'g', 4 ) );
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
	util::fvector3 physicalCoord ( m_Interface.xBox->text().toFloat(),
								   m_Interface.yBox->text().toFloat(),
								   m_Interface.zBox->text().toFloat() );
	m_ViewerCore->physicalCoordsChanged( physicalCoord );
}

void VoxelInformationWidget::voxPosChanged()
{
	util::ivector4 voxelCoords( m_Interface.columnSpin->text().toInt(),
								m_Interface.rowSpin->text().toInt(),
								m_Interface.sliceBox->text().toInt(),
								m_ViewerCore->getCurrentImage()->getImageProperties().timestep );
	m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( voxelCoords ) ) ;

}



void VoxelInformationWidget::synchronize()
{
	if( m_ViewerCore->hasImage() ) {
		setVisible( true );
		const boost::shared_ptr<ImageHolder> image = m_ViewerCore->getCurrentImage();
		disconnectSignals();
		m_Interface.colormapFrame->setVisible( !image->getImageProperties().isRGB );

		if( image->getImageProperties().imageType == ImageHolder::structural_image ) {
			m_Interface.frame_6->setMinimumHeight( 15 );
			m_sepWidget->setVisible( false );
			m_LowerThreshold->setVisible( false );
			m_UpperThreshold->setVisible( false );
			m_LayoutLeft->addWidget( m_LabelMin );
			m_LayoutRight->addWidget( m_LabelMax );
			m_LabelMax->setVisible( true );
			m_LabelMin->setVisible( true );
			m_LabelMin->setAlignment( Qt::AlignRight );
			m_LowerHalfColormapLabel->setVisible( false );
		} else if ( image->getImageProperties().imageType == ImageHolder::statistical_image ) {
			m_Interface.frame_6->setMinimumHeight( 40 );
			m_sepWidget->setVisible( true );
			m_LowerThreshold->setText( QString::number( image->getImageProperties().lowerThreshold, 'g', 4 ) );
			m_UpperThreshold->setText( QString::number( image->getImageProperties().upperThreshold, 'g', 4 ) );
			m_UpperThreshold->setVisible( true );
			m_LabelMax->setVisible( true );

			if( image->getImageProperties().minMax.first->as<double>() < 0 && image->getImageProperties().minMax.second->as<double>() > 0 ) {
				m_sepWidget->setVisible( true );
			} else {
				m_sepWidget->setVisible( false );
			}

			if( image->getImageProperties().minMax.first->as<double>() >= 0 ) {
				m_LowerHalfColormapLabel->setVisible( false );
				m_LowerThreshold->setVisible( false );
				m_LabelMin->setVisible( false );
				m_LayoutLeft->addWidget( m_UpperThreshold );
				m_LayoutRight->addWidget( m_LabelMax );
			} else {
				m_LowerHalfColormapLabel->setVisible( true );
				m_LowerThreshold->setVisible( true );
				m_LabelMin->setVisible( true );
				m_LayoutLeft->addWidget( m_UpperThreshold );
				m_LayoutLeft->addWidget( m_LowerThreshold );
				m_LayoutRight->addWidget( m_LabelMax );
				m_LayoutRight->addWidget( m_LabelMin );
				m_LabelMin->setAlignment( Qt::AlignLeft );
			}

			if( image->getImageProperties().minMax.second->as<double>() <= 0 ) {
				m_UpperHalfColormapLabel->setVisible( false );
				m_UpperThreshold->setVisible( false );
				m_LabelMax->setVisible( false );
				m_LabelMin->setVisible( true );
				m_LowerThreshold->setVisible( true );
				m_LowerHalfColormapLabel->setVisible( true );
				m_LayoutRight->addWidget( m_LabelMin );
				m_LayoutLeft->addWidget( m_LowerThreshold );
			} else {
				m_UpperHalfColormapLabel->setVisible( true );

			}
		}

		if( !image->getImageProperties().isRGB ) {
			m_LabelMin->setText( QString::number( image->getImageProperties().scalingMinMax.first, 'g', 4 ) );
			m_LabelMax->setText( QString::number( image->getImageProperties().scalingMinMax.second, 'g', 4 ) );
		}

		const util::ivector4 outerCorner = util::ivector4( image->getImageSize()[0] - 1, image->getImageSize()[1] - 1, image->getImageSize()[2] - 1 );

		m_Interface.columnSpin->setMaximum( outerCorner[0] );

		m_Interface.rowSpin->setMaximum( outerCorner[1] );

		m_Interface.sliceBox->setMaximum( outerCorner[2] );

		m_Interface.xBox->setMinimum( -1000 );

		m_Interface.xBox->setMaximum( 1000 );

		m_Interface.yBox->setMinimum( -1000 );

		m_Interface.yBox->setMaximum( 1000 );

		m_Interface.zBox->setMinimum( -1000 );

		m_Interface.zBox->setMaximum( 1000 );

		synchronizePos( image->getImageProperties().voxelCoords );

		const util::fvector3 transformedVec = image->getImageProperties().latchedOrientation.dot( image->getImageProperties().voxelSize );

		m_Interface.xBox->setSingleStep( fabs( transformedVec[0] ) );

		m_Interface.yBox->setSingleStep( fabs( transformedVec[1] ) );

		m_Interface.zBox->setSingleStep( fabs( transformedVec[2] ) );

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

		m_Interface.colormapFrame->adjustSize();

		if( image->getImageProperties().imageType == ImageHolder::structural_image ) {
			QSize size = m_Interface.colormapButton->size();
			m_UpperHalfColormapLabel->setPixmap(
				util::Singletons::get<color::Color, 10>().getIcon( image->getImageProperties().lut, size.width(), size.height() - 10 ).pixmap( size.width(), size.height() - 10 ) );
		} else if ( image->getImageProperties().imageType == ImageHolder::statistical_image ) {
			QSize size = m_Interface.colormapButton->size();
			m_UpperHalfColormapLabel->setPixmap(
				util::Singletons::get<color::Color, 10>().getIcon( image->getImageProperties().lut, size.width(), size.height() - 15, color::Color::upper_half ).pixmap( size.width(), size.height() - 15 ) );
			m_LowerHalfColormapLabel->setPixmap(
				util::Singletons::get<color::Color, 10>().getIcon( image->getImageProperties().lut, size.width(), size.height() - 15, color::Color::lower_half, true ).pixmap( size.width(), size.height() - 15 ) );
		}


		reconnectSignals();
	} else {
		setVisible( false );
	}
}

void VoxelInformationWidget::synchronizePos( util::fvector3 physicalCoords )
{
	synchronizePos( m_ViewerCore->getCurrentImage()->getISISImage()->getIndexFromPhysicalCoords( physicalCoords ) );
}

void VoxelInformationWidget::synchronizePos( util::ivector4 voxelCoords )
{
	boost::shared_ptr<ImageHolder> image = m_ViewerCore->getCurrentImage();
	image->correctVoxelCoords<3>( voxelCoords );
	const isis::data::Chunk &chunk = image->getISISImage()->getChunk( voxelCoords[0], voxelCoords[1], voxelCoords[2], voxelCoords[3], false );
	const std::string typeName = chunk.getTypeName();
	m_Interface.intensityValue->setToolTip( typeName.substr( 0, typeName.length() - 1 ).c_str() );

	switch( chunk.getTypeID() ) {
	case isis::data::ValueArray<bool>::staticID:
		displayIntensity<bool>( voxelCoords );
		break;
	case isis::data::ValueArray<int8_t>::staticID:
		displayIntensity<int8_t>( voxelCoords );
		break;
	case isis::data::ValueArray<uint8_t>::staticID:
		displayIntensity<uint8_t>( voxelCoords );
		break;
	case isis::data::ValueArray<int16_t>::staticID:
		displayIntensity<int16_t>( voxelCoords );
		break;
	case isis::data::ValueArray<uint16_t>::staticID:
		displayIntensity<uint16_t>( voxelCoords );
		break;
	case isis::data::ValueArray<int32_t>::staticID:
		displayIntensity<int32_t>( voxelCoords );
		break;
	case isis::data::ValueArray<uint32_t>::staticID:
		displayIntensity<uint32_t>( voxelCoords );
		break;
	case isis::data::ValueArray<int64_t>::staticID:
		displayIntensity<int64_t>( voxelCoords );
		break;
	case isis::data::ValueArray<uint64_t>::staticID:
		displayIntensity<uint64_t>( voxelCoords );
		break;
	case isis::data::ValueArray<float>::staticID:
		displayIntensity<float>( voxelCoords );
		break;
	case isis::data::ValueArray<double>::staticID:
		displayIntensity<double>( voxelCoords );
		break;
	case isis::data::ValueArray<util::color24>::staticID:
		displayIntensityColor<util::color24>( voxelCoords );
		break;
	case isis::data::ValueArray<util::color48>::staticID:
		displayIntensityColor<util::color48> ( voxelCoords );
		break;
	}

	m_Interface.timestepSpinBox->setValue( image->getImageProperties().timestep );
	image->getImageProperties().voxelValue = m_Interface.intensityValue->text().toDouble();
	disconnectSignals();
	m_Interface.columnSpin->setValue( voxelCoords[0] );
	m_Interface.rowSpin->setValue( voxelCoords[1] );
	m_Interface.sliceBox->setValue( voxelCoords[2] );
	const util::fvector3 physCoords = image->getISISImage()->getPhysicalCoordsFromIndex( voxelCoords );
	m_Interface.xBox->setValue( physCoords[0] );
	m_Interface.yBox->setValue( physCoords[1] );
	m_Interface.zBox->setValue( physCoords[2] );
	reconnectSignals();

}

void VoxelInformationWidget::onLUTMenuClicked()
{
	m_ViewerCore->getUICore()->getMainWindow()->preferencesDialog->getUI().tabWidget->setCurrentIndex( 0 );
	m_ViewerCore->getUICore()->getMainWindow()->preferencesDialog->show();
}



}
}
}
