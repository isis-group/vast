#include "PythonInterpreterDialog.hpp"


isis::viewer::plugin::PyhtonInterpreterDialog::PyhtonInterpreterDialog(QWidget* parent, isis::viewer::QViewerCore* core)
	: QDialog(parent),
	m_ViewerCore(core)
{
	m_Interface.setupUi(this);
	connect( m_Interface.run, SIGNAL( clicked()), this, SLOT( run()) );
	m_Bridge.reset( new PythonBridge( m_ViewerCore ) );
}


void isis::viewer::plugin::PyhtonInterpreterDialog::run()
{
	m_Bridge->run( m_Interface.pythonInput->toPlainText().toStdString() );
}
