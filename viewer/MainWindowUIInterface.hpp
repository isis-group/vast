#ifndef MAINWINDOWUIINTERFACE_HPP
#define MAINWINDOWUIINTERFACE_HPP

#include "MainWindow.hpp"

namespace isis {
namespace viewer {


class MainWindowUIInterface : public MainWindow
{
	Q_OBJECT;
public:
	MainWindowUIInterface(QViewerCore* core);
		

protected:
	void connectSignals();
	void setInitialState();
	QViewerCore* m_ViewerCore;
		
	
public Q_SLOTS:
	void showControlPanel(bool);
	void toggleAxialView(bool);
	void toggleCoronalView(bool);
	void toggleSagittalView(bool);
	void toggleViews( PlaneOrientation orientation, bool toggled);
	
}; 



}}

#endif
