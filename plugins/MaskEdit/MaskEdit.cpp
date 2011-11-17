
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
	m_ViewerCore->getColorHandler()->addColormap( ":/common/maskeditLUT" );
	connect( m_Interface.newMask, SIGNAL( clicked()), this, SLOT( createEmptyMask())) ;
	connect( m_Interface.radius, SIGNAL( valueChanged(int)), SLOT( radiusChange(int)));
	connect( m_Interface.cut, SIGNAL( clicked(bool)), this , SLOT( cutClicked()));
	connect( m_Interface.paint, SIGNAL( clicked(bool)), this , SLOT( paintClicked()));

}

void MaskEditDialog::cutClicked()
{
	m_Interface.paint->setChecked( !m_Interface.cut->isChecked() );
}

void MaskEditDialog::paintClicked()
{
	m_Interface.cut->setChecked( !m_Interface.paint->isChecked() );
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
}


void MaskEditDialog::physicalCoordChanged(util::fvector4 physCoord)
{
	util::ivector4 voxel = m_CurrentMask->getISISImage()->getIndexFromPhysicalCoords( physCoord, true );
	util::ivector4 imageSize = m_CurrentMask->getImageSize();
	util::ivector4 start;
	util::ivector4 end;

	for( size_t i = 0; i < 3; i++ ) {
		start[i] = ( voxel[i] - m_Radius ) < 0 ? 0 : voxel[i] - m_Radius;
		end[i] = ( voxel[i] + m_Radius ) > imageSize[i] ? imageSize[i] : voxel[i] + m_Radius;
	}
	unsigned short radSquare = m_Radius * m_Radius;
	for( unsigned short k = start[2] + 1; k < end[2]; k++ ) {
		for( unsigned short j = start[1] + 1; j < end[1]; j++ ) {
			for( unsigned short i = start[0] + 1; i < end[0]; i++ ) {
				int x = voxel[0] - i;
				int y = voxel[1] - j;
				int z = voxel[2] - k;
				if( x * x + y * y + z * z <= radSquare ) {
					if( m_Interface.paint->isChecked() ) {
						m_CurrentMask->getISISImage()->voxel<uint8_t>( i, j, k ) = std::numeric_limits<uint8_t>::max();
						m_CurrentMask->getChunkVector()[0].voxel<uint8_t>( i, j, k ) = std::numeric_limits<uint8_t>::max();	
					} else {
						m_CurrentMask->getISISImage()->voxel<uint8_t>( i, j, k ) = std::numeric_limits<uint8_t>::min();
						m_CurrentMask->getChunkVector()[0].voxel<uint8_t>( i, j, k ) = std::numeric_limits<uint8_t>::min();	

					}
				}
			}
		}
	}

}
	
void MaskEditDialog::createEmptyMask()
{
	if( m_ViewerCore->hasImage() ) {
		boost::shared_ptr<ImageHolder> refImage = m_ViewerCore->getCurrentImage();
		util::ivector4 size = refImage->getImageSize();
		isis::data::MemChunk<uint8_t> ch( size[0], size[1], size[2] );
		ch.join( static_cast<isis::util::PropertyMap&>( *refImage->getISISImage() ) );		
		isis::data::Image mask ( ch);
		mask.setPropertyAs<std::string>("source", m_Interface.maskName->text().toStdString() );
	
		m_CurrentMask = m_ViewerCore->addImage( mask, ImageHolder::anatomical_image );
		m_CurrentMask->minMax.second = isis::util::Value<uint8_t>( std::numeric_limits<uint8_t>::max() );
		m_CurrentMask->internMinMax.second = isis::util::Value<uint8_t>( std::numeric_limits<uint8_t>::max() );
		m_CurrentMask->extent = m_CurrentMask->minMax.second->as<double>() -  m_CurrentMask->minMax.first->as<double>();
		m_CurrentMask->opacity = 0.5;
		m_CurrentMask->lut = "maskeditLUT";
		BOOST_FOREACH( UICore::ViewWidgetEnsembleListType::const_reference ensemble, m_ViewerCore->getUI()->getEnsembleList() ) {
			WidgetInterface::ImageVectorType iVector;
			for( unsigned short i = 0; i < 3; i++ ) {
				iVector = ensemble[i].widgetImplementation->getImageVector();
				if( std::find( iVector.begin(), iVector.end(), refImage ) != iVector.end() ) {
					m_ViewerCore->attachImageToWidget( m_CurrentMask, ensemble[i].widgetImplementation ) ;
					ensemble[i].widgetImplementation->setMouseCursorIcon( QIcon( ":/common/brush.png" ) );
					
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