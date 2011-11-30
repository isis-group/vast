#ifndef QIMAGEWIDGETIMPLEMENTATION_HPP
#define QIMAGEWIDGETIMPLEMENTATION_HPP

#include <QImage>
#include <QWidget>
#include <QPainter>
#include <QtGui>
#include "widgetinterface.hpp"
#include "qviewercore.hpp"
#include "QMemoryHandler.hpp"
#include "QOrientationHandler.hpp"
#include "color.hpp"

namespace isis
{

namespace viewer
{


class QImageWidgetImplementation : public QWidget, public WidgetInterface
{
	Q_OBJECT
	struct ImageProperties {
		/**scaling, offset, size**/
		isis::viewer::QOrienationHandler::ViewPortType viewPort;
	};
	typedef std::map<boost::shared_ptr<ImageHolder>, ImageProperties> ImagePropertiesMapType;

public:
	
	QImageWidgetImplementation( QViewerCore *core, QWidget *parent = 0, QWidget *share = 0, PlaneOrientation orienation = axial );
	QImageWidgetImplementation( QViewerCore *core, QWidget *parent = 0, PlaneOrientation orientation = axial );

	virtual WidgetInterface *createSharedWidget( QWidget *parent, PlaneOrientation orienation = axial );

public Q_SLOTS:

	virtual void setEnableCrosshair( bool enable ) { m_ShowCrosshair = enable; }
	
	virtual void setZoom( float zoom );
	virtual void addImage( const boost::shared_ptr<ImageHolder> image );
	virtual bool removeImage( const boost::shared_ptr<ImageHolder> image );
	virtual void paintImage( boost::shared_ptr< ImageHolder > image );
	virtual void paintCrosshair() const;

	virtual bool lookAtPhysicalCoords( const util::fvector4 &physicalCoords );
	virtual void updateScene();
	virtual void setInterpolationType( InterpolationType interType ) { m_InterpolationType = interType; }
	virtual void setShowLabels( bool show ) { m_ShowLabels = show; m_Border = show ? 18 : 0; }
	virtual void setMouseCursorIcon( QIcon icon );
	virtual void setCrossHairColor( QColor color) { m_CrosshairColor = color; }

	virtual std::string getWidgetName() const;
	virtual void setWidgetName( const std::string &wName );
	virtual void keyPressEvent( QKeyEvent *e);

protected:
	void paintEvent( QPaintEvent *event );

	virtual void wheelEvent( QWheelEvent *e );
	virtual void mousePressEvent( QMouseEvent *e );
	virtual void mouseReleaseEvent( QMouseEvent *e );
	virtual void mouseMoveEvent( QMouseEvent *e );


Q_SIGNALS:
	void redraw();
	void physicalCoordsChanged( util::fvector4 );
	void zoomChanged( float zoomFactor );


private:
	QVBoxLayout *m_Layout;
	
	ImagePropertiesMapType m_ImageProperties;

	void emitMousePressEvent( QMouseEvent *e );
	void recalculateTranslation();
	void showLabels() const ;

	boost::shared_ptr<ImageHolder> getWidgetSpecCurrentImage() const;

	QMemoryHandler m_MemoryHandler;

	void commonInit();
	util::PropertyMap m_WidgetProperties;
	QPainter *m_Painter;
	QColor m_CrosshairColor;
	InternalImageType m_InterpolationType;
	ScalingType m_ScalingType;
	bool m_ShowLabels;
	unsigned short m_Border;
	bool m_LeftMouseButtonPressed;
	bool m_RightMouseButtonPressed;
	bool m_ShowScalingOffset;
	bool m_ShowCrosshair;
	
	float translationX;
	float translationY;
	float currentZoom;
	
	std::pair<int, int> m_StartCoordsPair;

};


}
} // end namespace
#endif

