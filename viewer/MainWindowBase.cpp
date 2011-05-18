#include "MainWindowBase.hpp"
#include <iostream>

namespace isis
{
namespace viewer
{

MainWindowBase::MainWindowBase( QWidget *parent )
	: QMainWindow( parent ),
	  m_CurrentPath( QDir::currentPath() )
{
	sa = new QScrollArea(this);
	ui.setupUi( this );
	this->setCentralWidget( sa );
	sa->setLayout(ui.gridLayout);
}

}
} // end namespace