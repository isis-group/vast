#include "QImageWidgetImplementation.hpp"

#include "QOrientationHandler.hpp"

namespace isis
{
namespace viewer
{
namespace qt
{


QImageWidgetImplementation::QImageWidgetImplementation( QViewerCore *core, QWidget *parent, PlaneOrientation orientation )
	: QWidget( parent ),
	  QWidgetImplementationBase( core, parent, orientation ),
	  m_MemoryHandler( core ),
	  m_Painter( new QPainter() )
{
	( new QVBoxLayout( parent ) )->addWidget( this );
	commonInit();
}


QImageWidgetImplementation::QImageWidgetImplementation( QViewerCore *core, QWidget *parent, QWidget *share, PlaneOrientation orienation )
	: QWidget( parent ),
	  QWidgetImplementationBase( core, parent, orienation ),
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
	m_WidgetProperties.setPropertyAs<bool>( "mousePressedRight", false );
	m_WidgetProperties.setPropertyAs<bool>( "mousePressedLeft", false );
	
	m_WidgetProperties.setPropertyAs<bool>( "zoomEvent", false );
	m_WidgetProperties.setPropertyAs<float>( "currentZoom", 1.0 );
	m_WidgetProperties.setPropertyAs<float>( "zoomFactorIn", 1.5 );
	m_WidgetProperties.setPropertyAs<float>( "zoomFactorOut", 1.5 );
	m_WidgetProperties.setPropertyAs<float>( "maxZoom", 20 );
	
	m_WidgetProperties.setPropertyAs<float>( "translationX", 0 );
	m_WidgetProperties.setPropertyAs<float>( "translationY", 0 );

}


QWidgetImplementationBase *QImageWidgetImplementation::createSharedWidget( QWidget *parent, PlaneOrientation orientation )
{
	return new QImageWidgetImplementation( m_ViewerCore, parent, this, orientation );
}


void QImageWidgetImplementation::addImage( const boost::shared_ptr< ImageHolder > image )
{
	m_ViewPortMap.insert( std::make_pair< boost::shared_ptr<ImageHolder>, ViewPortType>  (image, util::FixedVector<float,6>()));
	m_ImageVector.push_back( image );
}

bool QImageWidgetImplementation::removeImage(const boost::shared_ptr< ImageHolder > image)
{
	m_ViewPortMap.erase( image );
	m_ImageVector.erase( std::find(m_ImageVector.begin(), m_ImageVector.end(), image ) );
}


void QImageWidgetImplementation::setZoom( float zoom )
{
	if( zoom <= m_WidgetProperties.getPropertyAs<float>("maxZoom") ) {
		m_WidgetProperties.setPropertyAs<bool>("zoomEvent", true);
		m_WidgetProperties.setPropertyAs<float>( "currentZoom", zoom >= 1.0 ? zoom : 1.0 );
		update();
	}
}

void QImageWidgetImplementation::paintEvent( QPaintEvent *event )
{
	m_Painter->begin( this );
	
	//painting all anatomical images
	BOOST_FOREACH( ImageVectorType::const_reference image, m_ImageVector ) {
		if( image->getPropMap().getPropertyAs<bool>( "isVisible" )  && image->getImageProperties().imageType == ImageHolder::anatomical_image ) {
			paintImage( image);
		}
	}
	//painting the zmaps
	BOOST_FOREACH( ImageVectorType::const_reference image, m_ImageVector ) {
		if( image->getPropMap().getPropertyAs<bool>( "isVisible" )  && image->getImageProperties().imageType == ImageHolder::z_map ) {
			paintImage( image);
		}
	}
	paintCrosshair();
	m_Painter->end();

}

void QImageWidgetImplementation::recalculateTranslation()
{
	boost::shared_ptr<ImageHolder > image = m_ViewerCore->getCurrentImage();
	util::ivector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( image->getImageSize(), image, m_PlaneOrientation );
	util::ivector4 mappedVoxelCoords = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>( "voxelCoords" ), image, m_PlaneOrientation, false, false );
	util::ivector4 center = mappedSize / 2;
	util::ivector4 diff = center - mappedVoxelCoords;
	float zoom = m_WidgetProperties.getPropertyAs<float>("currentZoom");
	float transXConst = ( (center[0]+2) - mappedSize[0] / (2 * zoom) );
	float transYConst = ( (center[1]+2) - mappedSize[1] / (2 * zoom) );
	float transX = transXConst * ((float)diff[0] / (float)center[0]);
	float transY = transYConst * ((float)diff[1] / (float)center[1]);
	
	m_WidgetProperties.setPropertyAs<float>("translationX", transX * m_ViewPortMap[image][0] );
	m_WidgetProperties.setPropertyAs<float>( "translationY", transY * m_ViewPortMap[image][1] );
	m_WidgetProperties.setPropertyAs<bool>("zoomEvent", false);
}


void QImageWidgetImplementation::paintImage( boost::shared_ptr< ImageHolder > image )
{
	switch (image->getImageProperties().interpolationType) {
		case nn:
			m_Painter->setRenderHint(QPainter::Antialiasing, true );
			break;
		case lin:
			m_Painter->setRenderHint(QPainter::SmoothPixmapTransform, true );
			break;
	}
	if( image->getImageProperties().imageType == ImageHolder::z_map ) {
		m_ColorHandler.setLutType( Color::zmap_standard );
		m_ColorHandler.setOmitZeros(true);
	} else {
		m_ColorHandler.setLutType( Color::standard_grey_values );
	}
	if( m_ScalingType == automatic_scaling ) {
		m_ColorHandler.setOffsetAndScaling( image->getOptimalScalingPair() );
	}
	
	//TODO only update if necessary
	m_ColorHandler.setImage(image);
	m_ColorHandler.update();
	util::ivector4 mappedSizeAligned = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>( "alignedSize32Bit" ), image, m_PlaneOrientation );
	isis::data::MemChunk<InternalImageType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );
	
	m_MemoryHandler.fillSliceChunk( sliceChunk, image, m_PlaneOrientation, image->getPropMap().getPropertyAs<uint16_t>("currentTimestep") );
	
	QImage qImage( ( InternalImageType * ) sliceChunk.asValuePtr<InternalImageType>().getRawAddress().get(),
				   mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_Indexed8 );
	
	qImage.setColorTable( m_ColorHandler.getColorTable() );
	
	m_Painter->resetMatrix();
	m_ViewPortMap[image] =  QOrienationHandler::getViewPort( m_WidgetProperties, image, width(), height(),  
							m_PlaneOrientation
    						);
	if( !m_WidgetProperties.getPropertyAs<bool>("mousePressedLeft") || m_WidgetProperties.getPropertyAs<bool>("mousePressedRight") || m_WidgetProperties.getPropertyAs<bool>("zoomEvent")) {
		recalculateTranslation();
	}
	m_ViewPortMap[image][2] += m_WidgetProperties.getPropertyAs<float>("translationX");
	m_ViewPortMap[image][3] += m_WidgetProperties.getPropertyAs<float>("translationY");
	
	m_Painter->setTransform( QOrienationHandler::getTransform( m_ViewPortMap[image], m_WidgetProperties, image, width(), height(), m_PlaneOrientation ) );
	
	m_Painter->setOpacity( image->getPropMap().getPropertyAs<float>( "opacity" ) );
	m_Painter->drawImage( 0, 0, qImage );

}


void QImageWidgetImplementation::mousePressEvent( QMouseEvent *e )
{
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

bool QImageWidgetImplementation::isInViewPort( const ViewPortType &viewPort, QMouseEvent* e ) const
{
	return (e->x() > viewPort[2] && e->x() < (viewPort[2] + viewPort[4])
		&& e->y() > viewPort[3] && e->y() < (viewPort[3] + viewPort[5])
	);
}


void QImageWidgetImplementation::emitMousePressEvent( QMouseEvent *e )
{
	boost::shared_ptr<ImageHolder> image = m_ViewerCore->getCurrentImage();
	if( isInViewPort( m_ViewPortMap.at(image), e ) ) {
		uint16_t slice = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>( "voxelCoords" ), image, m_PlaneOrientation )[2];
		util::ivector4 coords = QOrienationHandler::convertWindow2VoxelCoords( m_ViewPortMap[image], m_WidgetProperties, image, e->x(), e->y(), slice, m_PlaneOrientation );
		physicalCoordsChanged( image->getISISImage()->getPhysicalCoordsFromIndex( coords ) );
	}
}

void QImageWidgetImplementation::paintCrosshair()
{
	boost::shared_ptr< ImageHolder > image = m_ViewerCore->getCurrentImage();
	std::pair<uint16_t, size_t> coords = QOrienationHandler::convertVoxel2WindowCoords( m_ViewPortMap[image], m_WidgetProperties, m_ViewerCore->getCurrentImage(), m_PlaneOrientation  );

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
	m_Painter->setOpacity(1.0);
	m_Painter->resetTransform();
	m_Painter->setTransform( QOrienationHandler::getTransform( m_ViewPortMap[image], m_WidgetProperties, image, width(), height(), m_PlaneOrientation ) );
	m_Painter->scale( 1.0 / m_ViewPortMap[image][0], 1.0 / m_ViewPortMap[image][1] );
	m_Painter->translate( -m_ViewPortMap[image][2], -m_ViewPortMap[image][3] );
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
	m_WidgetProperties.setPropertyAs<uint16_t>("currentTimeStep", physicalCoords[3] );
	BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) 
	{
		image.second->getPropMap().setPropertyAs<util::fvector4>( "physicalCoords", physicalCoords );
		image.second->getPropMap().setPropertyAs<util::ivector4>( "voxelCoords", image.second->getISISImage()->getIndexFromPhysicalCoords( physicalCoords ) );
	}
	update();
}

bool QImageWidgetImplementation::lookAtVoxelCoords(const isis::util::ivector4& voxelCoords)
{
	m_WidgetProperties.setPropertyAs<uint16_t>("currentTimeStep", voxelCoords[3] );
	BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) 
	{
		image.second->getPropMap().setPropertyAs<util::ivector4>( "voxelCoords", voxelCoords );
		image.second->getPropMap().setPropertyAs<util::fvector4>( "physicalCoords", image.second->getISISImage()->getPhysicalCoordsFromIndex( voxelCoords ) );
	}
	update();
}


void QImageWidgetImplementation::mouseReleaseEvent( QMouseEvent *e )
{
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

void QImageWidgetImplementation::updateScene(bool center)
{
	update();
}

std::string QImageWidgetImplementation::getWidgetName() const
{
	return windowTitle().toStdString();
}

void QImageWidgetImplementation::setWidgetName(const std::string& wName)
{
	setWindowTitle( QString( wName.c_str() ) );
	m_WidgetProperties.setPropertyAs<std::string>( "widgetName", wName );
}


}
}
} //end namespace
