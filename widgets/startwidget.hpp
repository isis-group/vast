#ifndef STARTWIDGET_HPP
#define STARTWIDGET_HPP


#include "ui_start.h"
#include "qviewercore.hpp"

namespace isis {
namespace viewer {
namespace widget {
	
	
class StartWidget : public QDialog
{
	Q_OBJECT
public:
	StartWidget( QWidget *parent, QViewerCore *core );
	
public Q_SLOTS:
	void openImageButtonClicked();
	void showMe( bool asStartDialog = false );
	void statusChanged( QString );
	void showMeChecked( bool );
	virtual void showEvent( QShowEvent *) ;
	void openPath();

private:
	Ui::startDialog m_Interface;
	QViewerCore *m_ViewerCore;
};
	
	
}}}




#endif
