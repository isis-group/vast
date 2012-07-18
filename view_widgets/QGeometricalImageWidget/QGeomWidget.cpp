/****************************************************************
 *
 * <Copyright information>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Author: Erik Tuerke, tuerke@cbs.mpg.de
 *
 * QGeomWidget.cpp
 *
 * Description:
 *
 *  Created on: Mar 26, 2012
 *      Author: tuerke
 ******************************************************************/

#include "QGeomWidget.hpp"
#include "geomhandler.hpp"
#include "geometrical.hpp"
#include <Adapter/qmatrixconversion.hpp>

#include "memoryhandler.hpp"
#include "uicore.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{

QGeomWidget::QGeomWidget()
	: QWidget(),
	  m_Painter( new QPainter( ) )
{}

void QGeomWidget::setup ( QViewerCore *core, QWidget *parent , PlaneOrientation planeOrienation )
{
	isis::viewer::widget::WidgetInterface::setup ( core , parent , planeOrienation );
	setParent( parent );
	m_Layout = new QVBoxLayout( parent );
	m_Layout->addWidget( this );
	m_Layout->setMargin( 0 );
	setFocus();
	setAutoFillBackground( true );
	setPalette( QPalette( Qt::black ) );
	setAcceptDrops( true );
	m_CrosshairColor = QColor( 255, 102, 0 );
	m_LatchOrientation = false;
	m_LeftMouseButtonPressed = false;
	m_RightMouseButtonPressed = false;
	m_ZoomEvent = false;
	m_RasterPhysicalCoords = true;
	m_Zoom = 1.;
	m_Border = 0;
	m_ShowLabels = false;
	m_ShowCrosshair = true;
	m_ShowScalingOffset = false;


}

void QGeomWidget::disconnectSignals()
{
	disconnect( m_ViewerCore, SIGNAL( emitUpdateScene() ), this, SLOT( updateScene() ) );
	disconnect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector3 ) ), this, SLOT( updateScene() ) );
	disconnect( m_ViewerCore, SIGNAL( emitZoomChanged( float ) ), this, SLOT( setZoom( float ) ) );
	disconnect( m_ViewerCore, SIGNAL( emitShowLabels( bool ) ), this, SLOT( setShowLabels( bool ) ) );
	disconnect( m_ViewerCore, SIGNAL( emitSetEnableCrosshair( bool ) ), this, SLOT( setEnableCrosshair( bool ) ) );
	m_ViewerCore->emitImageContentChanged.disconnect( boost::bind( &QGeomWidget::updateScene, this ) );
}

void QGeomWidget::connectSignals()
{
	connect( m_ViewerCore, SIGNAL( emitUpdateScene() ), this, SLOT( updateScene() ) );
	connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector3 ) ), this, SLOT( updateScene() ) );
	connect( m_ViewerCore, SIGNAL( emitZoomChanged( float ) ), this, SLOT( setZoom( float ) ) );
	connect( m_ViewerCore, SIGNAL( emitShowLabels( bool ) ), this, SLOT( setShowLabels( bool ) ) );
	connect( m_ViewerCore, SIGNAL( emitSetEnableCrosshair( bool ) ), this, SLOT( setEnableCrosshair( bool ) ) );
	m_ViewerCore->emitImageContentChanged.connect( boost::bind( &QGeomWidget::updateScene, this ) );
}


void QGeomWidget::resizeEvent ( QResizeEvent *event )
{
	QWidget::resizeEvent ( event );
}

void QGeomWidget::updateViewPort()
{
	//update viewport
	const float _width = m_BoundingBox[2] / _internal::rasteringFac;
	const float _height = m_BoundingBox[3] / _internal::rasteringFac;

	const float scalew = ( width() - 2 * m_Border ) / _width;
	const  float scaleh = ( height() - 2 * m_Border ) / _height;

	m_WindowViewPortScaling = std::min( scaleh, scalew );
	const float offsetW = ( width() - ( _width * m_WindowViewPortScaling ) ) / 2. ;
	const float offsetH = ( height() - ( _height * m_WindowViewPortScaling ) ) / 2.;
	m_ViewPort = util::fvector4( offsetW,
								 offsetH,
								 ( _width * m_WindowViewPortScaling ),
								 ( _height * m_WindowViewPortScaling ) );
}


void QGeomWidget::updateScene()
{
	update();
}

void QGeomWidget::addImage ( const ImageHolder::Pointer image )
{
	if( m_ImageComponentsMap.find( image ) == m_ImageComponentsMap.end() ) {
		m_ImageComponentsMap.insert( std::make_pair<ImageHolder::Pointer, ImageComponent>( image, ImageComponent() ) );
		m_ImageComponentsMap.at( image ).transform = _internal::getQTransform( image, m_PlaneOrientation, m_LatchOrientation );
	} else {
		LOG( Dev, warning ) << "Trying to add image " << image->getImageProperties().filePath
							<< " to widget of type " << getWidgetIdent()
							<< ". But this widget already owns this image.";
	}
}

bool QGeomWidget::removeImage ( const ImageHolder::Pointer image )
{
	const ImageComponentsMapType::iterator iter = m_ImageComponentsMap.find( image );

	if( iter != m_ImageComponentsMap.end() ) {
		m_ImageComponentsMap.erase( iter );
		return true;
	} else {
		LOG( Dev, warning ) << "Trying to remove image " << image->getImageProperties().filePath
							<< " from widget of type " << getWidgetIdent()
							<< ". But it seems this image has no such image.";
		return false;
	}
}


void QGeomWidget::paintEvent ( QPaintEvent* /*event*/ )
{
	if( m_ViewerCore->hasImage() ) {
		m_Painter->begin( this );

		switch( m_InterpolationType ) {
		case nn:
			m_Painter->setRenderHint( QPainter::NonCosmeticDefaultPen, true );
			break;
		case lin:
			m_Painter->setRenderHint( QPainter::SmoothPixmapTransform, true );
			break;
		}

		m_Painter->resetTransform();

		if( m_ViewerCore->getSettings()->getPropertyAs<bool>( "latchSingleImage" ) ) {
			m_LatchOrientation = getWidgetEnsemble()->getImageVector().size() == 1;
		} else {
			m_LatchOrientation = false;
		}

		//bounding box calculation
		m_BoundingBox = _internal::getPhysicalBoundingBox( getWidgetEnsemble()->getImageVector(), m_PlaneOrientation );
		_internal::zoomBoundingBox( m_BoundingBox,
									m_Translation,
									m_ViewerCore->getCurrentImage()->getImageProperties().physicalCoords,
									m_Zoom, m_PlaneOrientation,
									( m_RightMouseButtonPressed && !m_LeftMouseButtonPressed )
									|| m_ZoomEvent
									|| ( !m_RightMouseButtonPressed && !m_LeftMouseButtonPressed  ) );

		if( m_Zoom > 1 ) {
			m_BoundingBox[0] -= m_Translation[0];
			m_BoundingBox[1] -= m_Translation[1];
		}

		m_ZoomEvent = false;

		updateViewPort();
		m_Painter->setWindow( m_BoundingBox[0], m_BoundingBox[1], m_BoundingBox[2], m_BoundingBox[3] );
		m_Painter->setViewport( m_ViewPort[0], m_ViewPort[1], m_ViewPort[2], m_ViewPort[3] );
		BOOST_FOREACH( ImageHolder::Vector::const_reference image, getWidgetEnsemble()->getImageVector() ) {
			if( image->getImageProperties().isVisible && image->getImageProperties().opacity != 0 ) {
				paintImage( image );
			}
		}

		if( m_ViewerCore->hasImage() && m_ShowCrosshair ) {
			paintCrossHair();
		}

		if( m_ShowLabels ) {
			paintLabels();
		}

		if( m_ShowScalingOffset ) {
			m_Painter->resetMatrix();
			m_Painter->setFont( QFont( "Chicago", 10 ) );
			m_Painter->setPen( Qt::red );
			std::stringstream scalingOffset;
			const ImageHolder::Pointer image = getWidgetEnsemble()->getImageVector().back();
			scalingOffset << "Scaling: " << image->getImageProperties().scaling
						  << " Offset: " << image->getImageProperties().offset;
			m_Painter->drawText( 10, 30, scalingOffset.str().c_str() );
		}

		m_ShowScalingOffset = false;
		m_Painter->end();
	}
}

void QGeomWidget::paintImage( const ImageHolder::Pointer image )
{
	m_Painter->setTransform( _internal::getTransform2ISISSpace( m_PlaneOrientation, m_BoundingBox ) );
	QTransform transform = _internal::getQTransform( image, m_PlaneOrientation, false );

	m_Painter->setTransform( transform, true );

	m_Painter->setOpacity( image->getImageProperties().opacity );
	const util::ivector4 mappedSizeAligned = mapCoordsToOrientation( image->getImageProperties().alignedSize32, image->getImageProperties().latchedOrientation, m_PlaneOrientation );

	if ( !image->getImageProperties().isRGB ) {
		isis::data::MemChunk<InternalImageType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );

		if( m_LatchOrientation ) {
			MemoryHandler::fillSliceChunk<InternalImageType>( sliceChunk, image, m_PlaneOrientation );
		} else {
			MemoryHandler::fillSliceChunkOriented<InternalImageType>( sliceChunk, image, m_PlaneOrientation );
		}

		QImage qImage( &sliceChunk.voxel<InternalImageType>( 0 ), mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_Indexed8 );

		qImage.setColorTable( image->getImageProperties().colorMap );
		m_Painter->drawImage( 0, 0, qImage );
	} else {

		isis::data::MemChunk<InternalImageColorType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );

		if( m_LatchOrientation ) {
			MemoryHandler::fillSliceChunk<InternalImageColorType>( sliceChunk, image, m_PlaneOrientation );
		} else {
			MemoryHandler::fillSliceChunkOriented<InternalImageColorType>( sliceChunk, image, m_PlaneOrientation );
		}

		QImage qImage( ( InternalImageType * ) &sliceChunk.voxel<InternalImageColorType>( 0 ), mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_RGB888 );
		m_Painter->drawImage( 0, 0, qImage );
	}
}

void QGeomWidget::paintCrossHair() const
{
	const ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();

	const util::fvector3 mappedCoords = ( _internal::mapPhysicalCoords2Orientation( image->getImageProperties().physicalCoords, m_PlaneOrientation ) ) * _internal::rasteringFac;

	const int border = std::numeric_limits<int>::min() / 4;

	const float gapx = m_BoundingBox[2] / 32 * sqrt( m_Zoom ) ;
	const float gapy = m_BoundingBox[3] / 32 * sqrt( m_Zoom ) ;

	const QLine xline1( mappedCoords[0], border, mappedCoords[0], mappedCoords[1] - gapx );

	const QLine xline2( mappedCoords[0], mappedCoords[1] + gapx, mappedCoords[0], height() - border  );

	const QLine yline1( border, mappedCoords[1], mappedCoords[0] - gapy, mappedCoords[1] );

	const QLine yline2( mappedCoords[0] + gapy, mappedCoords[1],  width() - border, mappedCoords[1]  );

	QPen pen;

	pen.setColor( m_CrosshairColor );

	pen.setWidth( 1 );

	pen.setCosmetic( true );

	m_Painter->resetTransform();

	m_Painter->setWindow( m_BoundingBox[0], m_BoundingBox[1], m_BoundingBox[2], m_BoundingBox[3] );

	m_Painter->setViewport( m_ViewPort[0], m_ViewPort[1], m_ViewPort[2], m_ViewPort[3] );

	m_Painter->setTransform( _internal::getTransform2ISISSpace( m_PlaneOrientation, m_BoundingBox ) );

	m_Painter->setOpacity( 1.0 );

	m_Painter->setPen( pen );

	m_Painter->drawLine( xline1 );

	m_Painter->drawLine( xline2 );

	m_Painter->drawLine( yline1 );

	m_Painter->drawLine( yline2 );

	pen.setWidth( 5 );

	m_Painter->drawPoint( mappedCoords[0], mappedCoords[1] );
}

void QGeomWidget::paintLabels() const
{
	m_Painter->resetMatrix();
	m_Painter->resetTransform();
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
	case not_specified:
		break;
	}

}


util::fvector3 QGeomWidget::getPhysicalCoordsFromMouseCoords ( const int &x, const int &y ) const
{
	if( m_ViewerCore->hasImage() ) {
		const ImageHolder::Pointer image =  m_ViewerCore->getCurrentImage();

		util::fvector3 physicalCoords = image->getImageProperties().physicalCoords;
		const util::ivector4 oldVoxelCoords = image->getImageProperties().voxelCoords;

		switch( m_PlaneOrientation ) {
		case axial:
			physicalCoords[0] = ( ( width() - x ) - m_ViewPort[0] ) / m_WindowViewPortScaling + m_BoundingBox[0] / _internal::rasteringFac;
			physicalCoords[1] = ( y - m_ViewPort[1] ) / m_WindowViewPortScaling + m_BoundingBox[1] / _internal::rasteringFac;
			break;
		case sagittal:
			physicalCoords[1] = ( x - m_ViewPort[0] ) / m_WindowViewPortScaling + m_BoundingBox[0] / _internal::rasteringFac;
			physicalCoords[2] = ( height() - y - m_ViewPort[1] ) / m_WindowViewPortScaling + m_BoundingBox[1] / _internal::rasteringFac;
			break;
		case coronal:
			physicalCoords[0] = ( ( width() - x ) - m_ViewPort[0] ) / m_WindowViewPortScaling + m_BoundingBox[0] / _internal::rasteringFac;
			physicalCoords[2] = ( height() - y - m_ViewPort[1] ) / m_WindowViewPortScaling + m_BoundingBox[1] / _internal::rasteringFac;
			break;
		case not_specified:
			break;
		}

		const util::ivector4 mappingVec = mapCoordsToOrientation( util::ivector4( 0, 1, 2, 3 ), image->getImageProperties().latchedOrientation, m_PlaneOrientation  );

		if( m_RasterPhysicalCoords ) {
			util::ivector4 voxelCoords = image->getISISImage()->getIndexFromPhysicalCoords( physicalCoords );
			voxelCoords[mappingVec[2]] = oldVoxelCoords[mappingVec[2]];
			physicalCoords = image->getISISImage()->getPhysicalCoordsFromIndex( voxelCoords );
		}

		return physicalCoords;
	}

	return util::fvector3();
}


void QGeomWidget::mousePressEvent ( QMouseEvent *e )
{
	if( e->button() == Qt::LeftButton && geometry().contains( e->pos() ) && QApplication::keyboardModifiers() == Qt::ControlModifier ) {
		QDrag *drag = new QDrag( this );
		QMimeData *mimeData = new QMimeData;
		mimeData->setText( getWidgetEnsemble()->getImageVector().back()->getImageProperties().filePath.c_str() );
		drag->setMimeData( mimeData );
		drag->setPixmap( QIcon( ":/common/vast.jpg" ).pixmap( 15 ) );
		drag->exec();
		return;
	}

	const util::fvector3 physicalCoords = getPhysicalCoordsFromMouseCoords( e->x(), e->y() );

	if( e->button() == Qt::LeftButton ) {
		m_LeftMouseButtonPressed = true;
	}

	if( e->button() == Qt::RightButton ) {
		m_RightMouseButtonPressed = true;
	}

	if( m_LeftMouseButtonPressed && m_RightMouseButtonPressed ) {
		m_StartCoordsPair.first = e->x();
		m_StartCoordsPair.second = e->y();
	}

	if( m_LeftMouseButtonPressed )  m_ViewerCore->onWidgetClicked( this, physicalCoords, Qt::LeftButton );

	if( m_RightMouseButtonPressed ) m_ViewerCore->onWidgetClicked( this, physicalCoords, Qt::RightButton );

}

void QGeomWidget::mouseMoveEvent ( QMouseEvent *e )
{
	if( m_RightMouseButtonPressed && m_LeftMouseButtonPressed ) {
		if( QApplication::keyboardModifiers() == Qt::ShiftModifier ) {
			BOOST_FOREACH( ImageHolder::Vector::const_reference image, m_ViewerCore->getImageVector() ) {
				const double offset =  ( m_StartCoordsPair.second - e->y() ) / ( float )height() * image->getImageProperties().extent;
				const double scaling = 1.0 - ( m_StartCoordsPair.first - e->x() ) / ( float )width() * 5;
				image->getImageProperties().offset = offset;
				image->getImageProperties().scaling = scaling < 0.0 ? 0.0 : scaling;
				image->getImageProperties().scalingMinMax = operation::NativeImageOps::getMinMaxFromScalingOffset( std::make_pair<double, double>( scaling, offset ), image );
				image->updateColorMap();
			}
		} else {
			boost::shared_ptr<ImageHolder> image = getWidgetEnsemble()->getImageVector().back();
			const double offset =  ( m_StartCoordsPair.second - e->y() ) / ( float )height() * image->getImageProperties().extent;
			const double scaling = 1.0 - ( m_StartCoordsPair.first - e->x() ) / ( float )width() * 5;
			image->getImageProperties().offset = offset;
			image->getImageProperties().scaling = scaling < 0.0 ? 0.0 : scaling;
			image->getImageProperties().scalingMinMax = operation::NativeImageOps::getMinMaxFromScalingOffset( std::make_pair<double, double>( scaling, offset ), image );
			image->updateColorMap();
		}

		m_ShowScalingOffset = true;
		m_ViewerCore->updateScene();
		m_ViewerCore->getUICore()->refreshUI();

	} else {
		const util::fvector3 physicalCoords = getPhysicalCoordsFromMouseCoords(  e->x(), e->y() );

		if( m_LeftMouseButtonPressed ) {
			m_ViewerCore->onWidgetMoved( this, physicalCoords, Qt::LeftButton );
		}

		if( m_RightMouseButtonPressed ) {
			m_ViewerCore->onWidgetMoved( this, physicalCoords, Qt::RightButton );
		}
	}

}

void QGeomWidget::mouseReleaseEvent ( QMouseEvent *e )
{
	if( e->button() == Qt::LeftButton ) {
		m_LeftMouseButtonPressed = false;
	}

	if( e->button() == Qt::RightButton ) {
		m_RightMouseButtonPressed = false;
	}
}

void QGeomWidget::wheelEvent ( QWheelEvent *e )
{
	float oldZoom = m_Zoom;

	if ( e->delta() < 0 ) {
		oldZoom /= 1.5;
	} else {
		oldZoom *= 1.5;

	}

	if( m_ViewerCore->getSettings()->getPropertyAs<bool>( "propagateZooming" ) ) {
		m_ViewerCore->zoomChanged( oldZoom );
	} else {
		setZoom( oldZoom );
	}
}


void QGeomWidget::lookAtPhysicalCoords ( const util::fvector3& /*physicalCoords*/ )
{

}


void QGeomWidget::setInterpolationType ( InterpolationType interpolation )
{
	m_InterpolationType = interpolation;
}

void QGeomWidget::setMouseCursorIcon ( QIcon icon )
{
	if( !icon.isNull() )  {
		setCursor( QCursor( icon.pixmap( 45, 45 ) ) );
	} else {
		setCursor( Qt::ArrowCursor );
	}
}

void QGeomWidget::setZoom ( float zoom )
{
	if( zoom >= 1 && zoom < 100 ) {
		m_ZoomEvent = true;
		m_Zoom = zoom;
		updateScene();
	}
}

void QGeomWidget::dragEnterEvent ( QDragEnterEvent *e )
{
	if( e->mimeData()->hasFormat( "text/plain" ) ) {
		bool hasImage = false;
		BOOST_FOREACH( ImageHolder::Vector::const_reference image, getWidgetEnsemble()->getImageVector() ) {
			if( image->getImageProperties().fileName == e->mimeData()->text().toStdString() ) {
				hasImage = true;
			}
		}

		if( !hasImage ) {
			e->acceptProposedAction();
		}
	}
}

void QGeomWidget::dropEvent ( QDropEvent *e )
{
	const std::string &text = e->mimeData()->text().toStdString();

	if( m_ViewerCore->getImageMap().find( text ) != m_ViewerCore->getImageMap().end() ) {
		const ImageHolder::Pointer image = m_ViewerCore->getImageMap()[text];
		WidgetEnsemble::Pointer myEnsemble;
		BOOST_FOREACH( WidgetEnsemble::Vector::reference ensemble, m_ViewerCore->getUICore()->getEnsembleList() ) {
			BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, *ensemble ) {
				if( ensembleComponent->getWidgetInterface() == this ) {
					myEnsemble = ensemble;
				}
			}
		}
		myEnsemble->addImage( image  );
		BOOST_FOREACH( WidgetEnsemble::Vector::reference ensemble, m_ViewerCore->getUICore()->getEnsembleList() ) {
			if( ensemble != myEnsemble ) {
				ensemble->removeImage( image );
			}
		}

		m_ViewerCore->setCurrentImage( image );
		m_ViewerCore->updateScene();
		m_ViewerCore->getUICore()->refreshUI();
	} else {
		m_ViewerCore->getUICore()->openFromDropEvent( e );
	}
}



}
}
}// end namespace

const QWidget *loadOptionWidget()
{
	return new QWidget();
}

isis::viewer::widget::WidgetInterface *loadWidget()
{
	return new isis::viewer::widget::QGeomWidget();
}

const isis::util::PropertyMap *getProperties()
{
	isis::util::PropertyMap *properties = new isis::util::PropertyMap();
	properties->setPropertyAs<std::string>( "widgetIdent", "qt4_geometrical_plane_widget" );
	properties->setPropertyAs<std::string>( "widgetName", "Geometrical plane widget" );
	properties->setPropertyAs<uint8_t>( "numberOfEntitiesInEnsemble", 3 );
	properties->setPropertyAs<bool>( "hasOptionWidget", false );
	return properties;
}