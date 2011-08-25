#ifndef MAINWINDOWBASE_HPP
#define MAINWINDOWBASE_HPP

#include <QtGui>
#include "ui_isisViewerMain.h"
#include "QPreferencesDialog.hpp"
#include "QPlottingDialog.hpp"

namespace isis
{
namespace viewer
{

class MainWindowBase : public QMainWindow
{
	Q_OBJECT
public:
	MainWindowBase( QWidget *parent = 0 );
	

protected Q_SLOTS:

Q_SIGNALS:


protected:
	QPreferencesDialog* m_PreferencesDialog;
	QPlottingDialog* m_PlottingDialog;
	Ui::isisViewerMain ui;
	QScrollArea *sa;
	QString m_CurrentPath;
};



}
} // end namespace



#endif
