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

	m_WidgetType = type_qt;
	m_PlottingDialog = new QPlottingDialog(this);
}

}
} // end namespace