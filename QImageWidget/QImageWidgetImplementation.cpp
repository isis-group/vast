#include "QImageWidgetImplementation.hpp"
#include "QtWidgetCommon.hpp"
#include "QOrientationHandler.hpp"

namespace isis
{
namespace viewer
{
namespace qt
{


QImageWidgetImplementation::QImageWidgetImplementation( QViewerCore *core, QWidget *parent, PlaneOrientation orientation )
	: QWidget( parent ),
	  WidgetImplementationBase( core, parent, orientation ),
	  m_MemoryHandler( core )
{
	( new QVBoxLayout( parent ) )->addWidget( this );
	commonInit();
}


QImageWidgetImplementation::QImageWidgetImplementation( QViewerCore *core, QWidget *parent, QWidget *share, PlaneOrientation orienation )
	: QWidget( parent ),
	  WidgetImplementationBase( core, parent, orienation ),
	  m_MemoryHandler( core )
{
	( new QVBoxLayout( parent ) )->addWidget( this );
	commonInit();
}

void QImageWidgetImplementation::commonInit()
{
	connect( this, SIGNAL( zoomChanged( float ) ), m_ViewerCore, SLOT( zoomChanged( float ) ) );
	connect( this, SIGNAL( physicalCoordsChanged( util::fvector4 ) ), m_ViewerCore, SLOT( physicalCoordsChanged ( util::fvector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( lookAtPhysicalCoords( util::fvector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitZoomChanged( float ) ), this, SLOT( setZoom( float ) ) );
	setAutoFillBackground( true );
	setPalette( QPalette( Qt::black ) );
	m_LutType = Color::standard_grey_values;
	m_Painter = new QPainter();
	m_WidgetProperties.setPropertyAs<bool>("mousePressed", false );
	m_WidgetProperties.setPropertyAs<float>("currentZoom", 1.0 );
	m_WidgetProperties.setPropertyAs<float>("zoomFactorIn", 1.5);
	m_WidgetProperties.setPropertyAs<float>("zoomFactorOut", 1.5);


}


WidgetImplementationBase *QImageWidgetImplementation::createSharedWidget( QWidget *parent, PlaneOrientation orientation )
{
	return new QImageWidgetImplementation( m_ViewerCore, parent, this, orientation );
}


void QImageWidgetImplementation::addImage( const boost::shared_ptr< ImageHolder > image )
{

}

void QImageWidgetImplementation::setZoom( float zoom )
{
    m_WidgetProperties.setPropertyAs<float>("currentZoom", zoom );
    update();
}

void QImageWidgetImplementation::paintEvent( QPaintEvent *event )
{
	m_Painter->begin( this );
	BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
		if( image.second->getPropMap().getPropertyAs<bool>( "isVisible" ) ) {
			paintImage( image.second );
		}
	}
	paintCrosshair();
	m_Painter->end();

}

void QImageWidgetImplementation::paintImage( boost::shared_ptr< ImageHolder > image )
{

	util::ivector4 mappedSizeAligned = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>( "alignedSize32Bit" ), image, m_PlaneOrientation );
	isis::data::MemChunk<InternalImageType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );

	m_MemoryHandler.fillSliceChunk( sliceChunk, image, m_PlaneOrientation );

	QImage qImage( ( InternalImageType * ) sliceChunk.asValuePtr<InternalImageType>().getRawAddress().lock().get(),
				   mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_Indexed8 );
	qImage.setColorCount( 512 );
	qImage.setColorTable( Color::getColorTable( m_LutType ) );

	m_Painter->setTransform( QOrienationHandler::getTransform( m_WidgetProperties, image, width(), height(), m_PlaneOrientation ) );
	m_Painter->setOpacity( image->getPropMap().getPropertyAs<float>( "opacity" ) );
	m_Painter->drawImage( 0, 0, qImage );
}


void QImageWidgetImplementation::mousePressEvent( QMouseEvent *e )
{
	QWidget::mousePressEvent( e );
	m_WidgetProperties.setPropertyAs<bool>("mousePressed", true );
	emitMousePressEvent( e );

	
}

void QImageWidgetImplementation::mouseMoveEvent( QMouseEvent *e )
{
	if( m_WidgetProperties.getPropertyAs<bool>("mousePressed") ) {
	    emitMousePressEvent( e ); 
	}
}

void QImageWidgetImplementation::emitMousePressEvent(QMouseEvent* e)
{
    	
	size_t slice = QOrienationHandler::mapCoordsToOrientation( m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<util::ivector4>("voxelCoords"), m_ViewerCore->getCurrentImage(), m_PlaneOrientation )[2];
	std::pair<size_t, size_t> coords = QOrienationHandler::convertWindow2VoxelCoords( m_WidgetProperties, m_ViewerCore->getCurrentImage(), e->x(), e->y(), m_PlaneOrientation );
	util::ivector4 mappedCoords = QOrienationHandler::mapCoordsToOrientation( util::ivector4( coords.first, coords.second, slice), m_ViewerCore->getCurrentImage(), m_PlaneOrientation, true, false );
	physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getImage()->getPhysicalCoordsFromIndex( mappedCoords ) );
}

void QImageWidgetImplementation::paintCrosshair()
{
	std::pair<size_t, size_t> coords = QOrienationHandler::convertVoxel2WindowCoords( m_WidgetProperties, m_ViewerCore->getCurrentImage(), m_PlaneOrientation  );
	util::fvector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( m_ViewerCore->getCurrentImage()->getImageSize(), m_ViewerCore->getCurrentImage(), m_PlaneOrientation );
	util::fvector4 scalingAndOffset = m_WidgetProperties.getPropertyAs<util::fvector4>( "scalingAndOffset" );
	
	QLine xline1( coords.first, 0, coords.first, coords.second - 15 );
	QLine xline2( coords.first, coords.second + 15, coords.first, height() );
	
	QLine yline1( 0, coords.second, coords.first - 15, coords.second );
	QLine yline2( coords.first + 15, coords.second, width(), coords.second  );
	
	QPen pen;
	pen.setColor( QColor(255, 102, 0) );
	
	m_Painter->scale( 1.0 / scalingAndOffset[0], 1.0 / scalingAndOffset[1] );
	m_Painter->translate( -scalingAndOffset[2], -scalingAndOffset[3] );
	m_Painter->setPen( pen );
	m_Painter->drawLine( xline1 );
	m_Painter->drawLine( xline2 );
	m_Painter->drawLine( yline1 );
	m_Painter->drawLine( yline2 );
	pen.setWidth(2);;
	m_Painter->drawPoint( coords.first, coords.second );
	m_Painter->scale( scalingAndOffset[0], scalingAndOffset[1]);
	m_Painter->translate( scalingAndOffset[2], scalingAndOffset[3] );
}

bool QImageWidgetImplementation::lookAtPhysicalCoords(const isis::util::fvector4& physicalCoords)
{
    m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<util::ivector4>( "voxelCoords", m_ViewerCore->getCurrentImage()->getImage()->getIndexFromPhysicalCoords(physicalCoords) );
    m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<util::ivector4>( "physicalCoords", physicalCoords );
    update();
}

void QImageWidgetImplementation::mouseReleaseEvent(QMouseEvent* e)
{
    QWidget::mouseReleaseEvent(e);
    m_WidgetProperties.setPropertyAs<bool>("mousePressed", false );
}

void QImageWidgetImplementation::wheelEvent(QWheelEvent* e)
{
    float oldZoom = m_WidgetProperties.getPropertyAs<float>("currentZoom");
    if ( e->delta() < 0 ) {
	oldZoom /= m_WidgetProperties.getPropertyAs<float>("zoomFactorOut");
    } else {
	oldZoom *= m_WidgetProperties.getPropertyAs<float>("zoomFactorIn");
	
    }
    if( m_ViewerCore->getOption()->propagateZooming ) {
	    zoomChanged( oldZoom );
    } else {
	    setZoom( oldZoom );
    }
    
}


}
}
} //end namespace