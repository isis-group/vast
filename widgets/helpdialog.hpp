#ifndef HELPDIALOG_HPP
#define HELPDIALOG_HPP

#include "ui_helpDialog.h"

namespace isis {
namespace viewer {
namespace widget {
	

class HelpDialog : public QDialog
{
	Q_OBJECT
public:
	HelpDialog( QWidget *parent );

private:
	Ui::helpDialog m_Interface;
};

	
}}}


#endif