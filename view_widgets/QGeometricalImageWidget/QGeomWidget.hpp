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
 * QGeomWidget.hpp
 *
 * Description:
 *
 *  Created on: Mar 26, 2012
 *      Author: tuerke
 ******************************************************************/
#ifndef VAST_QGEOMWIDGET_HPP
#define VAST_QGEOMWIDGET_HPP

#include <QImage>
#include <QWidget>
#include <QPainter>
#include <QtGui>
#include "widgetinterface.h"
#include "qviewercore.hpp"
#include "color.hpp"

namespace isis {
namespace viewer {
namespace widget {

class QGeomWidget : public QWidget, public WidgetInterface
{
	Q_OBJECT
public:
	QGeomWidget();

	virtual void setup( QViewerCore *, QWidget *, PlaneOrientation );

	virtual std::string getWidgetName() const { return std::string( "qt4_geometrical_plane_widget" ); }
	
	virtual void updateScene();
    virtual void lookAtPhysicalCoords ( const util::fvector4& physicalCoords );
	virtual void addImage( const ImageHolder::Pointer image );
	virtual bool removeImage( const ImageHolder::Pointer image );
    virtual void setEnableCrosshair ( bool enable );
    virtual void setInterpolationType ( InterpolationType interpolation );
    virtual void setMouseCursorIcon ( QIcon );
    virtual void setZoom ( float zoom );

protected:
	void paintEvent( QPaintEvent *event );
	void resizeEvent( QResizeEvent *event );
	
private:
	void paintImage( const ImageHolder::Pointer );
	
	QPainter *m_Painter;
	QVBoxLayout *m_Layout;
	util::fvector4 m_BoundingBox;
	util::fvector4 m_ViewPort;
};


}}}// end namespac

#endif //VAST_QGEOMWIDGET_HPP