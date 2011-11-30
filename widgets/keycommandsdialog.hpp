#ifndef KEYCOMMANDSDIALOG_HPP
#define KEYCOMMANDSDIALOG_HPP


#include "ui_keyCommands.h"

namespace isis {
namespace viewer {
namespace widget {

class KeyCommandsDialog : public QDialog
{
	Q_OBJECT
public:
	KeyCommandsDialog( QWidget *parent ) : QDialog(parent) {
		m_Interface.setupUi(this);
	}
	
private:
	Ui::keyCommandsDialog m_Interface;
	
	
};

}}}

#endif
