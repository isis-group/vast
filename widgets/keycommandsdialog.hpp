#ifndef KEYCOMMANDSDIALOG_HPP
#define KEYCOMMANDSDIALOG_HPP

#include <iostream>
#include "ui_keyCommands.h"
#include <boost/foreach.hpp>

namespace isis
{
namespace viewer
{
namespace ui
{

class KeyCommandsDialog : public QDialog
{
	Q_OBJECT
public:
	KeyCommandsDialog( QWidget *parent );

private:
	Ui::keyCommandsDialog m_Interface;
	std::list<std::pair< std::string, std::string> > m_KeyCommands;
	void insertIntoTable();


};

}
}
}

#endif
