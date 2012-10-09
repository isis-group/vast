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
 * fileDialog.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef FILEDIALOG_HPP
#define FILEDIALOG_HPP


#include "ui_fileOpenWidget.h"
#include "../viewer/qviewercore.hpp"
#include <QFileDialog>
#include <QCompleter>
#include <boost/filesystem.hpp>

namespace isis
{
namespace viewer
{
namespace ui
{

class FileDialog : public QDialog
{
	Q_OBJECT
public:
	enum FileMode { OPEN_FILE, OPEN_DIR };
	FileDialog( QWidget *parent, QViewerCore *core  );

	void setMode( FileMode mode ) { m_Mode = mode;  }

public Q_SLOTS:
	void browse();
	void parsePath();
	void openPath();
	void imageTypeChanged( int );
	void rfChanged( int );
	virtual void closeEvent( QCloseEvent * );
	virtual void showEvent( QShowEvent * );
	void modeChanged();
	void setup();
	void advancedChecked( bool );
	void favoritesChecked( bool );
	void addToFavList();
	void removeFromFavList();
	void onFavListClicked();
	void fileDialectChanged( QString );
	void openingMethodChanged();

	static bool checkIfPathIsValid( QString path, unsigned short &validFiles, const util::istring &suffix, FileMode mode = OPEN_FILE, bool acceptNoSuffix = false );

private:
	FileMode m_Mode;
	QFileDialog m_FileDialog;
	Ui::fileDialog m_Interface;
	QViewerCore *m_ViewerCore;
	QStringList m_PathList;
	util::istring m_Suffix;
	util::istring m_Dialect;
	ImageHolder::ImageType m_ImageType;
	QCompleter *m_Completer;

};


}
}
}





#endif
