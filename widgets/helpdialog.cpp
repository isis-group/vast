#include "helpdialog.hpp"

#include <QMessageBox>

isis::viewer::widget::HelpDialog::HelpDialog(QWidget* parent)
	: QDialog(parent)
{
	m_Interface.setupUi(this);
	if( std::string( VAST_DOC_PATH ).size() ) {
		std::string indexFile = std::string ( VAST_DOC_PATH ) + std::string("/index.html");
		m_Interface.helpView->load( QUrl(indexFile.c_str() ) );
	} else {
		QMessageBox msgBox;
		msgBox.setText( "Could not find help path for vast." );
		msgBox.exec();
	}
}
