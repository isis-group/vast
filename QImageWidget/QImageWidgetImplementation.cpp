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
    m_LutType = Color::standard_grey_values;

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
	if( image.second->getPropMap().getPropertyAs<bool>("isVisible") ) {
	    paintImage( image.second );
	} 
    }

}

void QImageWidgetImplementation::paintImage(boost::shared_ptr< ImageHolder > image)
{
    
    util::ivector4 mappedSizeAligned = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>("alignedSize32Bit"), image, m_PlaneOrientation);
    isis::data::MemChunk<InternalImageType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );
    
    m_MemoryHandler.fillSliceChunk( sliceChunk, image, m_PlaneOrientation );
    
    QImage qImage( (InternalImageType*) sliceChunk.asValuePtr<InternalImageType>().getRawAddress().lock().get(), 
		   mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_Indexed8 );
    
    qImage.setColorTable(Color::getColorTable( m_LutType ));
    QPainter painter( this );
    
    painter.setTransform( QOrienationHandler::getTransform(image, width(), height(), m_PlaneOrientation ) );
    painter.setOpacity( image->getPropMap().getPropertyAs<float>("opacity") );
    painter.drawImage(0,0, qImage);
}


void QImageWidgetImplementation::mousePressEvent(QMouseEvent* e)
{
    QWidget::mousePressEvent(e);
    std::cout << e->x() << " : " << e->y() << std::endl;
    
    
}



}}} //end namespace