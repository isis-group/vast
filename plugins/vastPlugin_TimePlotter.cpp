#include "vastPlugin_TimePlotter.hpp"


namespace isis {
namespace viewer {
namespace plugin {
	
TimePlotter::TimePlotter()
{
	
}

	
bool TimePlotter::call()
{
	m_PlotterDialog = new PlotterDialog(parent);
	std::cout << "Calling TimePlotter::call" << std::endl;
	m_PlotterDialog->show();
	return true;
}

TimePlotter::~TimePlotter()
{

}



}}}