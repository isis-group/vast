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


UICore::RowType UICore::appendWidgetRow(const std::string& widgetType )
{
	RowType row;
	int currentRow = m_MainWindow->getUI().widgetGridLayout->rowCount();
	row[0] =  createWidgetEnsemble( widgetType, axial );
	row[1] =  createWidgetEnsemble( widgetType, sagittal );
	row[2] =  createWidgetEnsemble( widgetType, coronal );
	m_MainWindow->getUI().widgetGridLayout->addWidget( row[0].dockWidget, currentRow, 0 );
	m_MainWindow->getUI().widgetGridLayout->addWidget( row[1].dockWidget, currentRow, 1 );
	m_MainWindow->getUI().widgetGridLayout->addWidget( row[2].dockWidget, currentRow, 2 );
	m_EnsembleMap[row[0].viewWidget] = row[0];
	m_EnsembleMap[row[1].viewWidget] = row[1];
	m_EnsembleMap[row[2].viewWidget] = row[2];
	m_RowList.push_back(row);
	return row;	
}


UICore::WidgetEnsemble UICore::appendWidget( const std::string& widgetType, PlaneOrientation planeOrientation)
{
	WidgetEnsemble ret = createWidgetEnsemble( widgetType, planeOrientation);
	m_MainWindow->getUI().widgetGridLayout->addWidget( ret.dockWidget );
	return ret;
}

UICore::WidgetEnsemble UICore::appendWidget( const std::string& widgetType, int row, int column, PlaneOrientation planeOrientation)
{
	WidgetEnsemble ret = createWidgetEnsemble( widgetType, planeOrientation);
	m_MainWindow->getUI().widgetGridLayout->addWidget( ret.dockWidget, row, column );
	return ret;
}


bool UICore::removeWidget(const QWidgetImplementationBase *widget )
{
	EnsembleMapType::const_iterator iter = m_EnsembleMap.find( widget );
	if( iter != m_EnsembleMap.end() ) {
		m_MainWindow->getUI().widgetGridLayout->removeWidget( iter->second.dockWidget );
		m_EnsembleMap.erase(widget);
		m_ViewWidgetList.erase( std::find( m_ViewWidgetList.begin(), m_ViewWidgetList.end(), widget ) );
		return true;
	} else {
		LOG( Runtime, error ) << "Tried to remove widget " << widget 
			<< " of type " << widget->getWidgetName() << ". But there is no such widget!";
		return false;
	}
}



ui::UICore::WidgetEnsemble UICore::createWidgetEnsemble( const std::string& widgetType, PlaneOrientation planeOrientation )
{
	QDockWidget *dockWidget = new QDockWidget( m_MainWindow );
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
	m_Core->registerWidget( viewWidget );
	WidgetEnsemble ensemble;
	ensemble.dockWidget = dockWidget;
	ensemble.frame = frameWidget;
	ensemble.viewWidget = viewWidget;
	ensemble.planeOrientation = planeOrientation;
	ensemble.widgetType = widgetType;
	ensemble.ID = m_ViewWidgetList.size();
	m_ViewWidgetList.push_back( ensemble.viewWidget );
	m_EnsembleMap[ensemble.viewWidget] = ensemble;
	return ensemble;

}

void UICore::reloadPluginsToGUI()
{
	m_MainWindow->reloadPluginsToGUI( m_Core );
}

	
}}}