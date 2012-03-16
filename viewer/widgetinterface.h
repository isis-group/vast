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
 * widgetinterface.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef QWIDGETIMPLEMENTATIONBASE_HPP
#define QWIDGETIMPLEMENTATIONBASE_HPP

#ifdef __cplusplus
#include "common.hpp"
#include "imageholder.hpp"
#include <CoreUtils/propmap.hpp>
#include <QtGui>

namespace isis
{

namespace viewer
{

class QViewerCore;

namespace widget {
	
class WidgetInterface
{
public:
	enum MouseButton{ none, left_mouse_button, middle_mouse_button, right_mouse_button, left_and_righ_mouse_button };
private:
	struct WidgetProperties {

		WidgetProperties() {
			mouseButtonStatus = none;
			currentZoom = 1.0;
			maxZoom = 30.0;
			minZoom = 0.1;
			zoomingFactor = 1.5;
			crosshairEnabled = true;
			crosshairColor = QColor( 255, 102, 0 );
			crosshairWidth = 1;
			labelsEnabled = true;
			mouseCursor = Qt::ArrowCursor;
		}
		//zoom stuff
		float currentZoom;
		float maxZoom;
		float minZoom;
		float zoomingFactor;

		//crosshair stuff
		bool crosshairEnabled;
		int crosshairWidth;
		QCursor mouseCursor;
		QColor crosshairColor;

		PlaneOrientation planeOrientation;
		InterpolationType interpolationType;

		bool labelsEnabled;

		MouseButton mouseButtonStatus;

	};	
public:
	///returns a boost shared_ptr pointing to the widgets properties
	boost::shared_ptr<WidgetProperties> getWidgetProperties() { return m_WidgetProperties; }
	///returns a const boost shared_ptr pointing to the widgets properties
	const boost::shared_ptr<WidgetProperties> getWidgetProperties() const { return m_WidgetProperties; }
	
	virtual void setup( QViewerCore *core, QWidget *parent, PlaneOrientation orientation ){
		m_ViewerCore = core;
		m_Parent = parent;
		m_PlaneOrientation = orientation;
		m_IsSetup = true;
	};
	
	typedef std::vector<boost::shared_ptr<ImageHolder> > ImageVectorType;

	virtual unsigned short getNumberOfInstancesInEnsemble() const = 0;
	virtual void setEnableCrosshair( bool enable ) = 0;
	virtual void updateScene() = 0;
	virtual void setZoom( float zoom ) = 0;
	virtual void addImage( const boost::shared_ptr<ImageHolder> image ) = 0;
	virtual bool removeImage( const boost::shared_ptr< ImageHolder > image ) = 0;
	virtual std::string getWidgetName() const = 0;
	virtual void setInterpolationType( InterpolationType interpolation ) = 0;
	virtual void setMouseCursorIcon( QIcon ) = 0;
	virtual void setCrossHairColor( QColor ) {}
	virtual void setCrossHairWidth( int ) {}
	virtual QWidget *getOptionWidget() = 0;
	virtual bool hasOptionWidget() const = 0; 

	QWidget *getParent( ) const { return m_Parent; }

	virtual void lookAtPhysicalCoords( const util::fvector4 &physicalCoords ) = 0;

	ImageVectorType getImageVector() const { return m_ImageVector; }
	std::string widget_file;

	
protected:
	boost::shared_ptr<WidgetProperties> m_WidgetProperties;
	
	WidgetInterface() : m_IsSetup(false) {}

	QViewerCore *m_ViewerCore;
	PlaneOrientation m_PlaneOrientation;
	QWidget *m_Parent;
	ImageVectorType m_ImageVector;

private:
	bool m_IsSetup;
};

}
}
} //end namespace

#else
typedef struct WidgetInterface WidgetInterface;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__STDC__) || defined(__cplusplus)
#ifdef WIN32
	extern __declspec( dllexport ) isis::viewer::widget::WidgetInterface *loadWidget();
	extern __declspec( dllexport ) const isis::util::PropertyMap* getProperties();
#else
	extern isis::viewer::widget::WidgetInterface *loadWidget();
	extern const isis::util::PropertyMap* getProperties();
#endif
#else
#ifdef WIN32
	extern __declspec( dllexport ) WidgetInterface *loadWidget();
	extern __declspec( dllexport ) const isis::util::PropertyMap* getProperties();
#else
	extern WidgetInterface *loadWidget();
	extern const isis::util::PropertyMap* getProperties();
#endif
#endif

#ifdef __cplusplus
}
#endif



#endif