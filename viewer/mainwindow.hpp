#ifndef ISISMAINWINDOW_HPP
#define ISISMAINWINDOW_HPP

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "qviewercore.hpp"

namespace isis
{
namespace viewer
{

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
	void openImage();
	void loadSettings();
	void saveSettings();
	void closeEvent( QCloseEvent * );

protected:
	virtual void setupBasicElements();
private:
	Ui::vastMainWindow m_UI;
	QViewerCore *m_Core;

};


}
}





#endif