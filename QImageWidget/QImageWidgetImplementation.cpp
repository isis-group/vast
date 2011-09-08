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
    m_Painter = new QPainter();
    

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
    m_Painter->begin(this);
    BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
	if( image.second->getPropMap().getPropertyAs<bool>("isVisible") ) {
	    paintImage( image.second );
	} 
    }
    paintCrosshair();
    m_Painter->end();

}

void QImageWidgetImplementation::paintImage(boost::shared_ptr< ImageHolder > image)
{
    
    util::ivector4 mappedSizeAligned = QOrienationHandler::mapCoordsToOrientation( image->getPropMap().getPropertyAs<util::ivector4>("alignedSize32Bit"), image, m_PlaneOrientation);
    isis::data::MemChunk<InternalImageType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );
    
    m_MemoryHandler.fillSliceChunk( sliceChunk, image, m_PlaneOrientation );
    
    QImage qImage( (InternalImageType*) sliceChunk.asValuePtr<InternalImageType>().getRawAddress().lock().get(), 
		   mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_Indexed8 );
    qImage.setColorCount(512);
    qImage.setColorTable(Color::getColorTable( m_LutType ));
    
    m_Painter->setTransform( QOrienationHandler::getTransform( m_WidgetProperties, image, width(), height(), m_PlaneOrientation ) );
    m_Painter->setOpacity( image->getPropMap().getPropertyAs<float>("opacity") );
    m_Painter->drawImage(0,0, qImage);
}


void QImageWidgetImplementation::mousePressEvent(QMouseEvent* e)
{
    QWidget::mousePressEvent(e);

    std::pair<size_t, size_t> coords = QOrienationHandler::convertWindow2VoxelCoords( m_WidgetProperties, m_ViewerCore->getCurrentImage(), e->x(), e->y(), m_PlaneOrientation);
    m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<util::ivector4>("voxelCoords", util::ivector4(coords.first, coords.first, coords.first));
    update();
}

void QImageWidgetImplementation::mouseMoveEvent(QMouseEvent* e)
{
    QWidget::mouseMoveEvent(e);
    std::pair<size_t, size_t> coords = QOrienationHandler::convertWindow2VoxelCoords( m_WidgetProperties, m_ViewerCore->getCurrentImage(), e->x(), e->y(), m_PlaneOrientation);
    m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<util::ivector4>("voxelCoords", util::ivector4(coords.first, coords.first, coords.first));
    update();
}


void QImageWidgetImplementation::paintCrosshair()
{
    std::pair<size_t, size_t> coords = QOrienationHandler::convertVoxel2WindowCoords( m_WidgetProperties, m_ViewerCore->getCurrentImage(), m_PlaneOrientation  );
    util::fvector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( m_ViewerCore->getCurrentImage()->getImageSize(), m_ViewerCore->getCurrentImage(), m_PlaneOrientation);
    util::fvector4 scalingAndOffset = m_WidgetProperties.getPropertyAs<util::fvector4>("scalingAndOffset");
    QLine xline(coords.first, 0, coords.first, mappedSize[1] );
    QLine yline(0, coords.second, mappedSize[0], coords.second );
    m_Painter->setPen( Qt::blue );
    m_Painter->drawLine(xline);
    m_Painter->drawLine(yline);
}



}}} //end namespace