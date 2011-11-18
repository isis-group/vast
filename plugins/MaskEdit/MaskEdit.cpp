
#include "MaskEdit.hpp"
#include <DataStorage/image.hpp>
#include "uicore.hpp"
#include "common.hpp"
#include <CoreUtils/vector.hpp>


namespace isis {
namespace viewer {
namespace plugin {

MaskEditDialog::MaskEditDialog(QWidget* parent, QViewerCore* core)
	: QDialog(parent),
	m_ViewerCore(core),
	m_Radius(2)
{
	m_Interface.setupUi( this );
	m_Interface.cut->setEnabled(false);
	m_Interface.paint->setEnabled(false);
	m_Interface.radius->setEnabled(false);
	m_Interface.radius->setMaximum(500);
	m_Interface.radius->setValue(m_Radius);
	m_Interface.maskName->setText("mask1");
	m_Interface.xRes->setMinimum(0.01);
	m_Interface.yRes->setMinimum(0.01);
	m_Interface.zRes->setMinimum(0.01);
	m_Interface.xRes->setMaximum(10);
	m_Interface.yRes->setMaximum(10);
	m_Interface.zRes->setMaximum(10);
	m_ViewerCore->getColorHandler()->addColormap( ":/common/maskeditLUT" );
	connect( m_Interface.newMask, SIGNAL( clicked()), this, SLOT( createEmptyMask())) ;
	connect( m_Interface.radius, SIGNAL( valueChanged(int)), SLOT( radiusChange(int)));
	connect( m_Interface.cut, SIGNAL( clicked(bool)), this , SLOT( cutClicked()));
	connect( m_Interface.paint, SIGNAL( clicked(bool)), this , SLOT( paintClicked()));

}

void MaskEditDialog::cutClicked()
{
	for( unsigned short i = 0; i < 3; i++ ) {
		m_CurrentWidgetEnsemble[i].widgetImplementation->setMouseCursorIcon( QIcon( ":/common/cutCrosshair.png" ) );
	}
	m_ViewerCore->setShowCrosshair(false);
	m_ViewerCore->updateScene();
}

void MaskEditDialog::paintClicked()
{
	for( unsigned short i = 0; i < 3; i++ ) {
		m_CurrentWidgetEnsemble[i].widgetImplementation->setMouseCursorIcon( QIcon( ":/common/paintCrosshair.png" ) );
	}
	m_ViewerCore->setShowCrosshair(false);
	m_ViewerCore->updateScene();
}



void MaskEditDialog::radiusChange(int r)
{
	m_Radius = r;
}


void MaskEditDialog::showEvent(QShowEvent* )
{
	connect( m_ViewerCore, SIGNAL ( emitPhysicalCoordsChanged(util::fvector4)), this, SLOT( physicalCoordChanged(util::fvector4)));	
	m_Interface.maskType->clear();
	BOOST_FOREACH( std::list<std::string>::const_reference type, isis::viewer::getSupportedTypeList())
	{
		m_Interface.maskType->addItem( type.c_str() );
	}
	if( m_ViewerCore->hasImage() ) {
		const util::fvector4 voxelSize = m_ViewerCore->getCurrentImage()->getISISImage()->getPropertyAs<util::fvector4>("voxelSize") ;
		m_Interface.xRes->setValue( voxelSize[0] );
		m_Interface.yRes->setValue( voxelSize[1] );
		m_Interface.zRes->setValue( voxelSize[2] );
	} else {
		m_Interface.xRes->setValue(3);
		m_Interface.yRes->setValue(3);
		m_Interface.zRes->setValue(3);
	}

}


void MaskEditDialog::physicalCoordChanged(util::fvector4 physCoord)
{
	if( m_CurrentMask ) {

		switch( m_CurrentMajorTypeID ) {
			case isis::data::ValuePtr<bool>::staticID:
				manipulateVoxel<bool>(physCoord, std::numeric_limits<bool>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<int8_t>::staticID:
				manipulateVoxel<int8_t>(physCoord,std::numeric_limits<int8_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<uint8_t>::staticID:
				manipulateVoxel<uint8_t>(physCoord,std::numeric_limits<uint8_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<int16_t>::staticID:
				manipulateVoxel<int16_t>(physCoord, std::numeric_limits<int16_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<uint16_t>::staticID:
				manipulateVoxel<uint16_t>(physCoord, std::numeric_limits<uint16_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<int32_t>::staticID:
				manipulateVoxel<int32_t>(physCoord, std::numeric_limits<int32_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<uint32_t>::staticID:
				manipulateVoxel<uint32_t>(physCoord, std::numeric_limits<uint32_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<int64_t>::staticID:
				manipulateVoxel<int64_t>(physCoord, std::numeric_limits<int64_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<uint64_t>::staticID:
				manipulateVoxel<uint64_t>(physCoord, std::numeric_limits<uint64_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<double>::staticID:
				manipulateVoxel<double>(physCoord, std::numeric_limits<double>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<float>::staticID:
				manipulateVoxel<float>(physCoord, std::numeric_limits<float>::max(), m_CurrentMask );
				break;
		}
		m_ViewerCore->updateScene();
	}

}
	
void MaskEditDialog::createEmptyMask()
{
	if( m_ViewerCore->hasImage() ) {
		boost::shared_ptr<ImageHolder> refImage = m_ViewerCore->getCurrentImage();
		std::string dataType = m_Interface.maskType->currentText().toStdString();
		dataType.append("*");
		boost::shared_ptr<ImageHolder> maskImage;
		m_CurrentMajorTypeID = isis::util::getTransposedTypeMap(false, true).at( dataType );
		switch ( isis::util::getTransposedTypeMap(false, true).at( dataType ) ) {
			case isis::data::ValuePtr<bool>::staticID:
				maskImage = _createEmptyMask<bool>(refImage);
				break;
			case isis::data::ValuePtr<int8_t>::staticID:
				maskImage = _createEmptyMask<int8_t>(refImage);
				break;
			case isis::data::ValuePtr<uint8_t>::staticID:
				maskImage = _createEmptyMask<uint8_t>(refImage);
				break;
			case isis::data::ValuePtr<int16_t>::staticID:
				maskImage = _createEmptyMask<int16_t>(refImage);
				break;
			case isis::data::ValuePtr<uint16_t>::staticID:
				maskImage = _createEmptyMask<uint16_t>(refImage);
				break;
			case isis::data::ValuePtr<int32_t>::staticID:
				maskImage = _createEmptyMask<int32_t>(refImage);
				break;
			case isis::data::ValuePtr<uint32_t>::staticID:
				maskImage = _createEmptyMask<uint32_t>(refImage);
				break;
			case isis::data::ValuePtr<int64_t>::staticID:
				maskImage = _createEmptyMask<int64_t>(refImage);
				break;
			case isis::data::ValuePtr<uint64_t>::staticID:
				maskImage = _createEmptyMask<uint64_t>(refImage);
				break;
			case isis::data::ValuePtr<float>::staticID:
				maskImage = _createEmptyMask<float>(refImage);
				break;
			case isis::data::ValuePtr<double>::staticID:
				maskImage = _createEmptyMask<double>(refImage);
				break;
			default:
				LOG( Runtime, error )  << "Unknown type " << dataType << " !";
				return;
		}
		m_CurrentMask = maskImage;
		m_CurrentMask->extent = m_CurrentMask->minMax.second->as<double>() -  m_CurrentMask->minMax.first->as<double>();
		m_CurrentMask->opacity = 0.5;
		m_CurrentMask->lut = "maskeditLUT";
		m_CurrentMajorTypeID = m_CurrentMask->getISISImage()->getMajorTypeID();
		BOOST_FOREACH( UICore::ViewWidgetEnsembleListType::const_reference ensemble, m_ViewerCore->getUI()->getEnsembleList() ) {
			WidgetInterface::ImageVectorType iVector;
			for( unsigned short i = 0; i < 3; i++ ) {
				iVector = ensemble[i].widgetImplementation->getImageVector();
				if( std::find( iVector.begin(), iVector.end(), refImage ) != iVector.end() ) {
					m_CurrentWidgetEnsemble = ensemble;
					m_ViewerCore->attachImageToWidget( m_CurrentMask, ensemble[i].widgetImplementation ) ;
					ensemble[i].widgetImplementation->setMouseCursorIcon( QIcon( ":/common/paintCrosshair.png" ) );
					
				}			
			}
		}
		m_ViewerCore->setCurrentImage( m_CurrentMask );
		m_ViewerCore->setShowCrosshair(false);
		m_ViewerCore->updateScene();
		m_ViewerCore->getUI()->refreshUI();
		m_Interface.cut->setEnabled(true);
		m_Interface.paint->setEnabled(true);
		m_Interface.radius->setEnabled(true);
		m_Interface.paint->setChecked(true);
	}
}

void MaskEditDialog::closeEvent(QCloseEvent* )
{
	disconnect( m_ViewerCore, SIGNAL ( emitPhysicalCoordsChanged(util::fvector4)), this, SLOT( physicalCoordChanged(util::fvector4)));
	BOOST_FOREACH( UICore::ViewWidgetEnsembleListType::const_reference ensemble, m_ViewerCore->getUI()->getEnsembleList() ) {
		for ( unsigned short i = 0; i < 3; i++ ) {
			ensemble[i].widgetImplementation->setMouseCursorIcon( QIcon() );
			ensemble[i].widgetImplementation->setEnableCrosshair(true);
		}
	}
	
}

	
}}}