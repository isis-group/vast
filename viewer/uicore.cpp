#include "uicore.hpp"
#include <DataStorage/io_interface.h>
#include "QImageWidgetImplementation.hpp"
#include <QSignalMapper>

namespace isis {
namespace viewer {
namespace ui {
	
	
	
UICore::UICore( QViewerCore *core )
	: m_MainWindow( new MainWindow() ),
	m_Core( core )
{
	m_UICoreProperties.setPropertyAs<uint16_t>("maxWidgetHeight", 200);
	m_UICoreProperties.setPropertyAs<uint16_t>("maxWidgetWidth", 200);
}

	
void UICore::showMainWindow()
{
	m_MainWindow->show();
}


bool UICore::appendWidget(const std::string& name, const std::string& widgetType, PlaneOrientation planeOrientation)
{
	m_MainWindow->getUI().widgetGridLayout->addWidget( createWidgetEnsemble( name, widgetType, planeOrientation) );
}

bool UICore::appendWidget(const std::string& name, const std::string& widgetType, int row, int column, PlaneOrientation planeOrientation)
{
	m_MainWindow->getUI().widgetGridLayout->addWidget( createWidgetEnsemble( name, widgetType, planeOrientation ), row, column );
}

bool UICore::removeWidget(const std::string& name)
{
	if( m_ViewWidgetMap.find( name) != m_ViewWidgetMap.end() ) {
		m_MainWindow->getUI().widgetGridLayout->removeWidget( m_ViewWidgetMap.at(name).dockWidget );
		return true;
	} else {
		LOG( Runtime, error ) << "Tried to remove widget " << name << ". But there is no such widget!";
		return false;
	}
}



QDockWidget* UICore::createWidgetEnsemble( const std::string &name, const std::string& widgetType, PlaneOrientation planeOrientation )
{
	QDockWidget *dockWidget = new QDockWidget( QString(name.c_str()), m_MainWindow );
	dockWidget->setFloating( false );
	dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
	dockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	dockWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	dockWidget->setMinimumHeight( m_UICoreProperties.getPropertyAs<uint16_t>("maxWidgetHeight") );
	dockWidget->setMinimumWidth( m_UICoreProperties.getPropertyAs<uint16_t>("maxWidgetWidth") );
	
	QFrame *frameWidget = new QFrame( dockWidget );
	dockWidget->setWidget( frameWidget );

#warning this has to be done with the help of a widget factor. nasty this way
	QWidgetImplementationBase *viewWidget = new qt::QImageWidgetImplementation(m_Core, frameWidget, planeOrientation );
	m_Core->registerWidget(name, viewWidget );
	WidgetEnsemble ensemble;
	ensemble.dockWidget = dockWidget;
	ensemble.frame = frameWidget;
	ensemble.viewWidget = viewWidget;
	ensemble.planeOrientation = planeOrientation;
	ensemble.widgetType = widgetType;
	m_ViewWidgetMap[name] = ensemble;
	return dockWidget;

}

void UICore::reloadPluginsToGUI()
{
	m_MainWindow->reloadPluginsToGUI( m_Core );
}


	
}}}