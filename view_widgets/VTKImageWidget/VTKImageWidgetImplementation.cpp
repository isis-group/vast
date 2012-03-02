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



namespace isis {
namespace viewer {
namespace widget {


VTKImageWidgetImplementation::VTKImageWidgetImplementation()
	: m_RenderWindow( vtkRenderWindow::New() ),
	  m_Renderer( vtkRenderer::New() )
{
	
}
	
VTKImageWidgetImplementation::VTKImageWidgetImplementation ( QViewerCore* core, QWidget* parent, PlaneOrientation orientation )
	: QVTKWidget( parent ),
	  m_Layout( new QVBoxLayout( parent ) ),
	  m_RenderWindow( vtkRenderWindow::New() ),
	  m_Renderer( vtkRenderer::New() )
{
	setup( core, parent, orientation );

	commonInit();
}

void VTKImageWidgetImplementation::setup ( QViewerCore* core, QWidget* parent, PlaneOrientation orientation )
{
	WidgetInterface::setup( core, parent, orientation );
	setParent( parent );
	m_Layout = new QVBoxLayout( parent );
	commonInit();
}


void VTKImageWidgetImplementation::paintEvent ( QPaintEvent* event )
{
	BOOST_FOREACH( ComponentsMapType::reference component, m_VTKImageComponentsMap ) {
		component.second.opacityFunction->RemoveAllPoints();
		component.second.colurFunction->RemoveAllPoints();
		for( unsigned short ci = 0; ci < 256; ci++ ) {
				component.second.colurFunction->AddRGBPoint(ci, QColor( component.first->getImageProperties().colorMap[ci] ).redF(),
																QColor( component.first->getImageProperties().colorMap[ci] ).greenF(),
																QColor( component.first->getImageProperties().colorMap[ci] ).blueF() );
		}
		if( component.first->getImageProperties().imageType == ImageHolder::z_map ) {
			for( unsigned short ci =0 ; ci < 256; ci++ ) {
				component.second.opacityFunction->AddPoint( ci, component.first->getImageProperties().alphaMap[ci] * component.first->getImageProperties().opacity );
			}
		} else {
			component.second.opacityFunction->AddPoint(0,0);
			component.second.opacityFunction->AddPoint(1, m_OpacityGradientFactor * component.first->getImageProperties().opacity );
			component.second.opacityFunction->AddPoint(256,component.first->getImageProperties().opacity);
		}

	}
	QVTKWidget::paintEvent(event);
}

void VTKImageWidgetImplementation::wheelEvent ( QWheelEvent* e )
{
//     QVTKWidget::wheelEvent ( e );
}

void VTKImageWidgetImplementation::commonInit()
{
	m_Layout->addWidget( this );
	m_Layout->setMargin( 0 );
	connect( m_ViewerCore, SIGNAL( emitUpdateScene( ) ), this, SLOT( updateScene( ) ) );
	setFocus();
	SetRenderWindow(m_RenderWindow);
	
	m_RenderWindow->AddRenderer( m_Renderer );
	m_Renderer->SetBackground( 0.1, 0.2, 0.4 );
	m_OpacityGradientFactor = 0;

}

void VTKImageWidgetImplementation::updateScene()
{
	update();
}

void VTKImageWidgetImplementation::setZoom ( float zoom )
{

}

void VTKImageWidgetImplementation::setEnableCrosshair ( bool enable )
{

}

void VTKImageWidgetImplementation::addImage ( const boost::shared_ptr< ImageHolder > image )
{

	VTKImageComponents component = m_VTKImageComponentsMap[image];
	m_ImageVector.push_back(image);
	m_Renderer->AddVolume( component.volume );
	component.setVTKImageData( VolumeHandler::getVTKImageData(image, image->getImageProperties().voxelCoords[3]) );
	component.mapper->SetImageSampleDistance( 1.5 );
	component.mapper->SetSampleDistance( 0.3 );
	if( m_ImageVector.size() == 1 ) {
		m_Renderer->GetActiveCamera()->SetPosition( image->getImageProperties().indexOrigin[0] * 2, image->getImageProperties().indexOrigin[1] * 2, image->getImageProperties().indexOrigin[2] * 1.5 );
		m_Renderer->GetActiveCamera()->Roll(-45);
	}
	m_Renderer->ResetCamera();
	
}


void VTKImageWidgetImplementation::lookAtPhysicalCoords ( const util::fvector4& physicalCoords )
{}

bool VTKImageWidgetImplementation::removeImage ( const boost::shared_ptr< ImageHolder > image )
{

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
}
void VTKImageWidgetImplementation::setMouseCursorIcon ( QIcon )
{

}



}}} //end namespace


isis::viewer::widget::WidgetInterface *loadWidget()
{
	return new isis::viewer::widget::VTKImageWidgetImplementation();
}

const isis::util::PropertyMap* getProperties()
{
	isis::util::PropertyMap *properties = new isis::util::PropertyMap();
	properties->setPropertyAs<std::string>("widgetIdent", "vtk_rendering_widget");
	properties->setPropertyAs<uint8_t>("numberOfEntitiesInEnsemble", 1);
	return properties;
}