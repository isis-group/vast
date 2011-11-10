#ifndef LOGGINGDIALOG_HPP
#define LOGGINGDIALOG_HPP
#include <QDialog>

#include "ui_loggingDialog.h"
#include "qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{

class LoggingDialog : public QDialog
{
	Q_OBJECT
public:
	LoggingDialog( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	void synchronize();

private:
	Ui::logDialog m_Interface;
	QViewerCore *m_ViewerCore;
};


}
}
}


#endif