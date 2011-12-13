#ifndef PYTHONINTERPRETERDIALOG_HPP
#define PYTHONINTERPRETERDIALOG_HPP

#include "ui_pythonInterpreter.h"
#include "qviewercore.hpp"
#include <QDialog>

#include "PythonBridge.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{


class PyhtonInterpreterDialog : public QDialog
{
	Q_OBJECT

public:
	PyhtonInterpreterDialog( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	virtual void run();

private:
	Ui::pythonDialog m_Interface;
	QViewerCore *m_ViewerCore;
	boost::scoped_ptr< PythonBridge > m_Bridge;
};


}
}
}


#endif