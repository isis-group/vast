#include "voxelInformationWidget.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{

VoxelInformationWidget::VoxelInformationWidget( QWidget *parent, QViewerCore *core )
	: QWidget( parent ),
	  m_Core( core )
{
	m_Interface.setupUi( this );
	m_Interface.spinBox->setMinimum(0);
	m_Interface.spinBox_2->setMinimum(0);
	m_Interface.spinBox_3->setMinimum(0);
	connect( m_Core, SIGNAL( emitVoxelCoordChanged(util::ivector4)), this, SLOT(synchronizePos(util::ivector4)));
}

void VoxelInformationWidget::synchronize()
{
	QSize size = m_Interface.colromapLabel->size();
	std::string lutName = m_Core->getCurrentImage()->getPropMap().getPropertyAs<std::string>( "lut" );
	m_Interface.colromapLabel->setPixmap(
		m_Core->getColorHandler()->getIcon( lutName, size.width(), size.height() - 10 ).pixmap( size.width(), size.height() - 10 ) );
	m_Interface.labelMin->setText( m_Core->getCurrentImage()->getMinMax().first->as<std::string>().c_str() );
	m_Interface.labelMax->setText( m_Core->getCurrentImage()->getMinMax().second->as<std::string>().c_str() );
	synchronizePos( m_Core->getCurrentImage()->getPropMap().getPropertyAs<util::ivector4>("voxelCoords"));
	m_Interface.spinBox->setMaximum( m_Core->getCurrentImage()->getImageSize()[0] - 1 );
	m_Interface.spinBox_2->setMaximum( m_Core->getCurrentImage()->getImageSize()[1] - 1 );
	m_Interface.spinBox_3->setMaximum( m_Core->getCurrentImage()->getImageSize()[2] - 1 );
	util::fvector4 physicalBegin = m_Core->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( util::ivector4() );
	util::fvector4 physicalEnd = m_Core->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( m_Core->getCurrentImage()->getImageSize() );
	m_Interface.doubleSpinBox->setMinimum( physicalBegin[0] );
	m_Interface.doubleSpinBox->setMaximum( physicalEnd[0] );
	m_Interface.doubleSpinBox_2->setMinimum( physicalBegin[1] );
	m_Interface.doubleSpinBox_2->setMaximum( physicalEnd[1] );
	m_Interface.doubleSpinBox_3->setMinimum( physicalBegin[2] );
	m_Interface.doubleSpinBox_3->setMaximum( physicalEnd[2] );
	
}


void VoxelInformationWidget::synchronizePos( util::ivector4 voxelCoords )
{
	m_Interface.intensityValue->setToolTip( m_Core->getCurrentImage()->getISISImage()->getChunk( voxelCoords[0], voxelCoords[1], voxelCoords[2], voxelCoords[3], false ).getTypeName().c_str() );
	switch( m_Core->getCurrentImage()->getISISImage()->getChunk( voxelCoords[0], voxelCoords[1], voxelCoords[2], voxelCoords[3], false ).getTypeID() ) {
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
	}
	m_Interface.spinBox->setValue( voxelCoords[0] );
	m_Interface.spinBox_2->setValue( voxelCoords[1] );
	m_Interface.spinBox_3->setValue( voxelCoords[2] );
	util::fvector4 physCoords = m_Core->getCurrentImage()->getPropMap().getPropertyAs<util::fvector4>("physicalCoords");
	m_Interface.doubleSpinBox->setValue( physCoords[0] );
	m_Interface.doubleSpinBox_2->setValue( physCoords[1] );
	m_Interface.doubleSpinBox_3->setValue( physCoords[2] );
		   
}



}
}
}