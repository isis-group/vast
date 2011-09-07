#include "QImageWidgetImplementation.hpp"
#include "QtWidgetCommon.hpp"
#include "QOrientationHandler.hpp"

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
    BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
	paintImage( image.second );
    }

}

void QImageWidgetImplementation::paintImage(boost::shared_ptr< ImageHolder > image)
{
    
    util::ivector4 mappedSizeAligned = QOrienationHandler::mapCoordsToOrientation( image->getImageProperties().alignedSize32Bit, image, m_PlaneOrientation);
    isis::data::MemChunk<InternalImageType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );
    
    m_MemoryHandler.fillSliceChunk( sliceChunk, image, 100, m_PlaneOrientation );
    
    QImage qImage( (InternalImageType*) sliceChunk.asValuePtr<InternalImageType>().getRawAddress().lock().get(), 
		   mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_Indexed8 );
    
    QVector<QRgb> colorTable;
    for (int i = 0; i < 256; i++) {
        colorTable.push_back(QColor(i, i, i).rgb());
    }
    
    
    qImage.setColorTable(colorTable);
    QPainter painter( this );
    util::fvector4 scalingAndOffset = QOrienationHandler::getScalingAndOffset(image, width(),height(),m_PlaneOrientation );
    QTransform transform;
    transform = transform.scale(scalingAndOffset[0], scalingAndOffset[1] );
    painter.setTransform(transform);
    painter.drawImage(scalingAndOffset[2],scalingAndOffset[3], qImage);
    
}



}}} //end namespace