#ifndef QWIDGETIMPLEMENTATIONBASE_HPP
#define QWIDGETIMPLEMENTATIONBASE_HPP

#include "common.hpp"
#include "imageholder.hpp"
#include <QtGui>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace isis
{

namespace viewer
{

class QViewerCore;

class QWidgetImplementationBase
{

public:
	virtual QWidgetImplementationBase *createSharedWidget( QWidget *parent, PlaneOrientation orienation ) = 0;

	virtual void updateScene( bool center ) = 0;
	virtual void setZoom( float zoom ) = 0;
	virtual void addImage( const boost::shared_ptr<ImageHolder> image ) = 0;
	virtual bool removeImage( const boost::shared_ptr< ImageHolder > image ) = 0;
	virtual void setWidgetName( const std::string &name ) = 0;
	virtual std::string getWidgetName() const = 0;
	virtual void setInterpolationType( InterpolationType interpolation ) = 0;

	virtual QWidget *getParent( ) const { return m_Parent; }
	virtual PlaneOrientation getPlaneOrientation() { return m_PlaneOrientation; }

	boost::uuids::uuid getID() const { return m_ID; }

protected:
	QWidgetImplementationBase( QViewerCore *core, QWidget *parent, PlaneOrientation orientation );

	QViewerCore *m_ViewerCore;
	PlaneOrientation m_PlaneOrientation;
	QWidget *m_Parent;
	boost::uuids::uuid m_ID;
};


}
} //end namespace


#endif