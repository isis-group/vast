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
#include "ImageComponents.hpp"
#include "OptionWidget.hpp"

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
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkCursor3D.h>
#include <vtkPolyDataMapper.h>



namespace isis
{
namespace viewer
{
namespace widget
{

class VTKImageWidgetImplementation : public QVTKWidget, public WidgetInterface
{
	Q_OBJECT

public:

	typedef std::map< ImageHolder::Pointer, VTKImageComponents > ComponentsMapType;

	VTKImageWidgetImplementation();
	VTKImageWidgetImplementation( QViewerCore *core, QWidget *parent = 0, PlaneOrientation orientation = axial );

public Q_SLOTS:

	virtual unsigned short getNumberOfInstancesInEnsemble() const { return 1; }
	virtual void setup( QViewerCore *, QWidget *, PlaneOrientation );
	virtual void setZoom( float zoom );
	virtual void updateScene();
	virtual void setEnableCrosshair( bool enable );
	virtual void addImage( const ImageHolder::Pointer image );
	virtual bool removeImage( const ImageHolder::Pointer image );
	virtual std::string getWidgetIdent() const { return std::string( "vtk_rendering_widget" ); }
	virtual std::string getWidgetName() const { return std::string( "3D Rendering widget (vtk)" ); }
	virtual void setInterpolationType( InterpolationType interpolation );
	virtual void setMouseCursorIcon( QIcon );
	virtual void lookAtPhysicalCoords( const util::fvector3 &physicalCoords );
	virtual bool hasOptionWidget() const { return true; }
	virtual QWidget *getOptionWidget() { return m_OptionWidget; }

	virtual void disconnectSignals();
	virtual void connectSignals();

	virtual void mousePressEvent( QMouseEvent * );
	virtual void mouseReleaseEvent( QMouseEvent * );
	virtual void mouseMoveEvent( QMouseEvent * );

	virtual void dragEnterEvent( QDragEnterEvent * );
	virtual void dropEvent( QDropEvent * );

	void showAnterior();
	void showPosterior();
	void showRight();
	void showLeft();
	void showSuperior();
	void showInferior();

	void setOpacityGradientFactor( double factor ) { m_OpacityGradientFactor = factor; }
	void setShade( bool shade );

	void resetCamera();

	void setCropping( double *cropping );
	void setMapper( int mapper, bool global = true );
	VTKImageComponents::VTKMapperType getMapper() const { return m_MapperType; }

protected:
	void paintEvent( QPaintEvent *event );
	virtual void wheelEvent( QWheelEvent *e );

private:
	QVBoxLayout *m_Layout;

	void commonInit();
	void updatePhysicalBounds();
	void currentImageChanged( const ImageHolder::Pointer image );

	//vtk stuff
	vtkRenderWindow *m_RenderWindow;
	vtkRenderer *m_Renderer;
	vtkActor *m_Actor;
	vtkPolyDataMapper *m_CursorMapper;
	vtkCursor3D *m_Cursor;

	ComponentsMapType m_VTKImageComponentsMap;
	geometrical::BoundingBoxType m_PhysicalBounds;

	double m_OpacityGradientFactor;
	int m_CameraDistance;
	OptionWidget *m_OptionWidget;

	util::dvector4 getCenterOfBoundingBox();

	bool m_RightButtonPressed;
	bool m_LeftButtonPressed;

	std::pair<int, int> m_StartCoordsPair;

	VTKImageComponents::VTKMapperType m_MapperType;

private Q_SLOTS:
	void reloadImage( const ImageHolder::Pointer );

};



}
}
} //end namespace




#endif // VTK_IMAGE_WIDGET_IMPLEMENTATION_HPP