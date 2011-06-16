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

	QViewerCore* m_ViewerCore;
		
	
public Q_SLOTS:
	void showControlPanel(bool);
	void showPlotting();
	void toggleAxialView(bool);
	void toggleCoronalView(bool);
	void toggleSagittalView(bool);
	void openImageAsZMap();
	void openImageAsAnatomicalImage();
	void openPreferences();
	
	void triggeredMakeCurrentImage( bool );
	void doubleClickedMakeCurrentImage( QListWidgetItem * );
	
	
	void toggleViews( PlaneOrientation orientation, bool toggled);
	
}; 



}}

#endif
