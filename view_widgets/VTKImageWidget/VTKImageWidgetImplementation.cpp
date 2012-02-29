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

VTKImageWidgetImplementation::VTKImageWidgetImplementation ( QViewerCore* core, QWidget* parent, PlaneOrientation orientation )
	: QVTKWidget( parent ),
	  m_Layout( new QVBoxLayout( parent ) ),
	  m_RenderWindow( vtkRenderWindow::New() ),
	  m_Renderer( vtkRenderer::New() ),
	  m_Mapper( vtkFixedPointVolumeRayCastMapper::New() ),
	  m_Volume( vtkVolume::New() ),
	  m_VolumeProperty( vtkVolumeProperty::New() )
{
	setup( core, parent, orientation );
	m_Layout->addWidget( this );
	m_Layout->setMargin( 0 );
	commonInit();
}

void VTKImageWidgetImplementation::paintEvent ( QPaintEvent* event )
{
	for( unsigned short i = 0; i < m_ImageVector.size(); i++ ) {
		vtkColorTransferFunction* colorFun = vtkColorTransferFunction::New();
		vtkPiecewiseFunction* opacityFun = vtkPiecewiseFunction::New();
		boost::shared_ptr<ImageHolder> image = m_ImageVector[i];
		for( unsigned short ci = 0; ci < 255; ci++ ) {
			colorFun->AddRGBPoint(ci, QColor( image->colorMap[ci] ).redF(), QColor( image->colorMap[ci] ).greenF(), QColor( image->colorMap[ci] ).blueF() );			
		}
		m_VolumeProperty->SetColor(i, colorFun );
		opacityFun->AddPoint(image->minMax.first->as<double>(), 0 );
		opacityFun->AddPoint(image->minMax.second->as<double>(), image->opacity );
		m_VolumeProperty->SetScalarOpacity(i, opacityFun);
	}
	QVTKWidget::paintEvent(event);
}

void VTKImageWidgetImplementation::wheelEvent ( QWheelEvent* e )
{
//     QVTKWidget::wheelEvent ( e );
}

void VTKImageWidgetImplementation::commonInit()
{
	connect( m_ViewerCore, SIGNAL( emitUpdateScene( ) ), this, SLOT( updateScene( ) ) );
	setFocus();
	SetRenderWindow(m_RenderWindow);
	
	m_RenderWindow->AddRenderer( m_Renderer );
	m_Volume->SetMapper( m_Mapper );
	m_Renderer->AddVolume( m_Volume );
	m_Renderer->SetBackground( 0.1, 0.2, 0.4 );
	m_Volume->SetProperty( m_VolumeProperty );

	m_Mapper->SetImageSampleDistance(1.5);
	m_Mapper->SetSampleDistance(0.3);
	
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
	util::Singletons::get<VolumeHandler, 10>().addImage( image, image->voxelCoords[3] );
	m_ImageVector.push_back(image);
	m_Mapper->SetInput( util::Singletons::get<VolumeHandler, 10>().getMergedImage() );
	m_Renderer->ResetCamera();
}

void VTKImageWidgetImplementation::lookAtPhysicalCoords ( const util::fvector4& physicalCoords )
{

}

bool VTKImageWidgetImplementation::removeImage ( const boost::shared_ptr< ImageHolder > image )
{

}
void VTKImageWidgetImplementation::setInterpolationType ( InterpolationType interpolation )
{
	switch( interpolation ) {
		case nn:
			m_VolumeProperty->SetInterpolationTypeToNearest();
			break;
		case lin:
			m_VolumeProperty->SetInterpolationTypeToLinear();
			break;
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

const char* getIdentifier()
{
	return "VTKRendering";
}