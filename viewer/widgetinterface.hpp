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

#include "common.hpp"
#include "imageholder.hpp"
#include <QtGui>

namespace isis
{

namespace viewer
{

class QViewerCore;

class WidgetInterface
{

public:
	typedef std::vector<boost::shared_ptr<ImageHolder> > ImageVectorType;

	virtual void setEnableCrosshair( bool enable ) = 0;
	virtual void updateScene() = 0;
	virtual void setZoom( float zoom ) = 0;
	virtual void addImage( const boost::shared_ptr<ImageHolder> image ) = 0;
	virtual bool removeImage( const boost::shared_ptr< ImageHolder > image ) = 0;
	virtual void setWidgetName( const std::string &name ) = 0;
	virtual std::string getWidgetName() const = 0;
	virtual void setInterpolationType( InterpolationType interpolation ) = 0;
	virtual void setMouseCursorIcon( QIcon ) = 0;
	virtual void setCrossHairColor( QColor ) {}
	virtual void setCrossHairWidth( int ) {}

	virtual QWidget *getParent( ) const { return m_Parent; }
	virtual PlaneOrientation getPlaneOrientation() { return m_PlaneOrientation; }

	virtual void lookAtPhysicalCoords( const util::fvector4 &physicalCoords ) = 0;

	ImageVectorType getImageVector() const { return m_ImageVector; }

protected:
	WidgetInterface( QViewerCore *core, QWidget *parent, PlaneOrientation orientation )
		: m_ViewerCore( core ),
		  m_PlaneOrientation( orientation ),
		  m_Parent( parent ) {}

	QViewerCore *m_ViewerCore;
	PlaneOrientation m_PlaneOrientation;
	QWidget *m_Parent;
	ImageVectorType m_ImageVector;
};


}
} //end namespace


#endif