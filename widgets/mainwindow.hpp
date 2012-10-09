/****************************************************************
 *
 * <Copyright information>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Author: Erik Türke, tuerke@cbs.mpg.de
 *
 * mainwindow.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef ISISMAINWINDOW_HPP
#define ISISMAINWINDOW_HPP

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "../viewer/qviewercore.hpp"


#include <CoreUtils/progressfeedback.hpp>



namespace isis
{
namespace viewer
{
namespace ui
{
class PreferencesDialog;
class ScalingWidget;
class LoggingDialog;
class FileDialog;
class StartWidget;
class KeyCommandsDialog;
class AboutDialog;
class HelpDialog;

}

namespace operation
{
class NativeImageOps;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	friend class UICore;
	MainWindow( QViewerCore *core );

	virtual ~MainWindow() {};

	const Ui::vastMainWindow &getInterface() const { return m_Interface; }
	Ui::vastMainWindow &getInterface() { return m_Interface; }

	void reloadPluginsToGUI( );

	ui::PreferencesDialog *preferencesDialog;
	ui::LoggingDialog *loggingDialog;
	ui::FileDialog *fileDialog;
	ui::StartWidget *startWidget;
	ui::ScalingWidget *scalingWidget;
	ui::KeyCommandsDialog *keyCommandsdialog;
	ui::HelpDialog *helpDialog;
	ui::AboutDialog *aboutDialog;


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
	void showKeyCommandDialog();
	void loadSettings();
	void saveSettings();
	void createScreenshot();
	void toggleSagittalView( bool );
	void toggleAxialView( bool );
	void toggleCoronalView( bool );
	void updateRecentOpenList();
	void openRecentPath( QString );
	void toggleGeometrical( bool );

protected:
	void dropEvent( QDropEvent * );


private:


	Ui::vastMainWindow m_Interface;
	QViewerCore *m_ViewerCore;

	QToolBar *m_Toolbar;

	QSpinBox *m_RadiusSpin;
	QAction *m_RadiusSpinAction;
	QLabel *m_StatusTextLabel;

	QAction *m_ActionReset_Scaling;
	QAction *m_ActionAuto_Scaling;

	QLabel *m_StatusMovieLabel;
	QMovie *m_StatusMovie;


};


}
}
#include "preferenceDialog.hpp"
#include "scalingWidget.hpp"
#include "../viewer/nativeimageops.hpp"
#include "loggingDialog.hpp"
#include "filedialog.hpp"
#include "startwidget.hpp"
#include "keycommandsdialog.hpp"
#include "aboutDialog.hpp"
#ifdef HAVE_WEBKIT
#include "helpdialog.hpp"
#endif //HAVE_WEBKIT


#endif
