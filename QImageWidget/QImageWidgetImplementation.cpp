#include "QImageWidgetImplementation.hpp"


namespace isis {
namespace viewer {
namespace qt {


QImageWidgetImplementation::QImageWidgetImplementation(QViewerCore* core, QWidget* parent, PlaneOrientation orientation)
    : QWidget( parent ),
    WidgetImplenentationBase( core, parent, orientation )
{
    m_Painter = new QPainter( this );
}


QImageWidgetImplementation::QImageWidgetImplementation(QViewerCore* core, QWidget* parent, QWidget* share, PlaneOrientation orienation)
    : QWidget( parent, share ),
    WidgetImplenentationBase( core, parent, orienation )
{
    m_Painter = new QPainter( this );
}



WidgetImplenentationBase *QImageWidgetImplementation::createSharedWidget( QWidget *parent, PlaneOrientation orientation )
{
	return new QGLWidgetImplementation( m_ViewerCore, parent, this, orientation );
}

}}} //end namespace