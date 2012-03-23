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
class WidgetEnsemble;

namespace widget {

class WidgetInterface
{
public:
	virtual void setup( QViewerCore *core, QWidget *parent, PlaneOrientation orientation ){
		m_ViewerCore = core;
		m_Parent = parent;
		m_PlaneOrientation = orientation;
		m_IsSetup = true;
	};

	virtual unsigned short getNumberOfInstancesInEnsemble() const = 0;
	virtual void setEnableCrosshair( bool enable ) = 0;
	virtual void updateScene() = 0;
	virtual void setZoom( float zoom ) = 0;
	virtual void addImage( const boost::shared_ptr<ImageHolder> image ) {};
	virtual bool removeImage( const boost::shared_ptr< ImageHolder > image ) {};
	virtual std::string getWidgetName() const = 0;
	virtual void setInterpolationType( InterpolationType interpolation ) = 0;
	virtual void setMouseCursorIcon( QIcon ) = 0;
	virtual void setCrossHairColor( QColor ) {}
	virtual void setCrossHairWidth( int ) {}
	virtual QWidget *getOptionWidget() = 0;
	virtual bool hasOptionWidget() const = 0;

	QWidget *getParent( ) const { return m_Parent; }
	PlaneOrientation getPlaneOrientation() { return m_PlaneOrientation; }

	virtual void lookAtPhysicalCoords( const util::fvector4 &physicalCoords ) = 0;

	void setWidgetEnsemble( boost::shared_ptr<WidgetEnsemble> ensemble ) { m_WidgetEnsemble = ensemble; }
	const boost::shared_ptr<WidgetEnsemble> getWidgetEnsemble() const { return m_WidgetEnsemble; }
	boost::shared_ptr<WidgetEnsemble> getWidgetEnsemble() { return m_WidgetEnsemble; }

	std::string widget_file;
protected:
	WidgetInterface() : m_IsSetup(false) {}

	QViewerCore *m_ViewerCore;
	PlaneOrientation m_PlaneOrientation;
	QWidget *m_Parent;
	boost::shared_ptr<WidgetEnsemble> m_WidgetEnsemble;

private:
	bool m_IsSetup;
};

}
}
} //end namespace

#else
typedef struct WidgetInterface WidgetInterface;
#endif

#include "widgetensemble.hpp"

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