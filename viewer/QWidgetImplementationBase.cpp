#include "QWidgetImplementationBase.hpp"

namespace isis
{
namespace viewer
{


QWidgetImplementationBase::QWidgetImplementationBase( QViewerCore *core, QWidget *parent, PlaneOrientation orientation )
	: m_ViewerCore( core ),
	  m_PlaneOrientation( orientation ),
	  m_Parent( parent )
{

}


}
} // end namespace