#ifndef FILEDIALOG_HPP
#define FILEDIALOG_HPP


#include "ui_fileOpenWidget.h"
#include "qviewercore.hpp"
#include <QFileDialog>
#include <QCompleter>
#include <boost/filesystem.hpp>

namespace isis
{
namespace viewer
{
namespace widget
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

	static bool checkIfPathIsValid( QString path, unsigned short &validFiles, const std::string &suffix, FileMode mode = OPEN_FILE, bool acceptNoSuffix = false );

private:
	FileMode m_Mode;
	QFileDialog m_FileDialog;
	Ui::fileDialog m_Interface;
	QViewerCore *m_ViewerCore;
	QStringList m_PathList;
	std::string m_Suffix;
	std::string m_Dialect;
	ImageHolder::ImageType m_ImageType;
	QCompleter *m_Completer;

};


}
}
}





#endif
