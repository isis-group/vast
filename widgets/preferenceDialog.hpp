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
 * preferencesDialog.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef QPREFERNCESDIALOG_HPP
#define QPREFERNCESDIALOG_HPP

#include "ui_preferences.h"
#include "qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{

class PreferencesDialog : public QDialog
{
	Q_OBJECT;

public:
	PreferencesDialog( QWidget *parent, QViewerCore *core );
protected:
	Ui::preferencesDialog preferencesUi;


public Q_SLOTS:
	void closeEvent( QCloseEvent * );
	void loadSettings();
	void saveSettings();
	void apply( int dummy = 0 );
	void toggleMultithreading( bool );
	void toggleUseAllThreads( bool );
	void numberOfThreadsChanged( int );
	void screenshotXChanged( int );
	virtual void showEvent( QShowEvent * ) { loadSettings(); }

private:
	QViewerCore *m_ViewerCore;

};

}
}
}



#endif