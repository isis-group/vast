#include "keycommandsdialog.hpp"


isis::viewer::widget::KeyCommandsDialog::KeyCommandsDialog(QWidget* parent)
	: QDialog(parent)
{
	m_Interface.setupUi(this);
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Zoom in", "+ / Mousewheel"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Zoom out", "- / Mousewheel"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Center crosshair", "Space"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Propagate zooming", "P, Z"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Ignore orientation", "I, O"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Toggle zmap mode", "T, Z, M"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Show preferences", "S, P"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Show labels", "S, L"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Show crosshair", "S, C"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Find local minimum", "F, M, I"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Find local maximum", "F, M, A"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Show scaling dialog", "S, S"));	
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Reset Scaling", "R, S"));	
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Auto Scaling", "A, S"));	
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Open image", "Ctrl+O"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Save image", "Ctrl+S"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Save image as", "Ctrl+Shift+S"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Save all images", "Ctrl+Alt+Shift+S"));
	m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Exit program", "Ctrl+Q"));
	
	m_Interface.tableWidget->setRowCount(m_KeyCommands.size());
	m_Interface.tableWidget->adjustSize();
	m_Interface.tableWidget->setColumnWidth(0, m_Interface.tableWidget->width() );
	m_Interface.tableWidget->setColumnWidth(1, m_Interface.tableWidget->width() / 2);
	insertIntoTable();
}

void isis::viewer::widget::KeyCommandsDialog::insertIntoTable()
{
	unsigned short index = 0;
	typedef std::list< std::pair< std::string, std::string > >::const_reference CommandsRef;
	BOOST_FOREACH( CommandsRef commands, m_KeyCommands ) 
	{
		m_Interface.tableWidget->setItem(index, 0, new QTableWidgetItem( commands.first.c_str() ) );
		m_Interface.tableWidget->setItem(index, 1, new QTableWidgetItem( commands.second.c_str() ) );
		index++;
	}
}