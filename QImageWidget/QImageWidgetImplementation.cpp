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
	  m_MemoryHandler( core ),
	  m_Painter( new QPainter() )
{
	( new QVBoxLayout( parent ) )->addWidget( this );
	commonInit();
}


QImageWidgetImplementation::QImageWidgetImplementation( QViewerCore *core, QWidget *parent, QWidget *share, PlaneOrientation orienation )
	: QWidget( parent ),
	  WidgetImplementationBase( core, parent, orienation ),
	  m_MemoryHandler( core ),
	  m_Painter( new QPainter() )
{
	( new QVBoxLayout( parent ) )->addWidget( this );
	commonInit();
}

void QImageWidgetImplementation::commonInit()
{	
	
	connect( this, SIGNAL( zoomChanged( float ) ), m_ViewerCore, SLOT( zoomChanged( float ) ) );
	connect( this, SIGNAL( voxelCoordsChanged( util::ivector4 ) ), m_ViewerCore, SLOT( voxelCoordsChanged ( util::ivector4 ) ) );
	connect( this, SIGNAL( physicalCoordsChanged( util::fvector4 ) ), m_ViewerCore, SLOT( physicalCoordsChanged ( util::fvector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitUpdateScene( bool ) ), this, SLOT( updateScene( bool ) ) );
	connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( lookAtPhysicalCoords( util::fvector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitVoxelCoordChanged( util::ivector4 ) ), this, SLOT( lookAtVoxelCoords(util::ivector4)) );
	connect( m_ViewerCore, SIGNAL( emitZoomChanged( float ) ), this, SLOT( setZoom( float ) ) );
	setAutoFillBackground( true );
	setPalette( QPalette( Qt::black ) );
	m_ColorHandler.setLutType( Color::standard_grey_values );
	m_ColorHandler.update();
	m_WidgetProperties.setPropertyAs<bool>( "mousePressedRight", false );
	m_WidgetProperties.setPropertyAs<bool>( "mousePressedLeft", false );
	m_WidgetProperties.setPropertyAs<float>( "currentZoom", 1.0 );
	m_WidgetProperties.setPropertyAs<float>( "zoomFactorIn", 1.5 );
	m_WidgetProperties.setPropertyAs<float>( "zoomFactorOut", 1.5 );

}


WidgetImplementationBase *QImageWidgetImplementation::createSharedWidget( QWidget *parent, PlaneOrientation orientation )
{
	return new QImageWidgetImplementation( m_ViewerCore, parent, this, orientation );
}


void QImageWidgetImplementation::addImage( const boost::shared_ptr< ImageHolder > image )
{
	m_ImageVector.push_back( image );
}

bool QImageWidgetImplementation::removeImage(const boost::shared_ptr< ImageHolder > image)
{
	m_ImageVector.erase( std::find(m_ImageVector.begin(), m_ImageVector.end(), image ) );
}


void QImageWidgetImplementation::setZoom( float zoom )
{
	m_WidgetProperties.setPropertyAs<float>( "currentZoom", zoom );
	update();
}

void QImageWidgetImplementation::paintEvent( QPaintEvent *event )
{
	m_Painter->begin( this );
	switch (m_InterpolationType) {
		case nn:
			m_Painter->setRenderHint(QPainter::Antialiasing, true );
			break;
		case lin:
			m_Painter->setRenderHint(QPainter::SmoothPixmapTransform, true );
			break;
	}
	BOOST_FOREACH( ImageVectorType::const_reference image, m_ImageVector ) {
		if( image->getPropMap().getPropertyAs<bool>( "isVisible" ) ) {
			paintImage( image);
		}
	}
	paintCrosshair();
	m_Painter->end();

}



void QImageWidgetImplementation::paintImage( boost::shared_ptr< ImageHolder > image )
{
	if( image->getImageProperties().imageType == ImageHolder::z_map ) {
		m_ColorHandler.setLutType( Color::zmap_standard );
	} else {
		m_ColorHandler.setLutType( Color::standard_grey_values );
	}
	m_ColorHandler.update();
	
	util::ivector4 mappedSizeAligned = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>( "alignedSize32Bit" ), image, m_PlaneOrientation );
	isis::data::MemChunk<InternalImageType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );

	m_MemoryHandler.fillSliceChunk( sliceChunk, image, m_PlaneOrientation );

	QImage qImage( ( InternalImageType * ) sliceChunk.asValuePtr<InternalImageType>().getRawAddress().get(),
				   mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_Indexed8 );
	
	qImage.setColorTable( m_ColorHandler.getColorTable() );
	
	m_Painter->resetMatrix();
	const float currentZoom = m_WidgetProperties.getPropertyAs<float>("currentZoom");
	m_Viewport = QOrienationHandler::getViewPort( image, width(), height(), 
						      currentZoom, 
						      m_PlaneOrientation, 
						      m_WidgetProperties.getPropertyAs<bool>( "mousePressedRight" )
    						);
	m_Viewport[0] *= currentZoom;
	m_Viewport[1] *= currentZoom;
	m_Viewport[4] *= currentZoom;
	m_Viewport[5] *= currentZoom;
	m_Painter->setTransform( QOrienationHandler::getTransform( m_Viewport, m_WidgetProperties, image, width(), height(), m_PlaneOrientation ) );
	
	m_Painter->setOpacity( image->getPropMap().getPropertyAs<float>( "opacity" ) );
	m_Painter->drawImage( 0, 0, qImage );

}


void QImageWidgetImplementation::mousePressEvent( QMouseEvent *e )
{
	QWidget::mousePressEvent( e );

	if( e->button() == Qt::RightButton ) {
		m_WidgetProperties.setPropertyAs<bool>( "mousePressedRight", true );
	} else if ( e->button() == Qt::LeftButton ) {
		m_WidgetProperties.setPropertyAs<bool>( "mousePressedLeft", true );
	}
	emitMousePressEvent( e );
}

void QImageWidgetImplementation::mouseMoveEvent( QMouseEvent *e )
{
	if( m_WidgetProperties.getPropertyAs<bool>( "mousePressedRight" ) || m_WidgetProperties.getPropertyAs<bool>( "mousePressedLeft" ) ) {
		emitMousePressEvent( e );
	}
}

bool QImageWidgetImplementation::isInViewPort( QMouseEvent* e ) const
{
	return (e->x() > m_Viewport[2] && e->x() < (m_Viewport[2] + m_Viewport[4])
		&& e->y() > m_Viewport[3] && e->y() < (m_Viewport[3] + m_Viewport[5])
	);
}


void QImageWidgetImplementation::emitMousePressEvent( QMouseEvent *e )
{
	boost::shared_ptr<ImageHolder> image = m_ViewerCore->getCurrentImage();
	if( isInViewPort( e ) ) {
		size_t slice = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>( "voxelCoords" ), m_ViewerCore->getCurrentImage(), m_PlaneOrientation )[2];
		util::ivector4 coords = QOrienationHandler::convertWindow2VoxelCoords( m_Viewport, m_WidgetProperties, image, e->x(), e->y(), slice, m_PlaneOrientation );
		physicalCoordsChanged( image->getImage()->getPhysicalCoordsFromIndex( coords ) );
	}
}

void QImageWidgetImplementation::paintCrosshair()
{
	std::pair<size_t, size_t> coords = QOrienationHandler::convertVoxel2WindowCoords( m_Viewport, m_WidgetProperties, m_ViewerCore->getCurrentImage(), m_PlaneOrientation  );
	util::fvector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( m_ViewerCore->getCurrentImage()->getImageSize(), m_ViewerCore->getCurrentImage(), m_PlaneOrientation );

	QLine xline1( coords.first, 0, coords.first, coords.second - 15 );
	QLine xline2( coords.first, coords.second + 15, coords.first, height() );

	QLine yline1( 0, coords.second, coords.first - 15, coords.second );
	QLine yline2( coords.first + 15, coords.second, width(), coords.second  );

	QPen pen;
	QVector<qreal> dashes;
	qreal space = 4;
	dashes << 27 << space << 27 << space ;
	pen.setDashPattern(dashes);
	pen.setColor( QColor( 255, 102, 0 ) );

	m_Painter->scale( 1.0 / m_Viewport[0], 1.0 / m_Viewport[1] );
	m_Painter->translate( -m_Viewport[2], -m_Viewport[3] );
	m_Painter->setPen( pen );
	m_Painter->drawLine( xline1 );
	m_Painter->drawLine( xline2 );
	m_Painter->drawLine( yline1 );
	m_Painter->drawLine( yline2 );
	pen.setWidth( 2 );;
	m_Painter->drawPoint( coords.first, coords.second );
}

bool QImageWidgetImplementation::lookAtPhysicalCoords( const isis::util::fvector4 &physicalCoords )
{
	BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) 
	{
		image.second->getPropMap().setPropertyAs<util::fvector4>( "physicalCoords", physicalCoords );
		image.second->getPropMap().setPropertyAs<util::ivector4>( "voxelCoords", image.second->getImage()->getIndexFromPhysicalCoords( physicalCoords ) );
	}
	update();
}

bool QImageWidgetImplementation::lookAtVoxelCoords(const isis::util::ivector4& voxelCoords)
{
	BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) 
	{
		image.second->getPropMap().setPropertyAs<util::fvector4>( "voxelCoords", voxelCoords );
		image.second->getPropMap().setPropertyAs<util::ivector4>( "physicalCoords", image.second->getImage()->getIndexFromPhysicalCoords( voxelCoords ) );
	}
	update();
}


void QImageWidgetImplementation::mouseReleaseEvent( QMouseEvent *e )
{
	QWidget::mouseReleaseEvent( e );

	if( e->button() == Qt::RightButton ) {
		m_WidgetProperties.setPropertyAs<bool>( "mousePressedRight", false );
	} 
	if ( e->button() == Qt::LeftButton ) {
		m_WidgetProperties.setPropertyAs<bool>( "mousePressedLeft", false );
	}
}

void QImageWidgetImplementation::wheelEvent( QWheelEvent *e )
{
	float oldZoom = m_WidgetProperties.getPropertyAs<float>( "currentZoom" );

	if ( e->delta() < 0 ) {
		oldZoom /= m_WidgetProperties.getPropertyAs<float>( "zoomFactorOut" );
	} else {
		oldZoom *= m_WidgetProperties.getPropertyAs<float>( "zoomFactorIn" );

	}

	if( m_ViewerCore->getOption()->propagateZooming ) {
		zoomChanged( oldZoom );
	} else {
		setZoom( oldZoom );
	}

}

void QImageWidgetImplementation::updateScene(bool )
{
	update();
}


}
}
} //end namespace