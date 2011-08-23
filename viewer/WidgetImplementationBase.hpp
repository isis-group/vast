#ifndef WIDGETIMPLEMENTATIONBASE_HPP
#define WIDGETIMPLEMENTATIONBASE_HPP

#include "common.hpp"
#include "QViewerCore.hpp"
#include <QtGui>

namespace isis {
   
namespace viewer {

class QViewerCore;
    
class WidgetImplenentationBase  {
    
public:
    virtual WidgetImplenentationBase *createSharedWidget( QWidget *parent, PlaneOrientation orienation ) = 0;
    
    virtual void setZoom( float zoom ) = 0;
    virtual void redraw();
    virtual void voxelCoordsChanged( util::ivector4 ) = 0;
    virtual void physicalCoordsChanged( util::fvector4 ) = 0;
    virtual void zoomChanged( float zoomFactor ) = 0;
    
    
    virtual void addImage( const boost::shared_ptr<ImageHolder> image ) = 0;
    
    virtual void setWidgetName( std::string name ) { m_WidgetName = name; }
    
    virtual QWidget* getParent( ) const { return m_Parent; }
    
    virtual PlaneOrientation getPlaneOrientation() { return m_PlaneOrientation; }

    
protected:
    WidgetImplenentationBase( QViewerCore* core, QWidget* parent, PlaneOrientation orientation );
    
    QViewerCore *m_ViewerCore;
    PlaneOrientation m_PlaneOrientation;
    QWidget* m_Parent;
    std::string m_WidgetName;
};


}} //end namespace


#endif