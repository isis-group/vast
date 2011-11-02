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
	connect( this, SIGNAL( physicalCoordsChanged( util::fvector4 ) ), m_ViewerCore, SLOT( physicalCoordsChanged( util::fvector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitUpdateScene( bool ) ), this, SLOT( updateScene( bool ) ) );
	connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( lookAtPhysicalCoords( util::fvector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitVoxelCoordChanged( util::ivector4 ) ), this, SLOT( lookAtVoxelCoords( util::ivector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitZoomChanged( float ) ), this, SLOT( setZoom( float ) ) );
	setAutoFillBackground( true );
	setPalette( QPalette( Qt::black ) );
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
	ImageProperties imgProperties;
	imgProperties.viewPort = ViewPortType();
	m_ImageProperties.insert( std::make_pair<boost::shared_ptr<ImageHolder> , ImageProperties >( image, imgProperties ) );
	m_ImageVector.push_back( image );
	image->addWidget( this );
}

bool QImageWidgetImplementation::removeImage( const boost::shared_ptr< ImageHolder > image )
{
	m_ImageProperties.erase( image );
	m_ImageVector.erase( std::find( m_ImageVector.begin(), m_ImageVector.end(), image ) );
}

boost::shared_ptr< ImageHolder > QImageWidgetImplementation::getWidgetSpecCurrentImage() const
{
	if( std::find( m_ImageVector.begin(), m_ImageVector.end(), m_ViewerCore->getCurrentImage() ) != m_ImageVector.end() ) {
		return m_ViewerCore->getCurrentImage();
	}

	return m_ImageVector.front();

}

void QImageWidgetImplementation::setZoom( float zoom )
{
	if( zoom <= m_WidgetProperties.getPropertyAs<float>( "maxZoom" ) ) {
		m_WidgetProperties.setPropertyAs<bool>( "zoomEvent", true );
		m_WidgetProperties.setPropertyAs<float>( "currentZoom", zoom >= 1.0 ? zoom : 1.0 );
		update();
		m_WidgetProperties.setPropertyAs<bool>( "zoomEvent", false );
	}

}

void QImageWidgetImplementation::paintEvent( QPaintEvent *event )
{
	if( m_ImageVector.size() ) {
		m_Painter->begin( this );
		m_ImageProperties.at( getWidgetSpecCurrentImage() ).viewPort
		= QOrienationHandler::getViewPort( m_WidgetProperties,
										   getWidgetSpecCurrentImage(),
										   width(),
										   height(),
										   m_PlaneOrientation

										 );

		boost::shared_ptr<ImageHolder> cImage =  getWidgetSpecCurrentImage();
		//painting all anatomical images
		BOOST_FOREACH( ImageVectorType::const_reference image, m_ImageVector ) {
			if( image.get() != cImage.get()
				&& image->getPropMap().getPropertyAs<bool>( "isVisible" )
				&& image->getImageProperties().imageType == ImageHolder::anatomical_image ) {
				paintImage( image );
			}
		}

		if( cImage->getImageProperties().imageType == ImageHolder::anatomical_image
			&& cImage->getPropMap().getPropertyAs<bool>( "isVisible" ) ) {
			paintImage( cImage );
		}

		//painting the zmaps
		BOOST_FOREACH( ImageVectorType::const_reference image, m_ImageVector ) {
			if( image.get() != cImage.get()
				&& image->getPropMap().getPropertyAs<bool>( "isVisible" )
				&& image->getImageProperties().imageType == ImageHolder::z_map ) {
				paintImage( image );
			}
		}

		if( cImage->getImageProperties().imageType == ImageHolder::z_map
			&& cImage->getPropMap().getPropertyAs<bool>( "isVisible" ) ) {
			paintImage( cImage );
		}

		paintCrosshair();
		m_Painter->end();
	}

}

void QImageWidgetImplementation::recalculateTranslation()
{
	const boost::shared_ptr<ImageHolder > image = getWidgetSpecCurrentImage();
	const util::ivector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( image->getImageSize(), image, m_PlaneOrientation );
	const util::ivector4 mappedVoxelCoords = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>( "voxelCoords" ), image, m_PlaneOrientation );
	const util::ivector4 signVec = QOrienationHandler::mapCoordsToOrientation( util::ivector4( 1, 1, 1, 1 ), image, m_PlaneOrientation, false, false );
	const util::ivector4 center = mappedSize / 2;
	const util::ivector4 diff = center - mappedVoxelCoords;
	const float zoom = m_WidgetProperties.getPropertyAs<float>( "currentZoom" );
	const float transXConst = ( ( center[0] + 2 ) - mappedSize[0] / ( 2 * zoom ) );
	const float transYConst = ( ( center[1] + 2 ) - mappedSize[1] / ( 2 * zoom ) );
	const float transX = transXConst * ( ( float )diff[0] / ( float )center[0] ) * signVec[0];
	const float transY = transYConst * ( ( float )diff[1] / ( float )center[1] ) * signVec[1];
	const ViewPortType viewPort = m_ImageProperties.at( image ).viewPort;
	m_WidgetProperties.setPropertyAs<float>( "translationX", transX * viewPort[0] );
	m_WidgetProperties.setPropertyAs<float>( "translationY", transY * viewPort[1] );
}


void QImageWidgetImplementation::paintImage( boost::shared_ptr< ImageHolder > image )
{
	ImageProperties &imgProps = m_ImageProperties.at( image );

	switch( m_InterpolationType ) {
	case 0:
		m_Painter->setRenderHint( QPainter::TextAntialiasing, true );
		break;
	case 1:
		m_Painter->setRenderHint( QPainter::SmoothPixmapTransform, true );
		break;
	}

	const util::ivector4 mappedSizeAligned = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>( "alignedSize32Bit" ), image, m_PlaneOrientation );

	isis::data::MemChunk<InternalImageType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );

	m_MemoryHandler.fillSliceChunk( sliceChunk, image, m_PlaneOrientation, image->getPropMap().getPropertyAs<uint16_t>( "currentTimestep" ) );

	QImage qImage( ( InternalImageType * ) sliceChunk.asValuePtr<InternalImageType>().getRawAddress().get(),
				   mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_Indexed8 );

	qImage.setColorTable( color::Color::adaptColorMapToImage(
							  m_ViewerCore->getColorHandler()->getColormapMap().at( image->getPropMap().getPropertyAs<std::string>( "lut" ) ), image ) );

	m_Painter->resetMatrix();

	if( image.get() != getWidgetSpecCurrentImage().get() ) {
		imgProps.viewPort =  QOrienationHandler::getViewPort( m_WidgetProperties, image, width(), height(),
							 m_PlaneOrientation );
	}

	if( !m_WidgetProperties.getPropertyAs<bool>( "mousePressedLeft" ) || m_WidgetProperties.getPropertyAs<bool>( "mousePressedRight" ) || m_WidgetProperties.getPropertyAs<bool>( "zoomEvent" ) ) {
		recalculateTranslation();
	}

	imgProps.viewPort[2] += m_WidgetProperties.getPropertyAs<float>( "translationX" );
	imgProps.viewPort[3] += m_WidgetProperties.getPropertyAs<float>( "translationY" );
	m_Painter->setTransform( QOrienationHandler::getTransform( imgProps.viewPort, image, width(), height(), m_PlaneOrientation ) );

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

bool QImageWidgetImplementation::isInViewPort( const ViewPortType &viewPort, QMouseEvent *e ) const
{
	return ( e->x() > viewPort[2] && e->x() < ( viewPort[2] + viewPort[4] )
			 && e->y() > viewPort[3] && e->y() < ( viewPort[3] + viewPort[5] )
		   );
}


void QImageWidgetImplementation::emitMousePressEvent( QMouseEvent *e )
{
	boost::shared_ptr<ImageHolder> image = getWidgetSpecCurrentImage();
	ImageProperties &imgProps = m_ImageProperties.at( image );

	if( isInViewPort( imgProps.viewPort, e ) ) {
		uint16_t slice = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>( "voxelCoords" ), image, m_PlaneOrientation )[2];
		util::ivector4 coords = QOrienationHandler::convertWindow2VoxelCoords( imgProps.viewPort, m_WidgetProperties, image, e->x(), e->y(), slice, m_PlaneOrientation );
		physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( coords ) );
	}
}

void QImageWidgetImplementation::paintCrosshair() const
{
	const boost::shared_ptr< ImageHolder > image = getWidgetSpecCurrentImage();
	const ImageProperties &imgProps = m_ImageProperties.at( image );
	std::pair<size_t, size_t> coords = QOrienationHandler::convertVoxel2WindowCoords( imgProps.viewPort, m_WidgetProperties, getWidgetSpecCurrentImage(), m_PlaneOrientation  );
	size_t border = 500;

	const QLine xline1( coords.first, -border , coords.first, coords.second - 15 );
	const QLine xline2( coords.first, coords.second + 15, coords.first, height() + border );

	const QLine yline1( -border, coords.second, coords.first - 15, coords.second );
	const QLine yline2( coords.first + 15, coords.second,  width() + border, coords.second  );

	QPen pen;
	pen.setColor( QColor( 255, 102, 0 ) );
	m_Painter->setOpacity( 1.0 );
	m_Painter->resetMatrix();
	m_Painter->resetTransform();
	m_Painter->setTransform( QOrienationHandler::getTransform( imgProps.viewPort, image, width(), height(), m_PlaneOrientation ) );
	m_Painter->scale( 1.0 / imgProps.viewPort[0], 1.0 / imgProps.viewPort[1] );
	m_Painter->translate( -imgProps.viewPort[2], -imgProps.viewPort[3] );
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
	m_WidgetProperties.setPropertyAs<uint16_t>( "currentTimeStep", physicalCoords[3] );
	BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) {
		image.second->getPropMap().setPropertyAs<util::fvector4>( "physicalCoords", physicalCoords );
		image.second->getPropMap().setPropertyAs<util::ivector4>( "voxelCoords", image.second->getISISImage()->getIndexFromPhysicalCoords( physicalCoords ) );
	}
	update();
}

bool QImageWidgetImplementation::lookAtVoxelCoords( const isis::util::ivector4 &voxelCoords )
{
	m_WidgetProperties.setPropertyAs<uint16_t>( "currentTimeStep", voxelCoords[3] );
	BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) {
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

void QImageWidgetImplementation::updateScene( bool center )
{
	update();
}

std::string QImageWidgetImplementation::getWidgetName() const
{
	return windowTitle().toStdString();
}

void QImageWidgetImplementation::setWidgetName( const std::string &wName )
{
	setWindowTitle( QString( wName.c_str() ) );
	m_WidgetProperties.setPropertyAs<std::string>( "widgetName", wName );
}

}
}
} //end namespace
