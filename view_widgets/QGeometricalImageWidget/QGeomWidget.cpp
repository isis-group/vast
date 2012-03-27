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
	
}

void QGeomWidget::resizeEvent ( QResizeEvent* event )
{
	//update viewport
	const float _width = fabs( m_BoundingBox[2] - m_BoundingBox[0] );
	const float _height = fabs( m_BoundingBox[3] - m_BoundingBox[1] );
	
	const float scalew = width() / _width;
	const  float scaleh = height() / _height;
	
	const float scale = std::min(scaleh, scalew);
	m_ViewPort = util::fvector4( (width() - (_width * scale)) / 2.  , (height() - (_height * scale)) / 2., _width * scale, _height * scale );
    QWidget::resizeEvent ( event );
}


void QGeomWidget::updateScene()
{
	update();
}

void QGeomWidget::addImage ( const ImageHolder::Pointer /*image*/ )
{
	m_BoundingBox = _internal::getPhysicalBoundingBox( getWidgetEnsemble()->getImageList(), m_PlaneOrientation );
}

bool QGeomWidget::removeImage ( const ImageHolder::Pointer /*image*/ )
{
	m_BoundingBox = _internal::getPhysicalBoundingBox( getWidgetEnsemble()->getImageList(), m_PlaneOrientation );
}


void QGeomWidget::paintEvent ( QPaintEvent* event )
{
	m_Painter->begin(this);
	m_Painter->setViewport( m_ViewPort[0], m_ViewPort[1], m_ViewPort[2], m_ViewPort[3] );
	m_Painter->setWindow( m_BoundingBox[0], m_BoundingBox[1], m_BoundingBox[2], m_BoundingBox[3] );
	BOOST_FOREACH( ImageHolder::Vector::const_reference image, getWidgetEnsemble()->getImageList() )
	{
		paintImage( image );
	}

	

	m_Painter->end();
}

void QGeomWidget::paintImage( const ImageHolder::Pointer image )
{
	m_Painter->setTransform( _internal::getQTransform( image, m_PlaneOrientation) );

	
	const util::ivector4 mappedSizeAligned = mapCoordsToOrientation( image->getImageProperties().alignedSize32, image->getImageProperties().latchedOrientation, m_PlaneOrientation );
	isis::data::MemChunk<InternalImageType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );
	MemoryHandler::fillSliceChunk<InternalImageType>( sliceChunk, image, m_PlaneOrientation );

	
	QImage qImage( ( InternalImageType * ) sliceChunk.asValueArray<InternalImageType>().getRawAddress().get(),
					   mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_Indexed8 );
	qImage.setColorTable( image->getImageProperties().colorMap );
	m_Painter->drawImage( 0,0, qImage );
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