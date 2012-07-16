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

namespace isis
{
namespace viewer
{
namespace widget
{

class QGeomWidget : public QWidget, public WidgetInterface
{
	Q_OBJECT

	struct ImageComponent {
		QTransform transform;
	};

	typedef std::map<ImageHolder::Pointer, ImageComponent> ImageComponentsMapType;

public:
	QGeomWidget();

	virtual void setup( QViewerCore *, QWidget *, PlaneOrientation );

	virtual std::string getWidgetIdent() const { return std::string( "qt4_geometrical_plane_widget" ); }
	virtual std::string getWidgetName() const { return std::string( "Geometrical widget" ); }
public Q_SLOTS:
	virtual void updateScene();
	virtual void lookAtPhysicalCoords ( const util::fvector3 &physicalCoords );
	virtual void addImage( const ImageHolder::Pointer image );
	virtual bool removeImage( const ImageHolder::Pointer image );
	virtual void setInterpolationType ( InterpolationType interpolation );
	virtual void setCrossHairColor( QColor color ) { m_CrosshairColor = color; }
	virtual void setMouseCursorIcon ( QIcon );
	virtual void setZoom ( float zoom );
	virtual void setShowLabels( bool show ) { m_ShowLabels = show; m_Border = show ? 18 : 0; }
	virtual void setEnableCrosshair( bool enable ) { m_ShowCrosshair = enable; }

	virtual void disconnectSignals();
	virtual void connectSignals();

	virtual void dragEnterEvent( QDragEnterEvent * );
	virtual void dropEvent( QDropEvent * );
protected:
	void paintEvent( QPaintEvent *event );
	void resizeEvent( QResizeEvent *event );
	void mousePressEvent( QMouseEvent *e );
	void mouseMoveEvent( QMouseEvent *e );
	void mouseReleaseEvent( QMouseEvent *e );
	void wheelEvent( QWheelEvent *e );

private:
	void paintImage( const ImageHolder::Pointer );
	void paintCrossHair() const;
	void paintLabels() const;
	void updateViewPort();

	bool m_LatchOrientation;
	bool m_RasterPhysicalCoords;
	bool m_LeftMouseButtonPressed;
	bool m_RightMouseButtonPressed;
	bool m_ZoomEvent;
	bool m_ShowLabels;
	bool m_ShowCrosshair;
	bool m_ShowScalingOffset;
	util::FixedVector<float, 2> m_Translation;
	float m_WindowViewPortScaling;

	util::fvector3 getPhysicalCoordsFromMouseCoords( const int &x, const int &y ) const;

	QPainter *m_Painter;
	QVBoxLayout *m_Layout;
	QColor m_CrosshairColor;
	util::fvector4 m_BoundingBox;
	util::fvector4 m_ViewPort;
	float m_Zoom;
	uint16_t m_Border;
	InterpolationType m_InterpolationType;
	std::pair<int, int> m_StartCoordsPair;

	ImageComponentsMapType m_ImageComponentsMap;
};


}
}
}// end namespac

#endif //VAST_QGEOMWIDGET_HPP