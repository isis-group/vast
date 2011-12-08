#ifndef ISISMAINWINDOW_HPP
#define ISISMAINWINDOW_HPP

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "qviewercore.hpp"
#include "preferenceDialog.hpp"
#include "nativeimageops.hpp"
#include <CoreUtils/progressfeedback.hpp>
#include "scalingWidget.hpp"
#include "loggingDialog.hpp"
#include "filedialog.hpp"
#include "startwidget.hpp"
#include "keycommandsdialog.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{
class PreferencesDialog;
class ScalingWidget;
class LoggingDialog;
class FileDialog;
class StartWidget;
class KeyCommandsDialog;
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
	
	widget::PreferencesDialog *preferencesDialog;
	widget::LoggingDialog *loggingDialog;
	widget::FileDialog *fileDialog;
	widget::StartWidget *startWidget;
	widget::ScalingWidget *scalingWidget;
	widget::KeyCommandsDialog *keyCommandsdialog;


public Q_SLOTS:
	void openImage();
	void saveImage();
	void saveImageAs();
	void saveAllImages();
	void closeEvent( QCloseEvent * );
	void findGlobalMin();
	void findGlobalMax();
	void spinRadiusChanged( int );
	void showScalingOption();
	void ignoreOrientation( bool );
	void propagateZooming( bool );
	void showLoggingDialog();
	void refreshUI();
	void resetScaling();
	void autoScaling();
	void toggleZMapMode(bool);
	void showKeyCommandDialog();
	void loadSettings();
	void saveSettings();
	

private:
	Ui::vastMainWindow m_UI;
	QViewerCore *m_ViewerCore;

	QToolBar *m_Toolbar;
	
	QSpinBox *m_RadiusSpin;
	QPushButton *m_LogButton;

	QAction *m_ActionReset_Scaling;
	QAction *m_ActionAuto_Scaling;


};


}
}





#endif