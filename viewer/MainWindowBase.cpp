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
	ui.setupUi( this );
	m_PreferencesDialog = new QPreferencesDialog();
}

}
} // end namespace