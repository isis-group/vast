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

#ifndef VTK_IMAGE_WIDGET_IMPLEMENTATION_HPP
#define VTK_IMAGE_WIDGET_IMPLEMENTATION_HPP

#include "VolumeHandler.hpp"

#include "widgetinterface.h"
#include "qviewercore.hpp"
#include <QVTKWidget.h>
//vtk
#include <vtkPlane.h>
#include <vtkCamera.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkImageAppendComponents.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
#include <vtkCursor3D.h>

namespace isis
{
namespace viewer
{
namespace widget
{

class VTKImageWidgetImplementation : public QVTKWidget, public WidgetInterface
{
	Q_OBJECT

	struct VTKImageComponents {
		VTKImageComponents()
			: volume( vtkVolume::New() ),
			  property( vtkVolumeProperty::New() ),
			  mapper( vtkFixedPointVolumeRayCastMapper::New() ),
			  colurFunction( vtkColorTransferFunction::New() ),
			  opacityFunction( vtkPiecewiseFunction::New() ),
			  imageData( vtkImageData::New() ) {
			volume->SetMapper( mapper );
			volume->SetProperty( property );
			property->SetColor( colurFunction );
			property->SetScalarOpacity( opacityFunction );
			mapper->SetInput( imageData );
		}

		void setVTKImageData( vtkImageData *image ) {
			mapper->SetInput( image );
			imageData = image;
		}
		vtkImageData* getVTKImageData() const { return imageData; }
		
		vtkVolume *volume;
		vtkVolumeProperty *property;
		vtkFixedPointVolumeRayCastMapper *mapper;
		vtkColorTransferFunction *colurFunction;
		vtkPiecewiseFunction *opacityFunction;
	private:
		vtkImageData *imageData;
	};
public:

	typedef std::map< boost::shared_ptr<ImageHolder>, VTKImageComponents > ComponentsMapType;

	VTKImageWidgetImplementation();
	VTKImageWidgetImplementation( QViewerCore *core, QWidget *parent = 0, PlaneOrientation orientation = axial );

public Q_SLOTS:

	virtual unsigned short getNumberOfInstancesInEnsemble() const { return 1; }
	virtual void setup( QViewerCore *, QWidget *, PlaneOrientation );
	virtual void setZoom( float zoom );
	virtual void updateScene();
	virtual void setEnableCrosshair( bool enable );
	virtual void addImage( const boost::shared_ptr<ImageHolder> image );
	virtual bool removeImage( const boost::shared_ptr< ImageHolder > image );
	virtual std::string getWidgetName() const { return std::string( "vtk_rendering_widget" ); }
	virtual void setInterpolationType( InterpolationType interpolation );
	virtual void setMouseCursorIcon( QIcon );
	virtual void lookAtPhysicalCoords( const util::fvector4 &physicalCoords );


	void setOpacityGradientFactor( float factor ) { m_OpacityGradientFactor = factor; }

protected:
	void paintEvent( QPaintEvent *event );
	virtual void wheelEvent( QWheelEvent *e );

private:
	QVBoxLayout *m_Layout;

	void commonInit();

	//vtk stuff
	vtkRenderWindow *m_RenderWindow;
	vtkRenderer *m_Renderer;
	vtkActor *m_Actor;
	vtkPolyDataMapper *m_CursorMapper;
	vtkCursor3D *m_Cursor;
	
	ComponentsMapType m_VTKImageComponentsMap;

	float m_OpacityGradientFactor;


};



}
}
} //end namespace




#endif // VTK_IMAGE_WIDGET_IMPLEMENTATION_HPP