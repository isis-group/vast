#include "widgetImplementationBase.hpp"

namespace isis
{
namespace viewer
{


QWidgetImplementationBase::QWidgetImplementationBase( QViewerCore *core, QWidget *parent, PlaneOrientation orientation )
	: m_ViewerCore( core ),
	  m_PlaneOrientation( orientation ),
	  m_Parent( parent ),
	  m_ID( boost::uuids::random_generator()() )
{

}


}
} // end namespace