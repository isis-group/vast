
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
	m_Interface.cut->setEnabled( false );
	m_Interface.paint->setEnabled( false );
	m_Interface.radius->setEnabled( false );
	m_Interface.radius->setMaximum( 500 );
	m_Interface.radius->setValue( m_Radius );

	util::Singletons::get<color::Color, 10>().addColormap( ":/common/maskeditLUT" );
	connect( m_Interface.newMask, SIGNAL( clicked() ), this, SLOT( createEmptyMask() ) ) ;
	connect( m_Interface.radius, SIGNAL( valueChanged( int ) ), SLOT( radiusChange( int ) ) );
	connect( m_Interface.cut, SIGNAL( clicked( bool ) ), this , SLOT( cutClicked() ) );
	connect( m_Interface.paint, SIGNAL( clicked( bool ) ), this , SLOT( paintClicked() ) );
	connect( m_Interface.editCurrentImage, SIGNAL( clicked() ), this, SLOT( editCurrentImage() ) );

}

void MaskEditDialog::cutClicked()
{
	for( unsigned short i = 0; i < 3; i++ ) {
		m_CurrentWidgetEnsemble[i].widgetImplementation->setMouseCursorIcon( QIcon( ":/common/cutCrosshair.png" ) );
	}

	m_ViewerCore->setShowCrosshair( false );
	m_ViewerCore->updateScene();
}

void MaskEditDialog::paintClicked()
{
	for( unsigned short i = 0; i < 3; i++ ) {
		m_CurrentWidgetEnsemble[i].widgetImplementation->setMouseCursorIcon( QIcon( ":/common/paintCrosshair.png" ) );
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
	connect( m_ViewerCore, SIGNAL ( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( physicalCoordChanged( util::fvector4 ) ) );

	if( !m_CurrentMask ) {
		m_Interface.cut->setEnabled( false );
		m_Interface.paint->setEnabled( false );
		m_Interface.radius->setEnabled( false );
	} else {
		m_Interface.cut->setEnabled( true );
		m_Interface.paint->setEnabled( true );
		m_Interface.radius->setEnabled( true );
		m_Interface.paint->setChecked( true );
	}

}


void MaskEditDialog::physicalCoordChanged( util::fvector4 physCoord )
{
	if( m_ViewerCore->hasImage() ) {
		if( m_CurrentMask ) {
			switch( m_CurrentMask->majorTypeID ) {
			case isis::data::ValuePtr<bool>::staticID:
				manipulateVoxel<bool>( physCoord, std::numeric_limits<bool>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<int8_t>::staticID:
				manipulateVoxel<int8_t>( physCoord, std::numeric_limits<int8_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<uint8_t>::staticID:
				manipulateVoxel<uint8_t>( physCoord, std::numeric_limits<uint8_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<int16_t>::staticID:
				manipulateVoxel<int16_t>( physCoord, std::numeric_limits<int16_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<uint16_t>::staticID:
				manipulateVoxel<uint16_t>( physCoord, std::numeric_limits<uint16_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<int32_t>::staticID:
				manipulateVoxel<int32_t>( physCoord, std::numeric_limits<int32_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<uint32_t>::staticID:
				manipulateVoxel<uint32_t>( physCoord, std::numeric_limits<uint32_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<int64_t>::staticID:
				manipulateVoxel<int64_t>( physCoord, std::numeric_limits<int64_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<uint64_t>::staticID:
				manipulateVoxel<uint64_t>( physCoord, std::numeric_limits<uint64_t>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<double>::staticID:
				manipulateVoxel<double>( physCoord, std::numeric_limits<double>::max(), m_CurrentMask );
				break;
			case isis::data::ValuePtr<float>::staticID:
				manipulateVoxel<float>( physCoord, std::numeric_limits<float>::max(), m_CurrentMask );
				break;
			default:
				LOG( Runtime, error ) << "Unknown type ID " << m_CurrentMask->majorTypeID << " when trying to paint mask";
				break;
			}

			m_ViewerCore->updateScene();
		}
	}

}

void MaskEditDialog::editCurrentImage()
{
	if( m_ViewerCore->hasImage() ) {
		m_CurrentMask = m_ViewerCore->getCurrentImage();
		m_Interface.cut->setEnabled( true );
		m_Interface.paint->setEnabled( true );
		m_Interface.radius->setEnabled( true );
		m_Interface.paint->setChecked( true );
		BOOST_FOREACH( UICore::ViewWidgetEnsembleListType::const_reference ensemble, m_ViewerCore->getUICore()->getEnsembleList() ) {
			WidgetInterface::ImageVectorType iVector;

			for( unsigned short i = 0; i < 3; i++ ) {
				iVector = ensemble[i].widgetImplementation->getImageVector();

				if( std::find( iVector.begin(), iVector.end(), m_CurrentMask ) != iVector.end() ) {
					m_CurrentWidgetEnsemble = ensemble;
					m_ViewerCore->attachImageToWidget( m_CurrentMask, ensemble[i].widgetImplementation ) ;
					ensemble[i].widgetImplementation->setMouseCursorIcon( QIcon( ":/common/paintCrosshair.png" ) );
					ensemble[i].widgetImplementation->setEnableCrosshair( false );

				}
			}
		}
		m_ViewerCore->updateScene();
	}
}


void MaskEditDialog::createEmptyMask()
{
	m_CreateMaskDialog->show();

}

void MaskEditDialog::closeEvent( QCloseEvent * )
{
	disconnect( m_ViewerCore, SIGNAL ( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( physicalCoordChanged( util::fvector4 ) ) );
	BOOST_FOREACH( UICore::ViewWidgetEnsembleListType::const_reference ensemble, m_ViewerCore->getUICore()->getEnsembleList() ) {
		for ( unsigned short i = 0; i < 3; i++ ) {
			ensemble[i].widgetImplementation->setMouseCursorIcon( QIcon() );
			ensemble[i].widgetImplementation->setEnableCrosshair( true );
		}
	}

}


}
}
}