#include "voxelInformationWidget.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{

VoxelInformationWidget::VoxelInformationWidget( QWidget *parent, QViewerCore *core )
	: QWidget( parent ),
	  m_ViewerCore( core ),
	  isConnected(false)
{
	m_Interface.setupUi( this );
	m_Interface.rowBox->setMinimum( 0 );
	m_Interface.columnBox->setMinimum( 0 );
	m_Interface.sliceBox->setMinimum( 0 );
}


void VoxelInformationWidget::connectSignals()
{
	connect( m_ViewerCore, SIGNAL( emitVoxelCoordChanged( util::ivector4 ) ), this, SLOT( synchronizePos( util::ivector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( synchronizePos( util::fvector4 ) ) );
	connect( m_Interface.rowBox, SIGNAL( editingFinished()), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.columnBox, SIGNAL( editingFinished()), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.sliceBox, SIGNAL( editingFinished()), this, SLOT( voxPosChanged() ) );
	connect( m_Interface.xBox, SIGNAL( editingFinished()), this, SLOT( physPosChanged() ) );
	connect( m_Interface.yBox, SIGNAL( editingFinished()), this, SLOT( physPosChanged() ) );
	connect( m_Interface.zBox, SIGNAL( editingFinished()), this, SLOT( physPosChanged() ) );
	isConnected = true;
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
								m_Interface.sliceBox->text().toInt() );
	m_ViewerCore->voxelCoordsChanged( voxelCoords ) ;

}



void VoxelInformationWidget::synchronize()
{
	const boost::shared_ptr<ImageHolder> image = m_ViewerCore->getCurrentImage();
	QSize size = m_Interface.colromapLabel->size();
	const std::string lutName = image->getPropMap().getPropertyAs<std::string>( "lut" );
	m_Interface.colromapLabel->setPixmap(
		m_ViewerCore->getColorHandler()->getIcon( lutName, size.width(), size.height() - 10 ).pixmap( size.width(), size.height() - 10 ) );
	m_Interface.labelMin->setText( image->getMinMax().first->as<std::string>().c_str() );
	m_Interface.labelMax->setText( image->getMinMax().second->as<std::string>().c_str() );
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
	synchronizePos( image->getPropMap().getPropertyAs<util::ivector4>( "voxelCoords" ) );
	const util::fvector4 voxelSpacing = image->getISISImage()->getPropertyAs<util::fvector4>( "voxelSize" ) + image->getISISImage()->getPropertyAs<util::fvector4>( "voxelGap" );
	boost::numeric::ublas::vector<float> vSpacing = boost::numeric::ublas::vector<float>( 4 );

	for( unsigned short i = 0; i < 4; i++ ) {
		vSpacing( i ) = voxelSpacing[i];
	}

	boost::numeric::ublas::vector<float> transformedVec = boost::numeric::ublas::prod( image->getNormalizedImageOrientation(), vSpacing );
	m_Interface.xBox->setSingleStep( fabs( transformedVec( 0 ) ) );
	m_Interface.yBox->setSingleStep( fabs( transformedVec( 1 ) ) );
	m_Interface.zBox->setSingleStep( fabs( transformedVec( 2 ) ) );
	if( !isConnected) {
		connectSignals();
	}
}

void VoxelInformationWidget::synchronizePos( util::fvector4 physicalCoords )
{
	synchronizePos( m_ViewerCore->getCurrentImage()->getISISImage()->getIndexFromPhysicalCoords( physicalCoords ) );
}

void VoxelInformationWidget::synchronizePos( util::ivector4 voxelCoords )
{
	const std::string typeName = m_ViewerCore->getCurrentImage()->getISISImage()->getChunk( voxelCoords[0], voxelCoords[1], voxelCoords[2], voxelCoords[3], false ).getTypeName();
	m_Interface.intensityValue->setToolTip( typeName.substr( 0, typeName.length() - 1 ).c_str() );

	switch( m_ViewerCore->getCurrentImage()->getISISImage()->getChunk( voxelCoords[0], voxelCoords[1], voxelCoords[2], voxelCoords[3], false ).getTypeID() ) {
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

	m_Interface.rowBox->setValue( voxelCoords[0] );
	m_Interface.columnBox->setValue( voxelCoords[1] );
	m_Interface.sliceBox->setValue( voxelCoords[2] );
	const util::fvector4 physCoords = m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( voxelCoords );
	m_Interface.xBox->setValue( physCoords[0] );
	m_Interface.yBox->setValue( physCoords[1] );
	m_Interface.zBox->setValue( physCoords[2] );

}



}
}
}