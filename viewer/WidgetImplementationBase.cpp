#include "WidgetImplementationBase.hpp"

namespace isis {
namespace viewer {
    
    
WidgetImplenentationBase::WidgetImplenentationBase( QViewerCore *core, QWidget *parent, PlaneOrientation orientation )
    : m_ViewerCore( core ),
      m_PlaneOrientation( orientation ),
      m_Parent( parent )
{
    ( new QVBoxLayout( parent ) )->addWidget( this );
    
}

    
}} // end namespace