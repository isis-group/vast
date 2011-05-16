#include "SubViewWindow.hpp"

namespace isis {
namespace viewer {
	
SubViewWindow::SubViewWindow(QWidget* parent, Qt::WindowFlags flags): QMainWindow(parent, flags)
{
	ui.setupUi(this);
}

	
}}