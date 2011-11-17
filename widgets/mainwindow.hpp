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


public Q_SLOTS:
	void openImage();
	void saveImage();
	void saveImageAs();
	void saveAllImages();
	void loadSettings();
	void saveSettings();
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
	virtual void keyPressEvent( QKeyEvent * );

private:
	Ui::vastMainWindow m_UI;
	QViewerCore *m_ViewerCore;

	QToolBar *m_Toolbar;
	
	QSpinBox *m_RadiusSpin;
	QPushButton *m_LogButton;

	QAction *m_ActionReset_Scaling;



};


}
}





#endif