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

class WidgetInterface
{

public:
	typedef std::vector<boost::shared_ptr<ImageHolder> > ImageVectorType;
	virtual WidgetInterface *createSharedWidget( QWidget *parent, PlaneOrientation orienation ) = 0;

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

	virtual QWidget *getParent( ) const { return m_Parent; }
	virtual PlaneOrientation getPlaneOrientation() { return m_PlaneOrientation; }

	virtual bool lookAtPhysicalCoords( const util::fvector4 &physicalCoords ) = 0;

	boost::uuids::uuid getID() const { return m_ID; }

	ImageVectorType getImageVector() const { return m_ImageVector; }

protected:
	WidgetInterface( QViewerCore *core, QWidget *parent, PlaneOrientation orientation )
		: m_ViewerCore( core ),
		m_PlaneOrientation( orientation ),
		m_Parent( parent ),
		m_ID( boost::uuids::random_generator()() ) {}

	QViewerCore *m_ViewerCore;
	PlaneOrientation m_PlaneOrientation;
	QWidget *m_Parent;
	boost::uuids::uuid m_ID;
	ImageVectorType m_ImageVector;
};


}
} //end namespace


#endif