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

#include "memoryhandler.hpp"

namespace isis {
namespace viewer {
namespace widget {

QGeomWidget::QGeomWidget()
	: QWidget(),
	m_Painter( new QPainter( ) )
{

}

void QGeomWidget::setup ( QViewerCore* core, QWidget* parent , PlaneOrientation planeOrienation )
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
	m_RasterPhysicalCoords = true;
	connect(m_ViewerCore, SIGNAL( emitUpdateScene()), this, SLOT( updateScene()));
	
}

void QGeomWidget::resizeEvent ( QResizeEvent* event )
{

    QWidget::resizeEvent ( event );
}

void QGeomWidget::updateViewPort()
{
	//update viewport
	const float _width = m_BoundingBox[2] / _internal::rasteringFac;
	const float _height = m_BoundingBox[3] / _internal::rasteringFac;
	
	const float scalew = width() / _width;
	const  float scaleh = height() / _height;

	m_WindowViewPortScaling = std::min(scaleh, scalew);
	const float offsetW = (width() - (_width * m_WindowViewPortScaling)) / 2. ;
	const float offsetH = (height() - (_height * m_WindowViewPortScaling)) / 2.;
	m_ViewPort = util::fvector4( offsetW, offsetH, _width * m_WindowViewPortScaling, _height * m_WindowViewPortScaling );
}


void QGeomWidget::updateScene()
{
	update();
}

void QGeomWidget::addImage ( const ImageHolder::Pointer /*image*/ )
{
	m_BoundingBox = _internal::getPhysicalBoundingBox( getWidgetEnsemble()->getImageVector(), m_PlaneOrientation );
}

bool QGeomWidget::removeImage ( const ImageHolder::Pointer /*image*/ )
{
	m_BoundingBox = _internal::getPhysicalBoundingBox( getWidgetEnsemble()->getImageVector(), m_PlaneOrientation );
}


void QGeomWidget::paintEvent ( QPaintEvent* event )
{
	if( m_ViewerCore->hasImage() ) {
		m_Painter->begin(this);
		m_Painter->resetTransform();
		m_BoundingBox = _internal::getPhysicalBoundingBox( getWidgetEnsemble()->getImageVector(), m_PlaneOrientation );
		updateViewPort();
		m_Painter->setWindow( m_BoundingBox[0], m_BoundingBox[1], m_BoundingBox[2], m_BoundingBox[3] );
		m_Painter->setViewport( m_ViewPort[0], m_ViewPort[1], m_ViewPort[2], m_ViewPort[3] );
		BOOST_FOREACH( ImageHolder::Vector::const_reference image, getWidgetEnsemble()->getImageVector() )
		{
			if( image->getImageProperties().isVisible && image->getImageProperties().opacity != 0 ) {
				paintImage( image );
			}
		}
		
		if( m_ViewerCore->hasImage() ) {
			paintCrossHair();
		}

		m_Painter->end();
	}
}

void QGeomWidget::paintImage( const ImageHolder::Pointer image )
{
	m_Painter->setTransform(_internal::getTransform2ISISSpace(m_PlaneOrientation, m_BoundingBox) );

	m_Painter->setTransform( _internal::getQTransform( image, m_PlaneOrientation, m_LatchOrientation), true );

	const util::ivector4 mappedSizeAligned = mapCoordsToOrientation( image->getImageProperties().alignedSize32, image->getImageProperties().latchedOrientation, m_PlaneOrientation );
	isis::data::MemChunk<InternalImageType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );
	MemoryHandler::fillSliceChunk<InternalImageType>( sliceChunk, image, m_PlaneOrientation );

	QImage qImage( ( InternalImageType * ) sliceChunk.asValueArray<InternalImageType>().getRawAddress().get(),
					   mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_Indexed8 );
	
	qImage.setColorTable( image->getImageProperties().colorMap );
	m_Painter->setOpacity(image->getImageProperties().opacity);
	m_Painter->drawImage( 0,0, qImage );
}

void QGeomWidget::paintCrossHair() const
{
	const ImageHolder::Pointer image = m_ViewerCore->getCurrentImage();

	const util::fvector4 mappedCoords = (_internal::mapPhysicalCoords2Orientation(image->getImageProperties().physicalCoords, m_PlaneOrientation) ) * _internal::rasteringFac;
	short border = -15000;
	const QLine xline1( mappedCoords[0], border, mappedCoords[0], mappedCoords[1] - 15 * _internal::rasteringFac );
	const QLine xline2( mappedCoords[0], mappedCoords[1] + 15 * _internal::rasteringFac, mappedCoords[0], height() - border  );

	const QLine yline1( border, mappedCoords[1], mappedCoords[0] - 15 * _internal::rasteringFac, mappedCoords[1] );
	const QLine yline2( mappedCoords[0] + 15 * _internal::rasteringFac, mappedCoords[1],  width() - border, mappedCoords[1]  );

	QPen pen;
	pen.setColor( m_CrosshairColor );
	pen.setWidth( 1 );
	pen.setCosmetic(true);
	m_Painter->resetTransform();
	m_Painter->setWindow( m_BoundingBox[0], m_BoundingBox[1], m_BoundingBox[2], m_BoundingBox[3] );
	m_Painter->setViewport( m_ViewPort[0], m_ViewPort[1], m_ViewPort[2], m_ViewPort[3] );
	m_Painter->setTransform(_internal::getTransform2ISISSpace(m_PlaneOrientation, m_BoundingBox) );
	m_Painter->setOpacity( 1.0 );
	m_Painter->setPen( pen );
	m_Painter->drawLine( xline1 );
	m_Painter->drawLine( xline2 );
	m_Painter->drawLine( yline1 );
	m_Painter->drawLine( yline2 );
	m_Painter->drawPoint( mappedCoords[0], mappedCoords[1] );
}

void QGeomWidget::emitPhysicalCoordsFromMouseCoords ( const int& x, const int& y ) const
{
	if( m_ViewerCore->hasImage() ) {
		util::fvector4 physicalCoords = m_ViewerCore->getCurrentImage()->getImageProperties().physicalCoords;
		switch(m_PlaneOrientation) {
			case axial:
				physicalCoords[0] = ((width() - x) - m_ViewPort[0]) / m_WindowViewPortScaling + m_BoundingBox[0] / _internal::rasteringFac;
				physicalCoords[1] = (y - m_ViewPort[1]) / m_WindowViewPortScaling + m_BoundingBox[1] / _internal::rasteringFac;
				break;
			case sagittal:
				physicalCoords[1] = (x - m_ViewPort[0]) / m_WindowViewPortScaling + m_BoundingBox[0] / _internal::rasteringFac;
				physicalCoords[2] = (height() - y - m_ViewPort[1]) / m_WindowViewPortScaling + m_BoundingBox[1] / _internal::rasteringFac;
				break;
			case coronal:
				physicalCoords[0] = ((width() - x) - m_ViewPort[0]) / m_WindowViewPortScaling + m_BoundingBox[0] / _internal::rasteringFac;
				physicalCoords[2] = (height() - y - m_ViewPort[1]) / m_WindowViewPortScaling + m_BoundingBox[1] / _internal::rasteringFac;
				break;
			case not_specified:
				break;
		}
		
		if( m_RasterPhysicalCoords ) {
			physicalCoords = m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex(
						m_ViewerCore->getCurrentImage()->getISISImage()->getIndexFromPhysicalCoords( physicalCoords ) );
		}
		m_ViewerCore->physicalCoordsChanged(physicalCoords);
	}
}


void QGeomWidget::mousePressEvent ( QMouseEvent* e )
{
	if( e->button() == Qt::LeftButton ) {
		m_LeftMouseButtonPressed = true;
	}
	if( e->button() == Qt::RightButton ) {
		m_RightMouseButtonPressed = true;
	}
	emitPhysicalCoordsFromMouseCoords( e->x(), e->y() );
    
}

void QGeomWidget::mouseMoveEvent ( QMouseEvent* e )
{
	if( m_RightMouseButtonPressed || m_LeftMouseButtonPressed ) {
		emitPhysicalCoordsFromMouseCoords(e->x(), e->y() );
	}
}

void QGeomWidget::mouseReleaseEvent ( QMouseEvent* e )
{
	if( e->button() == Qt::LeftButton ) {
		m_LeftMouseButtonPressed = false;
	}
	if( e->button() == Qt::RightButton ) {
		m_RightMouseButtonPressed = false;
	}
}


void QGeomWidget::lookAtPhysicalCoords ( const util::fvector4& physicalCoords )
{

}

void QGeomWidget::setEnableCrosshair ( bool enable )
{

}

void QGeomWidget::setInterpolationType ( InterpolationType interpolation )
{

}

void QGeomWidget::setMouseCursorIcon ( QIcon )
{

}

void QGeomWidget::setZoom ( float zoom )
{

}



}}}// end namespace


isis::viewer::widget::WidgetInterface *loadWidget()
{
	return new isis::viewer::widget::QGeomWidget();
}

const isis::util::PropertyMap *getProperties()
{
	isis::util::PropertyMap *properties = new isis::util::PropertyMap();
	properties->setPropertyAs<std::string>( "widgetIdent", "qt4_geometrical_plane_widget" );
	properties->setPropertyAs<uint8_t>( "numberOfEntitiesInEnsemble", 3 );
	return properties;
}