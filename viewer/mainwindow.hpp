#ifndef ISISMAINWINDOW_HPP
#define ISISMAINWINDOW_HPP

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "qviewercore.hpp"

namespace isis {
namespace viewer {
namespace ui {
	
class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow( QWidget *parent = 0 );
	
	virtual ~MainWindow() {};
	
	const Ui::vastMainWindow &getUI() const { return m_UI; }
	Ui::vastMainWindow &getUI() { return m_UI; }
	
	void reloadPluginsToGUI( QViewerCore *core );
	
protected:
	virtual void setupBasicElements();
private:
	Ui::vastMainWindow m_UI;
	
	
};
	
	
}}}





#endif