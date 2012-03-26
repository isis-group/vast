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

void QGeomWidget::updateScene()
{

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