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
 * Author: Erik Türke, tuerke@cbs.mpg.de
 *
 * QImageWidgetImplementation.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef QIMAGEWIDGETIMPLEMENTATION_HPP
#define QIMAGEWIDGETIMPLEMENTATION_HPP

#include <QImage>
#include <QWidget>
#include <QPainter>
#include <QtGui>
#include "widgetinterface.h"
#include "qviewercore.hpp"
#include "QOrientationHandler.hpp"
#include "color.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{

class QImageWidgetImplementation : public QWidget, public WidgetInterface
{
	Q_OBJECT
	struct ImageProperties {
		/**scaling, offset, size**/
		QOrientationHandler::ViewPortType viewPort;
	};
	typedef std::map<boost::shared_ptr<ImageHolder>, ImageProperties> ImagePropertiesMapType;

public:

	QImageWidgetImplementation( QViewerCore *core, QWidget *parent = 0, PlaneOrientation orientation = axial );
	QImageWidgetImplementation();

public Q_SLOTS:
	virtual bool hasOptionWidget() const { return false; };
	virtual QWidget *getOptionWidget() { return new QWidget(); }

	virtual unsigned short getNumberOfInstancesInEnsemble() const { return 3; }
	virtual void setEnableCrosshair( bool enable ) { m_ShowCrosshair = enable; }

	virtual void disconnectSignals();
	virtual void connectSignals();

	virtual void setup( QViewerCore *, QWidget *, PlaneOrientation );
	virtual void setZoom( float zoom );
	virtual void addImage( const boost::shared_ptr<ImageHolder> image );
	virtual bool removeImage( const boost::shared_ptr<ImageHolder> image );
	virtual void paintImage( boost::shared_ptr< ImageHolder > image );
	virtual void paintCrosshair() const;

	virtual void lookAtPhysicalCoords( const util::fvector3 &physicalCoords );
	virtual void updateScene();
	virtual void setInterpolationType( InterpolationType interType ) { m_InterpolationType = interType; }
	virtual void setShowLabels( bool show ) { m_ShowLabels = show; m_Border = show ? 18 : 0; }
	virtual void setMouseCursorIcon( QIcon icon );
	virtual void setCrossHairColor( QColor color ) { m_CrosshairColor = color; }
	virtual void setCrossHairWidth( int width ) { m_CrosshairWidth = width; }

	virtual std::string getWidgetIdent() const { return std::string( "qt4_plane_widget" ); }
	virtual std::string getWidgetName() const { return std::string( "Simple plane widget" ); }
	virtual void keyPressEvent( QKeyEvent *e );

	virtual void dragEnterEvent( QDragEnterEvent * );
	virtual void dropEvent( QDropEvent * );

protected:
	void paintEvent( QPaintEvent *event );

	virtual void wheelEvent( QWheelEvent *e );
	virtual void mousePressEvent( QMouseEvent *e );
	virtual void mouseReleaseEvent( QMouseEvent *e );
	virtual void mouseMoveEvent( QMouseEvent *e );


Q_SIGNALS:
	void redraw();
	void physicalCoordsChanged( util::fvector3 );
	void zoomChanged( float zoomFactor );


private:
	QVBoxLayout *m_Layout;

	ImagePropertiesMapType m_ImageProperties;

	void recalculateTranslation();
	void showLabels() const ;

	boost::shared_ptr<ImageHolder> getWidgetSpecCurrentImage() const;

	void commonInit();
	util::PropertyMap m_WidgetProperties;
	QPainter *m_Painter;
	QColor m_CrosshairColor;
	InternalImageType m_InterpolationType;
	bool m_ShowLabels;
	unsigned short m_Border;
	bool m_LeftMouseButtonPressed;
	bool m_RightMouseButtonPressed;
	bool m_ShowScalingOffset;
	bool m_ShowCrosshair;

	float translationX;
	float translationY;
	float currentZoom;
	int m_CrosshairWidth;

	util::fvector3 mouseCoords2PhysCoords( const int &x, const int &y );

	std::pair<int, int> m_StartCoordsPair;

};

}
}
} // end namespace


#endif