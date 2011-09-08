#ifndef QIMAGEWIDGETIMPLEMENTATION_HPP
#define QIMAGEWIDGETIMPLEMENTATION_HPP

#include <QImage>
#include <QWidget>
#include <QPainter>
#include <QtGui>
#include "WidgetImplementationBase.hpp"
#include "QViewerCore.hpp"
#include "QMemoryHandler.hpp"
#include "Color.hpp"

namespace isis
{

namespace viewer
{

namespace qt
{

class QImageWidgetImplementation : public QWidget,  public WidgetImplementationBase
{
	Q_OBJECT

public:
	QImageWidgetImplementation( QViewerCore *core, QWidget *parent = 0, QWidget *share = 0, PlaneOrientation orienation = axial );
	QImageWidgetImplementation( QViewerCore *core, QWidget *parent = 0, PlaneOrientation orientation = axial );

	virtual WidgetImplementationBase *createSharedWidget( QWidget *parent, PlaneOrientation orienation = axial );


public Q_SLOTS:

	virtual void setZoom( float zoom );
	virtual void addImage( const boost::shared_ptr<ImageHolder> image );

	virtual void paintImage( boost::shared_ptr< ImageHolder > image );
	virtual void paintCrosshair();

	virtual void mousePressEvent( QMouseEvent *e );
	virtual void mouseMoveEvent( QMouseEvent *e );

protected:
	void paintEvent( QPaintEvent *event );
Q_SIGNALS:
	void redraw();
	void voxelCoordsChanged( util::ivector4 );
	void physicalCoordsChanged( util::fvector4 );
	void zoomChanged( float zoomFactor );


private:
	QMemoryHandler m_MemoryHandler;
	Color::LookUpTableType m_LutType;
	void commonInit();
	util::PropertyMap m_WidgetProperties;
	QPainter *m_Painter;

};


}
}
} // end namespace
#endif

