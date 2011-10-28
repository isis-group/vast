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
	m_ViewWidgetArrangement = Default;
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

UICore::ViewWidgetEnsembleType UICore::createViewWidgetEnsemble(const std::string& widgetType, boost::shared_ptr< ImageHolder > image, bool show)
{
	ViewWidgetEnsembleType ensemble = createViewWidgetEnsemble( widgetType, show );
	ensemble[0].widgetImplementation->addImage( image );
	ensemble[1].widgetImplementation->addImage( image );
	ensemble[2].widgetImplementation->addImage( image );
	return ensemble;
}


UICore::ViewWidgetEnsembleType UICore::createViewWidgetEnsemble(const std::string& widgetType, bool show )
{
	ViewWidgetEnsembleType ensemble;
	ensemble[0] =  createViewWidget( widgetType, axial );
	ensemble[1] =  createViewWidget( widgetType, sagittal );
	ensemble[2] =  createViewWidget( widgetType, coronal );
	if( show ) {
		switch ( m_ViewWidgetArrangement ) {
			case Default: {
				if( m_EnsembleList.size() > 0 ) {
					int currentRow = m_MainWindow->getUI().centralGridLayout->rowCount();
					m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[0].dockWidget, currentRow, 0 );
					m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[1].dockWidget, currentRow, 1 );
					m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[2].dockWidget, currentRow, 2 );
				} else {
					m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[0].dockWidget, 0, 0 );
					m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[1].dockWidget, 0, 1 );
					m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[2].dockWidget, 1, 0 );
				}
				break;
			} case InRow: {
				int currentRow = m_MainWindow->getUI().centralGridLayout->rowCount();
				m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[0].dockWidget, currentRow, 0 );
				m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[1].dockWidget, currentRow, 1 );
				m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[2].dockWidget, currentRow, 2 );
				break;
			} case InColumn: {
				int currentColumn = m_MainWindow->getUI().centralGridLayout->columnCount();
				m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[0].dockWidget, 0, currentColumn );
				m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[1].dockWidget, 1, currentColumn );
				m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[2].dockWidget, 2, currentColumn );
			}
		}
	}
	m_EnsembleList.push_back( ensemble );
	return ensemble;	
}

UICore::ViewWidgetEnsembleType UICore::removeViewWidgetEnsemble( isis::viewer::QWidgetImplementationBase* widgetImplementation )
{
	BOOST_FOREACH( ViewWidgetEnsembleListType::reference ref, m_EnsembleList ) {
		if( ref[0].widgetImplementation == widgetImplementation 
			|| ref[1].widgetImplementation == widgetImplementation
			|| ref[2].widgetImplementation == widgetImplementation ) {
			return removeViewWidgetEnsemble( ref );
		}
	}
}

UICore::ViewWidgetEnsembleType UICore::removeViewWidgetEnsemble(UICore::ViewWidgetEnsembleType ensemble)
{
	for( unsigned short i = 0; i < 3; i++ ) {
		m_MainWindow->getUI().centralGridLayout->removeWidget( ensemble[i].dockWidget );
	}
	return ensemble;
}






UICore::ViewWidget UICore::createViewWidget( const std::string& widgetType, PlaneOrientation planeOrientation )
{
	
	QFrame *frameWidget = new QFrame();
	
	QDockWidget *dockWidget = createDockingEnsemble(frameWidget);
	dockWidget->setMinimumHeight( m_UICoreProperties.getPropertyAs<uint16_t>("maxWidgetHeight") );
	dockWidget->setMinimumWidth( m_UICoreProperties.getPropertyAs<uint16_t>("maxWidgetWidth") );
	dockWidget->setWidget( frameWidget );
	frameWidget->setParent( dockWidget );

#warning this has to be done with the help of a widget factor. nasty this way
	QWidgetImplementationBase *widgetImpl = new qt::QImageWidgetImplementation(m_Core, frameWidget, planeOrientation );
	registerWidget( widgetImpl );
	ViewWidget viewWidget;
	viewWidget.dockWidget = dockWidget;
	viewWidget.frame = frameWidget;
	viewWidget.widgetImplementation = widgetImpl;
	viewWidget.planeOrientation = planeOrientation;
	viewWidget.widgetType = widgetType;
	return viewWidget;

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


bool UICore::registerWidget(QWidgetImplementationBase* widget)
{
	if( std::find( m_WidgetList.begin(), m_WidgetList.end(), widget ) != m_WidgetList.end() ) {
		LOG( Runtime, warning ) << "Widget with id" << widget->getWidgetName() << "!";
		return false;
	}
	m_WidgetList.push_back(widget);
	return true;

}
	
}}