
#include "MaskEdit.hpp"
#include <DataStorage/image.hpp>
#include "uicore.hpp"


namespace isis {
namespace viewer {
namespace plugin {

MaskEditDialog::MaskEditDialog(QWidget* parent, QViewerCore* core)
	: QDialog(parent),
	m_ViewerCore(core),
	m_Radius(5)
{
	m_Interface.setupUi( this );
	connect( m_ViewerCore, SIGNAL ( emitPhysicalCoordsChanged(util::fvector4)), this, SLOT( physicalCoordChanged(util::fvector4)));
	connect( m_Interface.createMask, SIGNAL( clicked()), this, SLOT( createEmptyMask())) ;

}

void MaskEditDialog::physicalCoordChanged(util::fvector4 physCoord)
{
	util::ivector4 voxel = m_CurrentMask->getISISImage()->getIndexFromPhysicalCoords( physCoord );
	util::ivector4 imageSize = m_CurrentMask->getImageSize();
	util::ivector4 start;
	util::ivector4 end;

	for( size_t i = 0; i < 3; i++ ) {
		start[i] = ( voxel[i] - m_Radius ) < 0 ? 0 : voxel[i] - m_Radius;
		end[i] = ( voxel[i] + m_Radius ) > imageSize[i] ? imageSize[i] : voxel[i] + m_Radius;
	}
	for( unsigned short z = start[2]; z < end[2]; z++ ) {
		for( unsigned short y = start[1]; y < end[1]; y++ ) {
			for( unsigned short x = start[0]; x < end[0]; x++ ) {
				m_CurrentMask->getISISImage()->voxel<uint8_t>( x, y, z ) = std::numeric_limits<uint8_t>::max();
				m_CurrentMask->getChunkVector()[0].voxel<uint8_t>( x, y, z ) = std::numeric_limits<uint8_t>::max();	
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
	}
}

void MaskEditDialog::closeEvent(QCloseEvent* )
{
	disconnect( m_ViewerCore, SIGNAL ( emitPhysicalCoordsChanged(util::fvector4)), this, SLOT( physicalCoordChanged(util::fvector4)));
}

	
}}}