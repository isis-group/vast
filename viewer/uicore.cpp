#include "uicore.hpp"
#include <DataStorage/io_interface.h>
#include "QImageWidgetImplementation.hpp"
#include <QSignalMapper>

namespace isis {
namespace viewer {
	
	
UICore::UICore( QViewerCore *core )
	: m_MainWindow( new MainWindow( core ) ),
	m_Core( core )
{
	m_UICoreProperties.setPropertyAs<uint16_t>("maxWidgetHeight", 200);
	m_UICoreProperties.setPropertyAs<uint16_t>("maxWidgetWidth", 200);
	
	m_VoxelInformationWidget = new widget::VoxelInformationWidget( m_MainWindow, core );
	m_ImageStackWidget = new widget::ImageStackWidget( m_MainWindow, core );
}

void UICore::setOptionPosition(UICore::OptionPosition pos)
{
	switch (pos) {
	case bottom:
		m_MainWindow->getUI().bottomGridLayout->addWidget( createDockingEnsemble( m_ImageStackWidget ), 0,1 );
		m_MainWindow->getUI().bottomGridLayout->addWidget( createDockingEnsemble( m_VoxelInformationWidget ), 0,0 );
		break;
	case top:
		m_MainWindow->getUI().topGridLayout->addWidget( createDockingEnsemble( m_ImageStackWidget ), 0,1 );
		m_MainWindow->getUI().topGridLayout->addWidget( createDockingEnsemble( m_VoxelInformationWidget ), 0,0 );
		break;
	case central11:
		QGridLayout *layout = new QGridLayout(  );
		layout->setVerticalSpacing(0);
		layout->setHorizontalSpacing(0);
		layout->setMargin(0);
		layout->setContentsMargins(0,0,0,0);
		QFrame * frame = new QFrame( m_MainWindow );
		frame->setLayout( layout );
		m_MainWindow->getUI().centralGridLayout->addWidget( frame, 1, 1);
		layout->addWidget( createDockingEnsemble( m_VoxelInformationWidget ), 0,0);
		layout->addWidget( createDockingEnsemble( m_ImageStackWidget ), 1,0);
		break;
	}
}

void UICore::reensembleViewWidgets()
{
	BOOST_FOREACH( EnsembleMapType::const_reference ref, m_EnsembleMap ) {
	}
}

void UICore::showMainWindow()
{

	m_MainWindow->show();

} 


QDockWidget* UICore::createDockingEnsemble( QWidget* widget )
{
	QDockWidget *dockWidget = new QDockWidget( m_MainWindow );
	dockWidget->setFloating( false );
	widget->setContentsMargins(0,0,0,0);
	dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
	dockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	dockWidget->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum ) );
	dockWidget->setWidget( widget );
	dockWidget->setContentsMargins(0,0,0,0);
	return dockWidget;
	
}


UICore::RowType UICore::appendWidgetRow(const std::string& widgetType )
{
	RowType row;
	int currentRow = m_MainWindow->getUI().centralGridLayout->rowCount();
	row[0] =  createWidgetEnsemble( widgetType, axial );
	row[1] =  createWidgetEnsemble( widgetType, sagittal );
	row[2] =  createWidgetEnsemble( widgetType, coronal );
	m_MainWindow->getUI().centralGridLayout->addWidget( row[0].dockWidget, currentRow, 0 );
	m_MainWindow->getUI().centralGridLayout->addWidget( row[1].dockWidget, currentRow, 1 );
	m_MainWindow->getUI().centralGridLayout->addWidget( row[2].dockWidget, currentRow, 2 );
	m_EnsembleMap[row[0].viewWidget] = row[0];
	m_EnsembleMap[row[1].viewWidget] = row[1];
	m_EnsembleMap[row[2].viewWidget] = row[2];
	m_RowList.push_back(row);
	return row;	
}


UICore::WidgetEnsemble UICore::appendWidget( const std::string& widgetType, PlaneOrientation planeOrientation)
{
	WidgetEnsemble ret = createWidgetEnsemble( widgetType, planeOrientation);
	m_MainWindow->getUI().centralGridLayout->addWidget( ret.dockWidget );
	return ret;
}

UICore::WidgetEnsemble UICore::appendWidget( const std::string& widgetType, int row, int column, PlaneOrientation planeOrientation, Qt::Alignment alignment )
{
	WidgetEnsemble ret = createWidgetEnsemble( widgetType, planeOrientation);
	m_MainWindow->getUI().centralGridLayout->addWidget( ret.dockWidget, row, column, alignment );
	return ret;
}


bool UICore::removeWidget(const QWidgetImplementationBase *widget )
{
	EnsembleMapType::const_iterator iter = m_EnsembleMap.find( widget );
	if( iter != m_EnsembleMap.end() ) {
		m_MainWindow->getUI().centralGridLayout->removeWidget( iter->second.dockWidget );
		m_EnsembleMap.erase(widget);
		m_ViewWidgetList.erase( std::find( m_ViewWidgetList.begin(), m_ViewWidgetList.end(), widget ) );
		return true;
	} else {
		LOG( Runtime, error ) << "Tried to remove widget " << widget 
			<< " of type " << widget->getWidgetName() << ". But there is no such widget!";
		return false;
	}
}



UICore::WidgetEnsemble UICore::createWidgetEnsemble( const std::string& widgetType, PlaneOrientation planeOrientation )
{
	
	QFrame *frameWidget = new QFrame();
	
	QDockWidget *dockWidget = createDockingEnsemble(frameWidget);
	dockWidget->setMinimumHeight( m_UICoreProperties.getPropertyAs<uint16_t>("maxWidgetHeight") );
	dockWidget->setMinimumWidth( m_UICoreProperties.getPropertyAs<uint16_t>("maxWidgetWidth") );
	dockWidget->setWidget( frameWidget );
	frameWidget->setParent( dockWidget );

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
	m_MainWindow->reloadPluginsToGUI();
}

void UICore::synchronize()
{
	m_ImageStackWidget->updateImageStack( );
	m_VoxelInformationWidget->synchronize();

	
}


	
}}