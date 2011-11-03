#ifndef ISISMAINWINDOW_HPP
#define ISISMAINWINDOW_HPP

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "qviewercore.hpp"
#include "preferenceDialog.hpp"
#include "nativeimageops.hpp"
#include "CoreUtils/progressfeedback.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{
class PreferencesDialog;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow( QViewerCore *core );

	virtual ~MainWindow() {};

	const Ui::vastMainWindow &getUI() const { return m_UI; }
	Ui::vastMainWindow &getUI() { return m_UI; }

	void reloadPluginsToGUI( );


public Q_SLOTS:
	void showPreferences();
	void openImage();
	void openDir();
	void saveImage();
	void saveImageAs();
	void loadSettings();
	void saveSettings();
	void closeEvent( QCloseEvent * );
	void findGlobalMin();
	void findGlobalMax();
	void spinRadiusChanged(int);

private:
	Ui::vastMainWindow m_UI;
	QViewerCore *m_ViewerCore;

	QToolBar *m_Toolbar;
	isis::viewer::widget::PreferencesDialog *m_PreferencesDialog;
	QSpinBox *m_RadiusSpin;

};


}
}





#endif