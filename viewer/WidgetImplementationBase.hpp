#ifndef WIDGETIMPLEMENTATIONBASE_HPP
#define WIDGETIMPLEMENTATIONBASE_HPP

#include "common.hpp"
#include "ImageHolder.hpp"
#include <QtGui>

namespace isis
{

namespace viewer
{

class QViewerCore;

class WidgetImplementationBase
{

public:
	virtual WidgetImplementationBase *createSharedWidget( QWidget *parent, PlaneOrientation orienation ) = 0;

	virtual void setZoom( float zoom ) = 0;



	virtual void addImage( const boost::shared_ptr<ImageHolder> image ) = 0;
	virtual void setWidgetName( std::string name ) { m_WidgetName = name; }

	virtual QWidget *getParent( ) const { return m_Parent; }

	virtual PlaneOrientation getPlaneOrientation() { return m_PlaneOrientation; }
	virtual void setInterpolationType( InterpolationType interpolation ) = 0;


protected:
	WidgetImplementationBase( QViewerCore *core, QWidget *parent, PlaneOrientation orientation );

	QViewerCore *m_ViewerCore;
	PlaneOrientation m_PlaneOrientation;
	QWidget *m_Parent;
	std::string m_WidgetName;
};


}
} //end namespace


#endif