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
 * QImageWidgetImplementation.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "QImageWidgetImplementation.hpp"

#include "QOrientationHandler.hpp"
#include "uicore.hpp"

namespace isis
{
namespace viewer
{


QImageWidgetImplementation::QImageWidgetImplementation( QViewerCore *core, QWidget *parent, PlaneOrientation orientation )
    : QWidget( parent ),
      WidgetInterface( core, parent, orientation ),
      m_Layout( new QVBoxLayout( parent ) ),
      m_MemoryHandler( core ),
      m_Painter( new QPainter() ),
      m_ShowLabels( false )
{
    m_Layout->addWidget( this );
    m_Layout->setMargin( 0 );
    commonInit();
}


void QImageWidgetImplementation::commonInit()
{
    connect( this, SIGNAL( zoomChanged( float ) ), m_ViewerCore, SLOT( zoomChanged( float ) ) );
    connect( this, SIGNAL( physicalCoordsChanged( util::fvector4 ) ), m_ViewerCore, SLOT( physicalCoordsChanged( util::fvector4 ) ) );
    connect( m_ViewerCore, SIGNAL( emitUpdateScene( ) ), this, SLOT( updateScene( ) ) );
    connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( lookAtPhysicalCoords( util::fvector4 ) ) );
    connect( m_ViewerCore, SIGNAL( emitZoomChanged( float ) ), this, SLOT( setZoom( float ) ) );
    connect( m_ViewerCore, SIGNAL( emitShowLabels( bool ) ), this, SLOT( setShowLabels( bool ) ) );
    connect( m_ViewerCore, SIGNAL( emitSetEnableCrosshair( bool ) ), this, SLOT( setEnableCrosshair( bool ) ) );
    setAutoFillBackground( true );
    setPalette( QPalette( Qt::black ) );
    m_LeftMouseButtonPressed = false;
    m_RightMouseButtonPressed = false;
    m_ShowScalingOffset = false;

    m_WidgetProperties.setPropertyAs<bool>( "zoomEvent", false );
    m_WidgetProperties.setPropertyAs<float>( "zoomFactorIn", 1.5 );
    m_WidgetProperties.setPropertyAs<float>( "zoomFactorOut", 1.5 );
    m_WidgetProperties.setPropertyAs<float>( "maxZoom", 30 );
    m_WidgetProperties.setPropertyAs<float>( "minZoom", 1.0 );
    m_CrosshairWidth = 1;
    currentZoom = 1.0;
    translationX = 0.0;
    translationY = 0.0;
    m_ShowCrosshair = true;
    m_CrosshairColor = QColor( 255, 102, 0 );
    setAcceptDrops( true );
    setFocus();
}

void QImageWidgetImplementation::setMouseCursorIcon( QIcon icon )
{
    if( !icon.isNull() )  {
        setCursor( QCursor( icon.pixmap( 45, 45 ) ) );
    } else {
        setCursor( Qt::ArrowCursor );
    }
}

void QImageWidgetImplementation::addImage( const boost::shared_ptr< ImageHolder > image )
{
    ImageProperties imgProperties;
    imgProperties.viewPort = QOrientationHandler::ViewPortType();
    m_ImageProperties.insert( std::make_pair< boost::shared_ptr<ImageHolder> , ImageProperties >( image, imgProperties ) );
    m_ImageVector.push_back( image );
    image->addWidget( this );
    setFocus();
}

bool QImageWidgetImplementation::removeImage( const boost::shared_ptr< ImageHolder > image )
{
    image->removeWidget( this );
    m_ImageProperties.erase( image );
    ImageVectorType::iterator iter = std::find( m_ImageVector.begin(), m_ImageVector.end(), image );

    if( iter != m_ImageVector.end() ) {
        m_ImageVector.erase( iter );
    }

    return iter != m_ImageVector.end();
}

boost::shared_ptr< ImageHolder > QImageWidgetImplementation::getWidgetSpecCurrentImage() const
{
    if( std::find( m_ImageVector.begin(), m_ImageVector.end(), m_ViewerCore->getCurrentImage() ) != m_ImageVector.end() ) {
        return m_ViewerCore->getCurrentImage();
    }

    return m_ImageVector.front();

}

void QImageWidgetImplementation::setZoom( float zoom )
{
    if( zoom <= m_WidgetProperties.getPropertyAs<float>( "maxZoom" ) && zoom >= m_WidgetProperties.getPropertyAs<float>( "minZoom" ) ) {
        m_WidgetProperties.setPropertyAs<bool>( "zoomEvent", true );
        currentZoom = zoom;
        update();
        m_WidgetProperties.setPropertyAs<bool>( "zoomEvent", false );
    }

}

void QImageWidgetImplementation::paintEvent( QPaintEvent */*event*/ )
{
    if( m_ImageVector.size() ) {
        m_Painter->begin( this );
        m_ImageProperties.at( getWidgetSpecCurrentImage() ).viewPort
        = QOrientationHandler::getViewPort( currentZoom,
                                            getWidgetSpecCurrentImage(),
                                            width(),
                                            height(),
                                            m_PlaneOrientation,
                                            m_Border
                                          );
        boost::shared_ptr<ImageHolder> cImage =  getWidgetSpecCurrentImage();

        if( m_ViewerCore->getMode() == ViewerCoreBase::zmap ) {
            //painting all anatomical images
            BOOST_FOREACH( ImageVectorType::const_reference image, m_ImageVector ) {
                if( image.get() != cImage.get()
                    && image->isVisible
                    && image->imageType == ImageHolder::structural_image ) {
                    paintImage( image );
                }
            }

            if( cImage->imageType == ImageHolder::structural_image
                && cImage->isVisible ) {
                paintImage( cImage );
            }

            //painting the zmaps
            BOOST_FOREACH( ImageVectorType::const_reference image, m_ImageVector ) {
                if( image.get() != cImage.get()
                    && image->isVisible
                    && image->imageType == ImageHolder::z_map ) {
                    paintImage( image );
                }
            }

            if( cImage->imageType == ImageHolder::z_map
                && cImage->isVisible ) {
                paintImage( cImage );
            }
        } else {
            BOOST_FOREACH( ImageVectorType::const_reference image, m_ImageVector ) {
                if( image.get() != cImage.get()
                    && image->isVisible ) {
                    paintImage( image );
                }
            }

            if( cImage->isVisible ) {
                paintImage( cImage );
            }
        }


        if( m_ShowCrosshair ) {
            paintCrosshair();
        }

        if( m_ShowLabels ) {
            showLabels();
        }

        if( m_ShowScalingOffset ) {
            m_Painter->resetMatrix();
            m_Painter->setFont( QFont( "Chicago", 10 ) );
            m_Painter->setPen( Qt::red );
            std::stringstream scalingOffset;
            boost::shared_ptr<ImageHolder> image = getWidgetSpecCurrentImage();
            scalingOffset << "Scaling: " << image->scaling
                          << " Offset: " << image->offset;
            m_Painter->drawText( 10, 30, scalingOffset.str().c_str() );
        }

        m_ShowScalingOffset = false;
        m_Painter->end();
    }

}

void QImageWidgetImplementation::recalculateTranslation()
{
    const boost::shared_ptr<ImageHolder > image = getWidgetSpecCurrentImage();
    const util::ivector4 mappedSize = QOrientationHandler::mapCoordsToOrientation( image->getImageSize(), image, m_PlaneOrientation );
    const util::ivector4 mappedVoxelCoords = QOrientationHandler::mapCoordsToOrientation( image->voxelCoords, image, m_PlaneOrientation );
    const util::ivector4 signVec = QOrientationHandler::mapCoordsToOrientation( util::ivector4( 1, 1, 1, 1 ), image, m_PlaneOrientation, false, false );
    const util::ivector4 center = mappedSize / 2;
    const util::ivector4 diff = center - mappedVoxelCoords;
    const float transXConst = ( ( center[0] + 2 ) - mappedSize[0] / ( 2 * currentZoom ) );
    const float transYConst = ( ( center[1] + 2 ) - mappedSize[1] / ( 2 * currentZoom ) );
    const float transX = transXConst * ( ( float )diff[0] / ( float )center[0] ) * signVec[0];
    const float transY = transYConst * ( ( float )diff[1] / ( float )center[1] ) * signVec[1];
    const QOrientationHandler::ViewPortType viewPort = m_ImageProperties.at( image ).viewPort;
    translationX = transX * viewPort[0] ;
    translationY = transY * viewPort[1] ;
}


void QImageWidgetImplementation::paintImage( boost::shared_ptr< ImageHolder > image )
{
    ImageProperties &imgProps = m_ImageProperties.at( image );

    switch( m_InterpolationType ) {
    case 0:
        m_Painter->setRenderHint( QPainter::NonCosmeticDefaultPen, true );
        break;
    case 1:
        m_Painter->setRenderHint( QPainter::SmoothPixmapTransform, true );
        break;
    }

    const util::ivector4 mappedSizeAligned = QOrientationHandler::mapCoordsToOrientation( image->alignedSize32, image, m_PlaneOrientation );

    m_Painter->resetMatrix();

    if( image.get() != getWidgetSpecCurrentImage().get() ) {
        imgProps.viewPort =  QOrientationHandler::getViewPort( currentZoom, image, width(), height(),
                             m_PlaneOrientation, m_Border );
    }

    if( !m_LeftMouseButtonPressed || m_RightMouseButtonPressed || m_WidgetProperties.getPropertyAs<bool>( "zoomEvent" ) ) {
        recalculateTranslation();
    }

    imgProps.viewPort[2] += translationX;
    imgProps.viewPort[3] += translationY;

    m_Painter->setTransform( QOrientationHandler::getTransform( imgProps.viewPort, image, m_PlaneOrientation ) );

    m_Painter->setOpacity( image->opacity );

    if ( !image->isRGB ) {
        isis::data::MemChunk<InternalImageType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );
        m_MemoryHandler.fillSliceChunk<InternalImageType>( sliceChunk, image, m_PlaneOrientation, image->voxelCoords[3] );
        QImage qImage( ( InternalImageType * ) sliceChunk.asValuePtr<InternalImageType>().getRawAddress().get(),
                       mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_Indexed8 );
        qImage.setColorTable( image->colorMap );
        m_Painter->drawImage( 0, 0, qImage );
    } else {
        isis::data::MemChunk<InternalImageColorType> sliceChunk( mappedSizeAligned[0], mappedSizeAligned[1] );
        m_MemoryHandler.fillSliceChunk<InternalImageColorType>( sliceChunk, image, m_PlaneOrientation, image->voxelCoords[3] );
        QImage qImage( ( InternalImageType * ) sliceChunk.asValuePtr<InternalImageColorType>().getRawAddress().get(),
                       mappedSizeAligned[0], mappedSizeAligned[1], QImage::Format_RGB888 );
        m_Painter->drawImage( 0, 0, qImage );

    }

    //workaround to elimninate white edges
    m_Painter->resetMatrix();
    m_Painter->fillRect( imgProps.viewPort[4] + imgProps.viewPort[2] - ( m_InterpolationType ? 3 : 0 ) , 0, width(), height(), Qt::black );
    m_Painter->fillRect( 0, imgProps.viewPort[5] + imgProps.viewPort[3] - ( m_InterpolationType ? 3 : 0 ), width(), height(), Qt::black );
    m_Painter->fillRect( 0, 0, imgProps.viewPort[2], height(), Qt::black );
}


void QImageWidgetImplementation::mousePressEvent( QMouseEvent *e )
{
    if( e->button() == Qt::LeftButton && geometry().contains( e->pos() ) && QApplication::keyboardModifiers() == Qt::ControlModifier ) {
        QDrag *drag = new QDrag( this );
        QMimeData *mimeData = new QMimeData;
        mimeData->setText( getWidgetSpecCurrentImage()->getFileNames().front().c_str() );
        drag->setMimeData( mimeData );
        drag->setPixmap( QIcon( ":/common/vast.jpg" ).pixmap( 15 ) );
        drag->exec();
        return;
    }

    if( e->button() == Qt::RightButton ) {
        m_RightMouseButtonPressed = true;
    } else if ( e->button() == Qt::LeftButton ) {
        m_LeftMouseButtonPressed = true;
    }

    if( m_LeftMouseButtonPressed && m_RightMouseButtonPressed ) {
        m_StartCoordsPair.first = e->x();
        m_StartCoordsPair.second = e->y();
    }

    if( m_ViewerCore->getMode() == ViewerCoreBase::zmap ) {
        BOOST_FOREACH( ImageVectorType::const_reference image, m_ImageVector ) {
            if( image->imageType == ImageHolder::z_map ) {
                m_ViewerCore->setCurrentImage( image );
            }
        }
        m_ViewerCore->getUICore()->refreshUI();
    }

    setFocus();
    emitMousePressEvent( e );
}

void QImageWidgetImplementation::mouseMoveEvent( QMouseEvent *e )
{

    if( m_RightMouseButtonPressed && m_LeftMouseButtonPressed ) {
        if( QApplication::keyboardModifiers() == Qt::ShiftModifier ) {
            BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) {
                const double offset =  ( m_StartCoordsPair.second - e->y() ) / ( float )height() * image.second->extent;
                const double scaling = 1.0 - ( m_StartCoordsPair.first - e->x() ) / ( float )width() * 5;
                image.second->offset = offset < 0 ? 0 : offset;
                image.second->scaling = scaling < 0.0 ? 0.0 : scaling;
                image.second->updateColorMap();
            }
        } else {
            boost::shared_ptr<ImageHolder> image = getWidgetSpecCurrentImage();
            const double offset =  ( m_StartCoordsPair.second - e->y() ) / ( float )height() * image->extent;
            const double scaling = 1.0 - ( m_StartCoordsPair.first - e->x() ) / ( float )width() * 5;
            image->offset = offset < 0 ? 0 : offset;
            image->scaling = scaling < 0.0 ? 0.0 : scaling;
            image->updateColorMap();
        }

        m_ShowScalingOffset = true;
        m_ViewerCore->updateScene();
    } else if( m_RightMouseButtonPressed || m_LeftMouseButtonPressed ) {
        emitMousePressEvent( e );
    }

    QWidget::mouseMoveEvent( e );
}

void QImageWidgetImplementation::emitMousePressEvent( QMouseEvent *e )
{
    const boost::shared_ptr<ImageHolder> image = getWidgetSpecCurrentImage();
    const ImageProperties &imgProps = m_ImageProperties.at( image );
    const uint16_t slice = QOrientationHandler::mapCoordsToOrientation( image->voxelCoords, image, m_PlaneOrientation )[2];
    const util::ivector4 &coords = QOrientationHandler::convertWindow2VoxelCoords( imgProps.viewPort, image, e->x(), e->y(), slice, m_PlaneOrientation );
    physicalCoordsChanged( image->getISISImage()->getPhysicalCoordsFromIndex( coords ) );
}

void QImageWidgetImplementation::showLabels() const
{
    m_Painter->resetMatrix();
    m_Painter->setFont( QFont( "Chicago", 13 ) );

    switch( m_PlaneOrientation ) {
    case axial:
        m_Painter->setPen( QColor( 255, 0, 0 ) );
        m_Painter->drawText( 0, height() / 2 + 7, "L" );
        m_Painter->drawText( width() - 15, height() / 2 + 7, "R" );
        m_Painter->setPen( QColor( 0, 255, 0 ) );
        m_Painter->drawText( width() / 2 - 7, 15, "A" );
        m_Painter->drawText( width() / 2 - 7, height() - 2, "P" );
        break;
    case sagittal:
        m_Painter->setPen( QColor( 0, 255, 0 ) );
        m_Painter->drawText( 0, height() / 2 + 7, "A" );
        m_Painter->drawText( width() - 15, height() / 2 + 7, "P" );
        m_Painter->setPen( QColor( 0, 0, 255 ) );
        m_Painter->drawText( width() / 2 - 7, 15, "S" );
        m_Painter->drawText( width() / 2 - 7, height() - 2, "I" );
        break;
    case coronal:
        m_Painter->setPen( QColor( 255, 0, 0 ) );
        m_Painter->drawText( 0, height() / 2 + 10, "L" );
        m_Painter->drawText( width() - 15, height() / 2 + 7, "R" );
        m_Painter->setPen( QColor( 0, 0, 255 ) );
        m_Painter->drawText( width() / 2 - 7, 15, "S" );
        m_Painter->drawText( width() / 2 - 7, height() - 2, "I" );
        break;
    }



}


void QImageWidgetImplementation::paintCrosshair() const
{
    const boost::shared_ptr< ImageHolder > image = getWidgetSpecCurrentImage();
    const ImageProperties &imgProps = m_ImageProperties.at( image );
    std::pair<int, int> coords = QOrientationHandler::convertVoxel2WindowCoords( imgProps.viewPort, image, m_PlaneOrientation );
    short border = -5000;
    const QLine xline1( coords.first, border, coords.first, coords.second - 15 );
    const QLine xline2( coords.first, coords.second + 15, coords.first, height() - border  );

    const QLine yline1( border, coords.second, coords.first - 15, coords.second );
    const QLine yline2( coords.first + 15, coords.second,  width() - border, coords.second  );

    QPen pen;
    pen.setColor( m_CrosshairColor );
    pen.setWidth( m_CrosshairWidth );
    m_Painter->setOpacity( 1.0 );
    m_Painter->setTransform( QOrientationHandler::getTransform( imgProps.viewPort, image, m_PlaneOrientation ) );
    m_Painter->scale( 1.0 / imgProps.viewPort[0], 1.0 / imgProps.viewPort[1] );
    m_Painter->translate( -imgProps.viewPort[2], -imgProps.viewPort[3] );
    m_Painter->setPen( pen );
    m_Painter->drawLine( xline1 );
    m_Painter->drawLine( xline2 );
    m_Painter->drawLine( yline1 );
    m_Painter->drawLine( yline2 );
    pen.setWidth( 2 );
    m_Painter->drawPoint( coords.first, coords.second );


}

void QImageWidgetImplementation::lookAtPhysicalCoords( const isis::util::fvector4 &physicalCoords )
{
    BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) {
        image.second->physicalCoords = physicalCoords;
        image.second->voxelCoords = image.second->getISISImage()->getIndexFromPhysicalCoords( physicalCoords, true );
    }
    update();
}

void QImageWidgetImplementation::mouseReleaseEvent( QMouseEvent *e )
{
    if( e->button() == Qt::RightButton ) {
        m_RightMouseButtonPressed = false;
    }

    if ( e->button() == Qt::LeftButton ) {
        m_LeftMouseButtonPressed = false;
    }

    QWidget::mouseReleaseEvent( e );
}

void QImageWidgetImplementation::wheelEvent( QWheelEvent *e )
{
    float oldZoom = currentZoom;

    if ( e->delta() < 0 ) {
        oldZoom /= m_WidgetProperties.getPropertyAs<float>( "zoomFactorOut" );
    } else {
        oldZoom *= m_WidgetProperties.getPropertyAs<float>( "zoomFactorIn" );

    }

    if( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "propagateZooming" ) ) {
        zoomChanged( oldZoom );
    } else {
        setZoom( oldZoom );
    }

}

void QImageWidgetImplementation::updateScene()
{
    update();
}

std::string QImageWidgetImplementation::getWidgetName() const
{
    return windowTitle().toStdString();
}

void QImageWidgetImplementation::setWidgetName( const std::string &wName )
{
    setWindowTitle( QString( wName.c_str() ) );
    m_WidgetProperties.setPropertyAs<std::string>( "widgetName", wName );
}

void QImageWidgetImplementation::keyPressEvent( QKeyEvent *e )
{
    float oldZoom = currentZoom;

    if( e->key() == Qt::Key_Minus ) {
        oldZoom /= m_WidgetProperties.getPropertyAs<float>( "zoomFactorOut" );
    } else if ( e->key() == Qt::Key_Plus ) {
        oldZoom *= m_WidgetProperties.getPropertyAs<float>( "zoomFactorIn" );
    } else if( e->key() == Qt::Key_Space ) {
        m_ViewerCore->centerImages();
    }

    if( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "propagateZooming" ) ) {
        zoomChanged( oldZoom );
    } else {
        setZoom( oldZoom );
    }

    if( m_ViewerCore->hasImage() ) {
        if( e->key() == Qt::Key_Up ) {
            m_ViewerCore->getCurrentImage()->voxelCoords[1]++;
            m_ViewerCore->getCurrentImage()->checkVoxelCoords( m_ViewerCore->getCurrentImage()->voxelCoords );
            m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( m_ViewerCore->getCurrentImage()->voxelCoords ) );
        }

        if( e->key() == Qt::Key_Down ) {
            m_ViewerCore->getCurrentImage()->voxelCoords[1]--;
            m_ViewerCore->getCurrentImage()->checkVoxelCoords( m_ViewerCore->getCurrentImage()->voxelCoords );
            m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( m_ViewerCore->getCurrentImage()->voxelCoords ) );
        }

        if( e->key() == Qt::Key_Left ) {
            m_ViewerCore->getCurrentImage()->voxelCoords[0]--;
            m_ViewerCore->getCurrentImage()->checkVoxelCoords( m_ViewerCore->getCurrentImage()->voxelCoords );
            m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( m_ViewerCore->getCurrentImage()->voxelCoords ) );
        }

        if( e->key() == Qt::Key_Right ) {
            m_ViewerCore->getCurrentImage()->voxelCoords[0]++;
            m_ViewerCore->getCurrentImage()->checkVoxelCoords( m_ViewerCore->getCurrentImage()->voxelCoords );
            m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( m_ViewerCore->getCurrentImage()->voxelCoords ) );
        }

        if( e->key() == Qt::Key_PageUp ) {
            m_ViewerCore->getCurrentImage()->voxelCoords[2]++;
            m_ViewerCore->getCurrentImage()->checkVoxelCoords( m_ViewerCore->getCurrentImage()->voxelCoords );
            m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( m_ViewerCore->getCurrentImage()->voxelCoords ) );
        }

        if( e->key() == Qt::Key_PageDown ) {
            m_ViewerCore->getCurrentImage()->voxelCoords[2]--;
            m_ViewerCore->getCurrentImage()->checkVoxelCoords( m_ViewerCore->getCurrentImage()->voxelCoords );
            m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( m_ViewerCore->getCurrentImage()->voxelCoords ) );
        }
    }
}


void QImageWidgetImplementation::dragEnterEvent( QDragEnterEvent *e )
{
    if( e->mimeData()->hasFormat( "text/plain" ) ) {
        bool hasImage = false;
        BOOST_FOREACH( ImageVectorType::const_reference image, m_ImageVector ) {
            if( image->getFileNames().front() == e->mimeData()->text().toStdString() ) {
                hasImage = true;
            }
        }

        if( !hasImage ) {
            e->acceptProposedAction();
        }
    }
}


void QImageWidgetImplementation::dropEvent( QDropEvent *e )
{
    const boost::shared_ptr<ImageHolder> image = m_ViewerCore->getDataContainer().at( e->mimeData()->text().toStdString() );
    UICore::ViewWidgetEnsembleType myEnsemble = UICore::ViewWidgetEnsembleType();
    BOOST_FOREACH( UICore::ViewWidgetEnsembleListType::const_reference ensemble, m_ViewerCore->getUICore()->getEnsembleList() ) {
        for( unsigned short i = 0; i < 3; i++ ) {
            if( ensemble[i].widgetImplementation == this ) {
                myEnsemble = ensemble;
            } else if( myEnsemble != ensemble ) {
                ensemble[i].widgetImplementation->removeImage( image );
                image->removeWidget( ensemble[i].widgetImplementation );
            }
        }
    }

    for( unsigned short i = 0; i < 3; i++ ) {
        myEnsemble[i].widgetImplementation->addImage( image );
    }

    m_ViewerCore->setCurrentImage( image );
    m_ViewerCore->updateScene();
    m_ViewerCore->getUICore()->refreshUI();
}


}
} //end namespace
