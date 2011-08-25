#include "QImageWidgetImplementation.hpp"


namespace isis {
namespace viewer {
namespace qt {


QImageWidgetImplementation::QImageWidgetImplementation(QViewerCore* core, QWidget* parent, PlaneOrientation orientation)
    : QWidget( parent ),
    WidgetImplementationBase( core, parent, orientation ),
    m_MemoryHandler( core )
{
   
    ( new QVBoxLayout( parent ) )->addWidget( this );
   
//     
//     m_Painter->drawImage(0,0, *image );
}


QImageWidgetImplementation::QImageWidgetImplementation(QViewerCore* core, QWidget* parent, QWidget* share, PlaneOrientation orienation)
    : QWidget( parent ),
    WidgetImplementationBase( core, parent, orienation ),
    m_MemoryHandler( core )
{
    ( new QVBoxLayout( parent ) )->addWidget( this );
//     m_Painter = new QPainter( this );
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
    size_t plane = m_ViewerCore->getCurrentImage()->getImageSize()[0] * m_ViewerCore->getCurrentImage()->getImageSize()[1];
    uint8_t *dataPtr = ( uint8_t * ) calloc( plane , sizeof( uint8_t ) );
//     uint8_t *dataPtr = static_cast<uint8_t *>( m_ViewerCore->getCurrentImage()->getImageWeakPointer().lock().get() );
    m_MemoryHandler.getSlice( m_ViewerCore->getCurrentImage(), 100, dataPtr );
    QImage image = QImage( (uchar*)dataPtr, m_ViewerCore->getCurrentImage()->getImageSize()[0],m_ViewerCore->getCurrentImage()->getImageSize()[1], QImage::Format_Indexed8 );
    QVector<QRgb> colorTable;
    for (int i = 0; i < 256; i++) {
        colorTable.push_back(QColor(i, i, i).rgb());
    }
    image.setColorTable(colorTable);
    QPainter painter( this );
    
    painter.drawImage(0,0, image.scaled(width(), height()) );

}




}}} //end namespace