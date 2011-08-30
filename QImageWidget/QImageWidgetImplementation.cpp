#include "QImageWidgetImplementation.hpp"
#include "QtWidgetCommon.hpp"

namespace isis {
namespace viewer {
namespace qt {


QImageWidgetImplementation::QImageWidgetImplementation(QViewerCore* core, QWidget* parent, PlaneOrientation orientation)
    : QWidget( parent ),
    WidgetImplementationBase( core, parent, orientation ),
    m_MemoryHandler( core )
{
    ( new QVBoxLayout( parent ) )->addWidget( this );
    commonInit();
}


QImageWidgetImplementation::QImageWidgetImplementation(QViewerCore* core, QWidget* parent, QWidget* share, PlaneOrientation orienation)
    : QWidget( parent ),
    WidgetImplementationBase( core, parent, orienation ),
    m_MemoryHandler( core )
{
    ( new QVBoxLayout( parent ) )->addWidget( this );
    commonInit();
}

void QImageWidgetImplementation::commonInit()
{
    setAutoFillBackground( true );
    setPalette( QPalette(Qt::black ) );

}


WidgetImplementationBase *QImageWidgetImplementation::createSharedWidget( QWidget *parent, PlaneOrientation orientation )
{
	return new QImageWidgetImplementation( m_ViewerCore, parent, this, orientation );
}


void QImageWidgetImplementation::addImage(const boost::shared_ptr< ImageHolder > image)
{

}

void QImageWidgetImplementation::setZoom(float zoom)
{

}

void QImageWidgetImplementation::paintEvent( QPaintEvent *event )
{
    size_t x = 0;
    size_t y = 0;
    util::ivector4 alignedSize = get32BitAlignedSize( m_ViewerCore->getCurrentImage()->getImageSize());
    switch( m_PlaneOrientation ) {
	case axial:
	    x = alignedSize[0];
	    y = alignedSize[1];
	    break;
	case sagittal:
	    x = alignedSize[1];
	    y = alignedSize[2];
	    break;
	case coronal:
	    x = alignedSize[0];
	    y = alignedSize[2];
	    break;
    }
    uint8_t *dataPtr = ( uint8_t * ) calloc( x*y , sizeof( uint8_t ) );
    m_MemoryHandler.getSlice( m_ViewerCore->getCurrentImage(), 100, dataPtr, m_PlaneOrientation, alignedSize );
    QImage image = QImage( (uchar*)dataPtr,x,y, QImage::Format_Indexed8 );
    QVector<QRgb> colorTable;
    for (int i = 0; i < 256; i++) {
        colorTable.push_back(QColor(i, i, i).rgb());
    }
    image.setColorTable(colorTable);
    QPainter painter( this );
    painter.drawImage(0,0, image );

}




}}} //end namespace