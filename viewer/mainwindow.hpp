#ifndef ISISMAINWINDOW_HPP
#define ISISMAINWINDOW_HPP

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "qviewercore.hpp"
#include "preferenceDialog.hpp"

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

protected:
	virtual void setupBasicElements();
private:
	Ui::vastMainWindow m_UI;
	QViewerCore *m_Core;

	QToolBar *m_Toolbar;
	isis::viewer::widget::PreferencesDialog *m_PreferencesDialog;

};


}
}





#endif