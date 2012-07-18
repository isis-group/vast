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
 * Author: Erik Tuerke, tuerke@cbs.mpg.de, Samuel Eckermann, Hans-Christian Heinz
 *
 * VTKImageWidgetImplementation.cpp
 *
 * Description:
 *
 *  Created on: Feb 28, 2012
 ******************************************************************/

#include "VTKImageWidgetImplementation.hpp"

#include "CoreUtils/singletons.hpp"
#include "geometrical.hpp"
#include <widgetensemble.hpp>
#include <uicore.hpp>
#include <sys/stat.h>


namespace isis
{
namespace viewer
{
namespace widget
{


VTKImageWidgetImplementation::VTKImageWidgetImplementation()
	: m_RenderWindow( vtkRenderWindow::New() ),
	  m_Renderer( vtkRenderer::New() ),
	  m_Actor( vtkActor::New() ),
	  m_CursorMapper( vtkPolyDataMapper::New() ),
	  m_Cursor( vtkCursor3D::New() )
{
}

VTKImageWidgetImplementation::VTKImageWidgetImplementation ( QViewerCore *core, QWidget *parent, PlaneOrientation orientation )
	: QVTKWidget( parent ),
	  m_Layout( new QVBoxLayout( parent ) ),
	  m_RenderWindow( vtkRenderWindow::New() ),
	  m_Renderer( vtkRenderer::New() )
{
	setup( core, parent, orientation );

	commonInit();
}

void VTKImageWidgetImplementation::setup ( QViewerCore *core, QWidget *parent, PlaneOrientation orientation )
{
	WidgetInterface::setup( core, parent, orientation );
	setParent( parent );
	m_Layout = new QVBoxLayout( parent );
	commonInit();
	dynamic_cast<OptionWidget *>( getWidgetEnsemble()->getOptionWidget() )->setWidget( this );
}


void VTKImageWidgetImplementation::paintEvent ( QPaintEvent *event )
{
	BOOST_FOREACH( ComponentsMapType::reference component, m_VTKImageComponentsMap ) {
		const bool isVisible = component.first->getImageProperties().isVisible;
		component.second.opacityFunction->RemoveAllPoints();
		component.second.colorFunction->RemoveAllPoints();

		for( unsigned short ci = 0; ci < 256; ci++ ) {
			component.second.colorFunction->AddRGBPoint( ci, QColor( component.first->getImageProperties().colorMap[ci] ).redF(),
					QColor( component.first->getImageProperties().colorMap[ci] ).greenF(),
					QColor( component.first->getImageProperties().colorMap[ci] ).blueF() );
		}

		if( component.first->getImageProperties().imageType == ImageHolder::statistical_image ) {
			for( unsigned short ci = 0 ; ci < 256; ci++ ) {
				component.second.opacityFunction->AddPoint( ci, component.first->getImageProperties().alphaMap[ci] * component.first->getImageProperties().opacity * isVisible );
			}
		} else {
			component.second.opacityFunction->AddPoint( 0, 0 );
			component.second.opacityFunction->AddPoint( 1, m_OpacityGradientFactor * component.first->getImageProperties().opacity * isVisible );
			component.second.opacityFunction->AddPoint( 256, component.first->getImageProperties().opacity * isVisible );
		}
	}
	QVTKWidget::paintEvent( event );
}

void VTKImageWidgetImplementation::wheelEvent ( QWheelEvent */*e*/ )
{
	//     QVTKWidget::wheelEvent ( e );
}

void VTKImageWidgetImplementation::disconnectSignals()
{
	disconnect( m_ViewerCore, SIGNAL( emitUpdateScene( ) ), this, SLOT( updateScene( ) ) );
	disconnect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector3 ) ), this, SLOT( lookAtPhysicalCoords( util::fvector3 ) ) );
	disconnect( m_ViewerCore, SIGNAL( emitZoomChanged( float ) ), this, SLOT( setZoom( float ) ) );
	disconnect( m_ViewerCore, SIGNAL( emitSetEnableCrosshair( bool ) ), this, SLOT( setEnableCrosshair( bool ) ) );
	m_ViewerCore->emitImageContentChanged.disconnect( boost::bind( &VTKImageWidgetImplementation::reloadImage, this, _1 ) );
	m_ViewerCore->emitCurrentImageChanged.disconnect( boost::bind( &VTKImageWidgetImplementation::currentImageChanged, this, _1 ) );
}

void VTKImageWidgetImplementation::connectSignals()
{
	connect( m_ViewerCore, SIGNAL( emitUpdateScene( ) ), this, SLOT( updateScene( ) ) );
	connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector3 ) ), this, SLOT( lookAtPhysicalCoords( util::fvector3 ) ) );
	connect( m_ViewerCore, SIGNAL( emitZoomChanged( float ) ), this, SLOT( setZoom( float ) ) );
	connect( m_ViewerCore, SIGNAL( emitSetEnableCrosshair( bool ) ), this, SLOT( setEnableCrosshair( bool ) ) );
	m_ViewerCore->emitImageContentChanged.connect( boost::bind( &VTKImageWidgetImplementation::reloadImage, this, _1 ) );
	m_ViewerCore->emitCurrentImageChanged.connect( boost::bind( &VTKImageWidgetImplementation::currentImageChanged, this, _1 ) );
}

void VTKImageWidgetImplementation::commonInit()
{
	//  m_OptionWidget = new OptionWidget( this, m_ViewerCore );
	m_Layout->addWidget( this );
	m_Layout->setMargin( 0 );
	setFocus();
	SetRenderWindow( m_RenderWindow );
	setAcceptDrops( true );
	m_RenderWindow->AddRenderer( m_Renderer );
	m_Renderer->SetBackground( 0.1, 0.2, 0.4 );
	m_OpacityGradientFactor = 0;
	m_CameraDistance = 500;
	m_RightButtonPressed = false;
	m_LeftButtonPressed = false;

	if( m_ViewerCore->getSettings()->getPropertyAs<bool>( "showCrosshair" ) ) {
		m_Cursor->AllOn();
		m_Cursor->OutlineOff();
	} else {
		m_Cursor->AllOff();
	}

	m_Cursor->Update();
	m_CursorMapper->SetInputConnection( m_Cursor->GetOutputPort() );
	m_Actor->SetMapper( m_CursorMapper );
	m_Renderer->AddActor( m_Actor );
	m_MapperType = VTKImageComponents::CPU_FixedPointRayCast;
}

void VTKImageWidgetImplementation::currentImageChanged ( const ImageHolder::Pointer /*image*/ )
{
	//readd all images to the renderer so we have the same order as in the ensembles image vector
	m_Renderer->RemoveAllViewProps();
	BOOST_FOREACH( const ComponentsMapType::const_reference component, m_VTKImageComponentsMap ) {
		m_Renderer->AddVolume( component.second.volume );
	}
	m_Renderer->AddActor( m_Actor );

}


void VTKImageWidgetImplementation::updateScene()
{
	update();
}

void VTKImageWidgetImplementation::setZoom ( float /*zoom*/ )
{

}

void VTKImageWidgetImplementation::mousePressEvent ( QMouseEvent *e )
{
	if( e->button() == Qt::LeftButton && geometry().contains( e->pos() ) && QApplication::keyboardModifiers() == Qt::ControlModifier ) {
		QDrag *drag = new QDrag( this );
		QMimeData *mimeData = new QMimeData;
		mimeData->setText( getWidgetEnsemble()->getImageVector().back()->getImageProperties().filePath.c_str() );
		drag->setMimeData( mimeData );
		drag->setPixmap( QIcon( ":/common/vast.jpg" ).pixmap( 15 ) );
		drag->exec();
		return;
	}

	if( e->button() == Qt::LeftButton ) {
		m_LeftButtonPressed = true;
	}

	if( e->button() == Qt::RightButton ) {
		m_RightButtonPressed = true;
	}

	if( m_ViewerCore->hasImage() ) {
		m_ViewerCore->onWidgetClicked( this, m_ViewerCore->getCurrentImage()->getImageProperties().physicalCoords, e->button() );
	}

	QVTKWidget::mousePressEvent( e );
}

void VTKImageWidgetImplementation::mouseMoveEvent ( QMouseEvent *e )
{

	QVTKWidget::mouseMoveEvent( e );
}

void VTKImageWidgetImplementation::mouseReleaseEvent ( QMouseEvent *e )
{
	if( e->button() == Qt::LeftButton ) {
		m_LeftButtonPressed = false;
	}

	if( e->button() == Qt::RightButton ) {
		m_RightButtonPressed = false;
	}

	QVTKWidget::mouseReleaseEvent( e );
}

void VTKImageWidgetImplementation::dragEnterEvent ( QDragEnterEvent *e )
{
	if( e->mimeData()->hasFormat( "text/plain" ) ) {
		bool hasImage = false;
		BOOST_FOREACH( ImageHolder::Vector::const_reference image, getWidgetEnsemble()->getImageVector() ) {
			if( image->getImageProperties().fileName == e->mimeData()->text().toStdString() ) {
				hasImage = true;
			}
		}

		if( !hasImage ) {
			e->acceptProposedAction();
		}
	}
}

void VTKImageWidgetImplementation::dropEvent ( QDropEvent *e )
{
	const std::string text = e->mimeData()->text().toStdString() ;

	if( m_ViewerCore->getImageMap().find( text ) != m_ViewerCore->getImageMap().end() ) {
		const ImageHolder::Pointer image = m_ViewerCore->getImageMap()[text];
		WidgetEnsemble::Pointer myEnsemble;
		BOOST_FOREACH( WidgetEnsemble::Vector::reference ensemble, m_ViewerCore->getUICore()->getEnsembleList() ) {
			BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, *ensemble ) {
				if( ensembleComponent->getWidgetInterface() == this ) {
					myEnsemble = ensemble;
				}
			}
		}
		myEnsemble->addImage( image  );
		BOOST_FOREACH( WidgetEnsemble::Vector::reference ensemble, m_ViewerCore->getUICore()->getEnsembleList() ) {
			if( ensemble != myEnsemble ) {
				ensemble->removeImage( image );
			}
		}

		m_ViewerCore->setCurrentImage( image );
		m_ViewerCore->updateScene();
		m_ViewerCore->getUICore()->refreshUI();
	} else {
		m_ViewerCore->getUICore()->openFromDropEvent( e );
	}
}


void VTKImageWidgetImplementation::setEnableCrosshair ( bool enable )
{
	if( enable ) {
		m_Cursor->AllOn();
	} else {
		m_Cursor->AllOff();
	}

	m_Cursor->OutlineOff();
	m_RenderWindow->Render();
}

void VTKImageWidgetImplementation::reloadImage ( const ImageHolder::Pointer image )
{
	const ComponentsMapType::iterator iter = m_VTKImageComponentsMap.find( image );

	if( iter != m_VTKImageComponentsMap.end() ) {
		updatePhysicalBounds();
		iter->second.setVTKImageData( VolumeHandler::getVTKImageData( image, m_PhysicalBounds, image->getImageProperties().voxelCoords[3] ) );
	}
}

void VTKImageWidgetImplementation::setCropping ( double *cropping )
{
	const util::dvector4 cBB = getCenterOfBoundingBox();

	const float extent[] = { m_PhysicalBounds[0].second - m_PhysicalBounds[0].first,
							 m_PhysicalBounds[1].second - m_PhysicalBounds[1].first,
							 m_PhysicalBounds[2].second - m_PhysicalBounds[2].first
						   };
	double fCropping[6];

	cropping[0] = m_PhysicalBounds[0].first + extent[0] / 1000 * cropping[0];
	cropping[1] = m_PhysicalBounds[0].first + extent[0] / 1000 * cropping[1];

	cropping[2] = m_PhysicalBounds[1].first + extent[1] / 1000 * cropping[2];
	cropping[3] = m_PhysicalBounds[1].first + extent[1] / 1000 * cropping[3];

	cropping[4] = m_PhysicalBounds[2].first + extent[2] / 1000 * cropping[4];
	cropping[5] = m_PhysicalBounds[2].first + extent[2] / 1000 * cropping[5];

	fCropping[0] = std::min( cropping[0], cropping[1] );
	fCropping[1] = std::max( cropping[0], cropping[1] );

	fCropping[2] = std::min( cropping[2], cropping[3] );
	fCropping[3] = std::max( cropping[2], cropping[3] );

	fCropping[4] = std::min( cropping[4], cropping[5] );
	fCropping[5] = std::max( cropping[4], cropping[5] );


	BOOST_FOREACH( ComponentsMapType::reference component, m_VTKImageComponentsMap ) {
		component.second.setCropping( fCropping );
	}
	update();
}

void VTKImageWidgetImplementation::setMapper ( int mapper, bool global )
{
	m_MapperType = static_cast<VTKImageComponents::VTKMapperType>( mapper );

	if( global ) {
		BOOST_FOREACH( ComponentsMapType::reference component, m_VTKImageComponentsMap ) {
			component.second.setMapperType( m_MapperType );
			reloadImage( component.first );
		}
	}

	update();
}


void VTKImageWidgetImplementation::addImage ( const ImageHolder::Pointer image )
{
	updatePhysicalBounds();
	vtkImageData *vtkImage = VolumeHandler::getVTKImageData( image, m_PhysicalBounds, image->getImageProperties().voxelCoords[3] ) ;

	if( /*m_ViewerCore->getMode() == ViewerCoreBase::default_mode || */m_VTKImageComponentsMap.empty() ) {
		VTKImageComponents component( m_MapperType );
		m_VTKImageComponentsMap.insert( std::make_pair( image, component ) );
		m_Renderer->AddVolume( component.volume );
		component.setVTKImageData( vtkImage );
	} else {
		VTKImageComponents &component = m_VTKImageComponentsMap.begin()->second;
		component.mergeImage( vtkImage );
	}

	if( getWidgetEnsemble()->getImageVector().size() == 1 ) {
		m_Renderer->GetActiveCamera()->SetPosition( image->getImageProperties().indexOrigin[0] * 2, image->getImageProperties().indexOrigin[1] * 2, image->getImageProperties().indexOrigin[2] );
		m_Renderer->GetActiveCamera()->Roll( -65 );
	}

	resetCamera();
	lookAtPhysicalCoords( image->getImageProperties().physicalCoords );
	update();
}

void VTKImageWidgetImplementation::updatePhysicalBounds()
{
	m_PhysicalBounds = geometrical::getPhysicalBoundingBox( getWidgetEnsemble()->getImageVector() );
	m_Cursor->SetModelBounds( m_PhysicalBounds[0].first, m_PhysicalBounds[0].second, m_PhysicalBounds[1].first, m_PhysicalBounds[1].second, m_PhysicalBounds[2].first, m_PhysicalBounds[2].second );
}


void VTKImageWidgetImplementation::lookAtPhysicalCoords ( const util::fvector3 &physicalCoords )
{
	if( m_ViewerCore->hasImage() ) {
		boost::shared_ptr<ImageHolder> image = m_ViewerCore->getCurrentImage();

		m_Cursor->SetFocalPoint( physicalCoords[0],
								 physicalCoords[1],
								 physicalCoords[2] );
		update();
	}
}

bool VTKImageWidgetImplementation::removeImage ( const ImageHolder::Pointer image )
{
	ComponentsMapType::iterator iter = m_VTKImageComponentsMap.find( image );

	if( iter != m_VTKImageComponentsMap.end() ) {
		m_Renderer->RemoveVolume( iter->second.volume );
		m_VTKImageComponentsMap.erase( iter );
		updatePhysicalBounds();
		resetCamera();
	}

	updateScene();
	return true;
}
void VTKImageWidgetImplementation::setInterpolationType ( InterpolationType interpolation )
{
	BOOST_FOREACH( ComponentsMapType::reference component, m_VTKImageComponentsMap ) {
		switch( interpolation ) {
		case nn:
			component.second.property->SetInterpolationTypeToNearest();
			break;
		case lin:
			component.second.property->SetInterpolationTypeToLinear();
			break;
		}
	}
	update();
}

void VTKImageWidgetImplementation::setShade ( bool shade )
{
	BOOST_FOREACH( ComponentsMapType::reference component, m_VTKImageComponentsMap ) {
		component.second.property->SetShade( shade );
	}
	update();
}


void VTKImageWidgetImplementation::setMouseCursorIcon ( QIcon )
{
}


void VTKImageWidgetImplementation::showAnterior()
{
	const util::dvector4 center = getCenterOfBoundingBox();
	m_Renderer->GetActiveCamera()->SetParallelProjection( 1 );
	m_Renderer->GetActiveCamera()->SetViewUp( 0, 0, 1 );
	m_Renderer->GetActiveCamera()->SetPosition( center[0], -m_CameraDistance , center[2] );
	update();
}

void VTKImageWidgetImplementation::showInferior()
{
	const util::dvector4 center = getCenterOfBoundingBox();
	m_Renderer->GetActiveCamera()->SetParallelProjection( 1 );
	m_Renderer->GetActiveCamera()->SetViewUp( 0, -1, 0 );
	m_Renderer->GetActiveCamera()->SetPosition( center[0], center[1], -m_CameraDistance );
	update();
}

void VTKImageWidgetImplementation::showLeft()
{
	const util::dvector4 center = getCenterOfBoundingBox();
	m_Renderer->GetActiveCamera()->SetParallelProjection( 1 );
	m_Renderer->GetActiveCamera()->SetViewUp( 0, 0, 1 );
	m_Renderer->GetActiveCamera()->SetPosition( m_CameraDistance, center[1], center[2] );
	update();
}

void VTKImageWidgetImplementation::showPosterior()
{
	const util::dvector4 center = getCenterOfBoundingBox();
	m_Renderer->GetActiveCamera()->SetParallelProjection( 1 );
	m_Renderer->GetActiveCamera()->SetViewUp( 0, 0, 1 );
	m_Renderer->GetActiveCamera()->SetPosition( center[0], m_CameraDistance, center[2] );
	update();
}

void VTKImageWidgetImplementation::showRight()
{
	const util::dvector4 center = getCenterOfBoundingBox();
	m_Renderer->GetActiveCamera()->SetParallelProjection( 1 );
	m_Renderer->GetActiveCamera()->SetViewUp( 0, 0, 1 );
	m_Renderer->GetActiveCamera()->SetPosition( -m_CameraDistance, center[1], center[2] );
	update();
}

void VTKImageWidgetImplementation::showSuperior()
{
	const util::dvector4 center = getCenterOfBoundingBox();
	m_Renderer->GetActiveCamera()->SetParallelProjection( 1 );
	m_Renderer->GetActiveCamera()->SetViewUp( 0, -1, 0 );
	m_Renderer->GetActiveCamera()->SetPosition( center[0], center[1], m_CameraDistance );
	update();
}


isis::util::dvector4 VTKImageWidgetImplementation::getCenterOfBoundingBox()
{
	if( m_ViewerCore->hasImage() ) {
		util::dvector4 ret;
		ret[0] = ( m_PhysicalBounds[0].first + m_PhysicalBounds[0].second ) / 2.0;
		ret[1] = ( m_PhysicalBounds[1].first + m_PhysicalBounds[1].second ) / 2.0;
		ret[2] = ( m_PhysicalBounds[2].first + m_PhysicalBounds[2].second ) / 2.0;
		return ret;
	}

	return util::dvector4();
}

void VTKImageWidgetImplementation::resetCamera()
{
	m_Renderer->ResetCamera();
	update();
}


}
}
} //end namespace


const QWidget *loadOptionWidget()
{
	return new isis::viewer::widget::OptionWidget();
}

isis::viewer::widget::WidgetInterface *loadWidget()
{
	return new isis::viewer::widget::VTKImageWidgetImplementation();
}

const isis::util::PropertyMap *getProperties()
{
	isis::util::PropertyMap *properties = new isis::util::PropertyMap();
	properties->setPropertyAs<std::string>( "widgetIdent", "vtk_rendering_widget" );
	properties->setPropertyAs<std::string>( "widgetName", "3D Rendering widget (vtk) " );
	properties->setPropertyAs<uint8_t>( "numberOfEntitiesInEnsemble", 1 );
	properties->setPropertyAs<bool>( "hasOptionWidget", true );
	return properties;
}