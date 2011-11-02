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
	m_Interface.spinBox->setMinimum( 0 );
	m_Interface.spinBox_2->setMinimum( 0 );
	m_Interface.spinBox_3->setMinimum( 0 );
	connect( m_Core, SIGNAL( emitVoxelCoordChanged( util::ivector4 ) ), this, SLOT( synchronizePos( util::ivector4 ) ) );
	connect( m_Core, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( synchronizePos( util::fvector4 ) ) );
	connect( m_Interface.spinBox, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged( int ) ) );
	connect( m_Interface.spinBox_2, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged( int ) ) );
	connect( m_Interface.spinBox_3, SIGNAL( valueChanged( int ) ), this, SLOT( voxPosChanged( int ) ) );
	connect( m_Interface.doubleSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged( double ) ) );
	connect( m_Interface.doubleSpinBox_2, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged( double ) ) );
	connect( m_Interface.doubleSpinBox_3, SIGNAL( valueChanged( double ) ), this, SLOT( physPosChanged( double ) ) );
}

void VoxelInformationWidget::physPosChanged( double dummy )
{
	util::fvector4 physicalCoord ( m_Interface.doubleSpinBox->text().toFloat(),
								   m_Interface.doubleSpinBox_2->text().toFloat(),
								   m_Interface.doubleSpinBox_3->text().toFloat() );
	m_Core->physicalCoordsChanged( physicalCoord );
}

void VoxelInformationWidget::voxPosChanged( int dummy )
{

}


void VoxelInformationWidget::synchronize()
{
	const boost::shared_ptr<ImageHolder> image = m_Core->getCurrentImage();
	QSize size = m_Interface.colromapLabel->size();
	const std::string lutName = image->getPropMap().getPropertyAs<std::string>( "lut" );
	m_Interface.colromapLabel->setPixmap(
		m_Core->getColorHandler()->getIcon( lutName, size.width(), size.height() - 10 ).pixmap( size.width(), size.height() - 10 ) );
	m_Interface.labelMin->setText( image->getMinMax().first->as<std::string>().c_str() );
	m_Interface.labelMax->setText( image->getMinMax().second->as<std::string>().c_str() );
	synchronizePos( image->getPropMap().getPropertyAs<util::ivector4>( "voxelCoords" ) );
	const util::ivector4 outerCorner = util::ivector4( image->getImageSize()[0] - 1, image->getImageSize()[1] - 1, image->getImageSize()[2] - 1 );
	m_Interface.spinBox->setMaximum( outerCorner[0] );
	m_Interface.spinBox_2->setMaximum( outerCorner[1] );
	m_Interface.spinBox_3->setMaximum( outerCorner[2] );
	const util::fvector4 physicalBegin = image->getISISImage()->getPhysicalCoordsFromIndex( util::ivector4() );
	const util::fvector4 physicalEnd = image->getISISImage()->getPhysicalCoordsFromIndex( outerCorner );
	m_Interface.doubleSpinBox->setMinimum( physicalBegin[0] < physicalEnd[0] ? physicalBegin[0] : physicalEnd[0] );
	m_Interface.doubleSpinBox->setMaximum( physicalBegin[0] > physicalEnd[0] ? physicalBegin[0] : physicalEnd[0] );
	m_Interface.doubleSpinBox_2->setMinimum( physicalBegin[1] < physicalEnd[1] ? physicalBegin[1] : physicalEnd[1] );
	m_Interface.doubleSpinBox_2->setMaximum( physicalBegin[1] > physicalEnd[1] ? physicalBegin[1] : physicalEnd[1] );
	m_Interface.doubleSpinBox_3->setMinimum( physicalBegin[2] < physicalEnd[2] ? physicalBegin[2] : physicalEnd[2] );
	m_Interface.doubleSpinBox_3->setMaximum( physicalBegin[2] > physicalEnd[2] ? physicalBegin[2] : physicalEnd[2] );
	const util::fvector4 voxelSpacing = image->getISISImage()->getPropertyAs<util::fvector4>( "voxelSize" ) + image->getISISImage()->getPropertyAs<util::fvector4>( "voxelGap" );
	boost::numeric::ublas::vector<float> vSpacing = boost::numeric::ublas::vector<float>( 4 );

	for( unsigned short i = 0; i < 4; i++ ) {
		vSpacing( i ) = voxelSpacing[i];
	}

	boost::numeric::ublas::vector<float> transformedVec = boost::numeric::ublas::prod( image->getNormalizedImageOrientation(), vSpacing );
	m_Interface.doubleSpinBox->setSingleStep( fabs( transformedVec( 0 ) ) );
	m_Interface.doubleSpinBox_2->setSingleStep( fabs( transformedVec( 1 ) ) );
	m_Interface.doubleSpinBox_3->setSingleStep( fabs( transformedVec( 2 ) ) );
}

void VoxelInformationWidget::synchronizePos( util::fvector4 physicalCoords )
{
	synchronizePos( m_Core->getCurrentImage()->getISISImage()->getIndexFromPhysicalCoords( physicalCoords ) );
}

void VoxelInformationWidget::synchronizePos( util::ivector4 voxelCoords )
{
	const std::string typeName = m_Core->getCurrentImage()->getISISImage()->getChunk( voxelCoords[0], voxelCoords[1], voxelCoords[2], voxelCoords[3], false ).getTypeName();
	m_Interface.intensityValue->setToolTip( typeName.substr( 0, typeName.length() - 1 ).c_str() );

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
	const util::fvector4 physCoords = m_Core->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( voxelCoords );
	m_Interface.doubleSpinBox->setValue( physCoords[0] );
	m_Interface.doubleSpinBox_2->setValue( physCoords[1] );
	m_Interface.doubleSpinBox_3->setValue( physCoords[2] );

}



}
}
}