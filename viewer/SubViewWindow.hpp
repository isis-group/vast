#ifndef SUBVIEWWINDOW_HPP
#define SUBVIEWWINDOW_HPP

#include "ui_isisSubViewWidget.h"
#include <QtGui>

namespace isis {
namespace viewer {

class SubViewWindow : public QMainWindow
{
public:	
	explicit SubViewWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	
private:
	Ui::SubViewWindow ui;
};

}}
#endif
