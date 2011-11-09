#ifndef FILEDIALOG_HPP
#define FILEDIALOG_HPP


#include "ui_fileOpenWidget.h"
#include <QFileDialog>
#include <boost/filesystem.hpp>
#include "qviewercore.hpp"

namespace isis {
namespace viewer {
namespace widget {
	
class FileDialog : public QDialog
{
	Q_OBJECT
public:
	enum FileMode { OPEN_FILE, OPEN_DIR, SAVE_AS };
	FileDialog( QWidget *parent, QViewerCore *core  );
	
	void setMode( FileMode mode ) { m_Mode = mode;  }
	
public Q_SLOTS:
	void browse();
	void parsePath(QString path);
	void openPath();
	void saveToFile();
	void imageTypeChanged(int);
	void rfChanged(int);
	virtual void closeEvent( QCloseEvent *);
	virtual void showEvent( QShowEvent *);
	void modeChanged();
	void setup();
	
	bool checkIfPathIsValid( QString path, unsigned short &validFiles, bool acceptNoSuffix = false );
	
private:
	FileMode m_Mode;
	QFileDialog m_FileDialog;
	Ui::fileDialog m_Interface;
	QViewerCore *m_ViewerCore;
	QStringList m_PathList;
	std::string m_Suffix;
	std::string m_Dialect;
	ImageHolder::ImageType m_ImageType;
	util::slist m_FileFormatList;
	
	
};
	
	
}}}





#endif
