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
	std::string output = m_Bridge->run( m_Interface.pythonInput->toPlainText().toStdString() );
	m_Interface.output->addItem(output.c_str());
}
