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
	  m_Painter( new QPainter() ),
	  m_ShowLabels( false ),
	  m_Border( 0 )
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
	connect( this, SIGNAL( physicalCoordsChanged( util::fvector4 ) ), m_ViewerCore, SLOT( physicalCoordsChanged( util::fvector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitUpdateScene( ) ), this, SLOT( updateScene( ) ) );
	connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( lookAtPhysicalCoords( util::fvector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitZoomChanged( float ) ), this, SLOT( setZoom( float ) ) );
	connect( m_ViewerCore, SIGNAL( emitShowLabels( bool ) ), this, SLOT( setShowLabels( bool ) ) );
	connect( m_ViewerCore, SIGNAL( emitSetEnableCrosshair(bool)), this, SLOT( setEnableCrosshair(bool)));
	setAutoFillBackground( true );
	setPalette( QPalette( Qt::black ) );
	m_LeftMouseButtonPressed = false;
	m_RightMouseButtonPressed = false;
	m_ShowScalingOffset = false;

	m_WidgetProperties.setPropertyAs<bool>( "zoomEvent", false );
	m_WidgetProperties.setPropertyAs<float>( "zoomFactorIn", 1.5 );
	m_WidgetProperties.setPropertyAs<float>( "zoomFactorOut", 1.5 );
	m_WidgetProperties.setPropertyAs<float>( "maxZoom", 20 );
	m_WidgetProperties.setPropertyAs<float>( "minZoom", 1.0 );
	currentZoom = 1.0;
	translationX = 0.0;
	translationY = 0.0;
	m_ShowCrosshair = true;

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
	image->removeWidget( this );
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
	if( zoom <= m_WidgetProperties.getPropertyAs<float>( "maxZoom" ) && zoom >= m_WidgetProperties.getPropertyAs<float>( "minZoom" ) ) {
		m_WidgetProperties.setPropertyAs<bool>( "zoomEvent", true );
		currentZoom = zoom;
		update();
		m_WidgetProperties.setPropertyAs<bool>( "zoomEvent", false );
	}

}

void QImageWidgetImplementation::paintEvent( QPaintEvent *event )
{
	if( m_ImageVector.size() ) {
		m_Painter->begin( this );
		m_ImageProperties.at( getWidgetSpecCurrentImage() ).viewPort
		= QOrienationHandler::getViewPort( currentZoom,
										   getWidgetSpecCurrentImage(),
										   width(),
										   height(),
										   m_PlaneOrientation,
										   m_Border
										 );

		boost::shared_ptr<ImageHolder> cImage =  getWidgetSpecCurrentImage();
		//painting all anatomical images
		BOOST_FOREACH( ImageVectorType::const_reference image, m_ImageVector ) {
			if( image.get() != cImage.get()
				&& image->isVisible
				&& image->imageType == ImageHolder::anatomical_image ) {
				paintImage( image );
			}
		}

		if( cImage->imageType == ImageHolder::anatomical_image
			&& cImage->isVisible ) {
			paintImage( cImage );
		}

		//painting the zmaps
		BOOST_FOREACH( ImageVectorType::const_reference image, m_ImageVector ) {
			if( image.get() != cImage.get()
				&& image->isVisible
				&& image->imageType == ImageHolder::z_map ) {
				paintImage( image );
			}
		}

		if( cImage->imageType == ImageHolder::z_map
			&& cImage->isVisible ) {
			paintImage( cImage );
		}
		if( m_ShowCrosshair ) {
			paintCrosshair();
		}
		if( m_ShowLabels ) {
			showLabels();
		}

		if( m_ShowScalingOffset ) {
			m_Painter->resetMatrix();
			m_Painter->setFont( QFont( "Chicago", 10 ) );
			m_Painter->setPen( Qt::white );
			std::stringstream scalingOffset;
			boost::shared_ptr<ImageHolder> image = getWidgetSpecCurrentImage();
			scalingOffset << "Scaling: " << image->scaling
						  << " Offset: " << image->offset;
			m_Painter->drawText( 10, 30, scalingOffset.str().c_str() );
		}

		m_ShowScalingOffset = false;
		m_Painter->end();
	}

}

void QImageWidgetImplementation::recalculateTranslation()
{
	const boost::shared_ptr<ImageHolder > image = getWidgetSpecCurrentImage();
	const util::ivector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( image->getImageSize(), image, m_PlaneOrientation );
	const util::ivector4 mappedVoxelCoords = QOrienationHandler::mapCoordsToOrientation( image->voxelCoords, image, m_PlaneOrientation );
	const util::ivector4 signVec = QOrienationHandler::mapCoordsToOrientation( util::ivector4( 1, 1, 1, 1 ), image, m_PlaneOrientation, false, false );
	const util::ivector4 center = mappedSize / 2;
	const util::ivector4 diff = center - mappedVoxelCoords;
	const float transXConst = ( ( center[0] + 2 ) - mappedSize[0] / ( 2 * currentZoom ) );
	const float transYConst = ( ( center[1] + 2 ) - mappedSize[1] / ( 2 * currentZoom ) );
	const float transX = transXConst * ( ( float )diff[0] / ( float )center[0] ) * signVec[0];
	const float transY = transYConst * ( ( float )diff[1] / ( float )center[1] ) * signVec[1];
	const ViewPortType viewPort = m_ImageProperties.at( image ).viewPort;
	translationX = transX * viewPort[0] ;
	translationY = transY * viewPort[1] ;
}


void QImageWidgetImplementation::paintImage( boost::shared_ptr< ImageHolder > image )
{
	ImageProperties &imgProps = m_ImageProperties.at( image );

	switch( m_InterpolationType ) {
	case 0:
		m_Painter->setRenderHint( QPainter::NonCosmeticDefaultPen, true );
		break;
	case 1:
		m_Painter->setRenderHint( QPainter::SmoothPixmapTransform, true );
		break;
	}

	const util::ivector4 mappedSizeAligned = QOrienationHandler::mapCoordsToOrientation( image->alignedSize32, image, m_PlaneOrientation );

	isis::data::MemChunk<InternalImageType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );

	m_MemoryHandler.fillSliceChunk( sliceChunk, image, m_PlaneOrientation, image->voxelCoords[3] );

	QImage qImage( ( InternalImageType * ) sliceChunk.asValuePtr<InternalImageType>().getRawAddress().get(),
				   mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_Indexed8 );


	m_Painter->resetMatrix();

	if( image.get() != getWidgetSpecCurrentImage().get() ) {
		imgProps.viewPort =  QOrienationHandler::getViewPort( currentZoom, image, width(), height(),
							 m_PlaneOrientation, m_Border );
	}

	if( !m_LeftMouseButtonPressed || m_RightMouseButtonPressed || m_WidgetProperties.getPropertyAs<bool>( "zoomEvent" ) ) {
		recalculateTranslation();
	}

	imgProps.viewPort[2] += translationX;
	imgProps.viewPort[3] += translationY;

	m_Painter->setTransform( QOrienationHandler::getTransform( imgProps.viewPort, image, width(), height(), m_PlaneOrientation ) );

	m_Painter->setOpacity( image->opacity );

	qImage.setColorTable( color::Color::adaptColorMapToImage(
							  m_ViewerCore->getColorHandler()->getColormapMap().at( image->lut ), image ) );

	m_Painter->drawImage( 0, 0, qImage );

	m_Painter->resetMatrix();
	m_Painter->fillRect( imgProps.viewPort[4] + imgProps.viewPort[2], -1, width(), height(), Qt::black );
	m_Painter->fillRect( -1, imgProps.viewPort[5] + imgProps.viewPort[3], width(), height(), Qt::black );
	m_Painter->fillRect( 0, -1, imgProps.viewPort[2], height(), Qt::black );
}


void QImageWidgetImplementation::mousePressEvent( QMouseEvent *e )
{
	if( e->button() == Qt::RightButton ) {
		m_RightMouseButtonPressed = true;
	} else if ( e->button() == Qt::LeftButton ) {
		m_LeftMouseButtonPressed = true;
	}

	if( m_LeftMouseButtonPressed && m_RightMouseButtonPressed ) {
		m_StartCoordsPair.first = e->x();
		m_StartCoordsPair.second = e->y();
	}

	emitMousePressEvent( e );
}

void QImageWidgetImplementation::mouseMoveEvent( QMouseEvent *e )
{
	if( m_RightMouseButtonPressed && m_LeftMouseButtonPressed ) {
		boost::shared_ptr<ImageHolder> image = getWidgetSpecCurrentImage();
		const double offset =  ( m_StartCoordsPair.second - e->y() ) / ( float )height() * image->extent;
		const double scaling = 1.0 - ( m_StartCoordsPair.first - e->x() ) / ( float )width() * 5;
		image->offset = offset < 0 ? 0 : offset;
		image->scaling = scaling < 0.0 ? 0.0 : scaling;
		m_ShowScalingOffset = true;
		m_ViewerCore->updateScene();
	} else if( m_RightMouseButtonPressed || m_LeftMouseButtonPressed ) {
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

	uint16_t slice = QOrienationHandler::mapCoordsToOrientation( image->voxelCoords, image, m_PlaneOrientation )[2];
	util::ivector4 coords = QOrienationHandler::convertWindow2VoxelCoords( imgProps.viewPort, m_WidgetProperties, image, e->x(), e->y(), slice, m_PlaneOrientation );
	physicalCoordsChanged( image->getISISImage()->getPhysicalCoordsFromIndex( coords ) );
}

void QImageWidgetImplementation::showLabels() const
{
	m_Painter->resetMatrix();
	m_Painter->setFont( QFont( "Chicago", 13 ) );

	switch( m_PlaneOrientation ) {
	case axial:
		m_Painter->setPen( QColor( 255, 0, 0 ) );
		m_Painter->drawText( 0, height() / 2 + 7, "L" );
		m_Painter->drawText( width() - 15, height() / 2 + 7, "R" );
		m_Painter->setPen( QColor( 0, 255, 0 ) );
		m_Painter->drawText( width() / 2 - 7, 15, "A" );
		m_Painter->drawText( width() / 2 - 7, height() - 2, "P" );
		break;
	case sagittal:
		m_Painter->setPen( QColor( 0, 255, 0 ) );
		m_Painter->drawText( 0, height() / 2 + 7, "A" );
		m_Painter->drawText( width() - 15, height() / 2 + 7, "P" );
		m_Painter->setPen( QColor( 0, 0, 255 ) );
		m_Painter->drawText( width() / 2 - 7, 15, "S" );
		m_Painter->drawText( width() / 2 - 7, height() - 2, "I" );
		break;
	case coronal:
		m_Painter->setPen( QColor( 255, 0, 0 ) );
		m_Painter->drawText( 0, height() / 2 + 10, "L" );
		m_Painter->drawText( width() - 15, height() / 2 + 7, "R" );
		m_Painter->setPen( QColor( 0, 0, 255 ) );
		m_Painter->drawText( width() / 2 - 7, 15, "S" );
		m_Painter->drawText( width() / 2 - 7, height() - 2, "I" );
		break;
	}



}


void QImageWidgetImplementation::paintCrosshair() const
{
	const boost::shared_ptr< ImageHolder > image = getWidgetSpecCurrentImage();
	const ImageProperties &imgProps = m_ImageProperties.at( image );
	std::pair<int, int> coords = QOrienationHandler::convertVoxel2WindowCoords( imgProps.viewPort, image, m_PlaneOrientation );
#warning adopt border
	short border = -5000; 
	
	const QLine xline1( coords.first, border, coords.first, coords.second - 15 );
	const QLine xline2( coords.first, coords.second + 15, coords.first, height() - border  );

	const QLine yline1( border, coords.second, coords.first - 15, coords.second );
	const QLine yline2( coords.first + 15, coords.second,  width() - border, coords.second  );

	QPen pen;
	pen.setColor( QColor( 255, 102, 0 ) );
	m_Painter->setOpacity( 1.0 );
	m_Painter->setTransform( QOrienationHandler::getTransform( imgProps.viewPort, image, width(), height(), m_PlaneOrientation ) );
	m_Painter->scale( 1.0 / imgProps.viewPort[0], 1.0 / imgProps.viewPort[1] );
	m_Painter->translate( -imgProps.viewPort[2], -imgProps.viewPort[3] );
	m_Painter->setPen( pen );
	m_Painter->drawLine( xline1 );
	m_Painter->drawLine( xline2 );
	m_Painter->drawLine( yline1 );
	m_Painter->drawLine( yline2 );
	pen.setWidth( 2 );
	m_Painter->drawPoint( coords.first, coords.second );


}

bool QImageWidgetImplementation::lookAtPhysicalCoords( const isis::util::fvector4 &physicalCoords )
{
	BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) {
		image.second->physicalCoords = physicalCoords;
		image.second->voxelCoords = image.second->getISISImage()->getIndexFromPhysicalCoords( physicalCoords );
	}
	update();
}

void QImageWidgetImplementation::mouseReleaseEvent( QMouseEvent *e )
{
	if( e->button() == Qt::RightButton ) {
		m_RightMouseButtonPressed = false;
	}

	if ( e->button() == Qt::LeftButton ) {
		m_LeftMouseButtonPressed = false;
	}
}

void QImageWidgetImplementation::wheelEvent( QWheelEvent *e )
{
	float oldZoom = currentZoom;

	if ( e->delta() < 0 ) {
		oldZoom /= m_WidgetProperties.getPropertyAs<float>( "zoomFactorOut" );
	} else {
		oldZoom *= m_WidgetProperties.getPropertyAs<float>( "zoomFactorIn" );

	}
	if( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "propagateZooming" ) ) {
		zoomChanged( oldZoom );
	} else {
		setZoom( oldZoom );
	}

}

void QImageWidgetImplementation::updateScene()
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
