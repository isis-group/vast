#include "vastPlugin_TimePlotter.hpp"



namespace isis
{
namespace viewer
{
namespace plugin
{


bool TimePlotter::call()
{
	m_PlotterDialog = new PlotterDialog( parentWidget, viewerCore );
	m_PlotterDialog->show();
	return true;
}




}
}
}