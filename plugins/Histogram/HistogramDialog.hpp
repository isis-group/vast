#ifndef HISTOGRAMDIALOG_HPP
#define HISTOGRAMDIALOG_HPP

#include "ui_histogramDialog.h"
#include "qviewercore.hpp"

namespace isis {
namespace viewer {
namespace plugin {
	

class HistogramDialog : public QDialog
{
	Q_OBJECT
public:
	HistogramDialog( QWidget *parent, QViewerCore *core );
	
private:
	Ui::histogramDialog m_Interface;
	QViewerCore *m_ViewerCore;
};
	
	
}}}



#endif