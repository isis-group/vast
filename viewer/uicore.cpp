#include "uicore.hpp"
#include <DataStorage/io_interface.h>
#include "QImageWidgetImplementation.hpp"
#include <QSignalMapper>

namespace isis
{
namespace viewer
{


UICore::UICore( QViewerCore *core )
	: m_MainWindow( new MainWindow( core ) ),
	  m_Core( core )
{
	m_UICoreProperties.setPropertyAs<uint16_t>( "maxWidgetHeight", 200 );
	m_UICoreProperties.setPropertyAs<uint16_t>( "maxWidgetWidth", 200 );

	m_VoxelInformationWidget = new widget::VoxelInformationWidget( m_MainWindow, core );
	m_ImageStackWidget = new widget::ImageStackWidget( m_MainWindow, core );
	m_ViewWidgetArrangement = Default;
	m_VoxelInformationDockWidget = createDockingEnsemble( m_VoxelInformationWidget );
	m_ImageStackDockWidget = createDockingEnsemble( m_ImageStackWidget );
	m_RowCount = m_MainWindow->getUI().centralGridLayout->rowCount();
	
	
}

void UICore::setOptionPosition( UICore::OptionPosition pos )
{
	switch ( pos ) {
	case bottom:
		m_MainWindow->getUI().bottomGridLayout->addWidget( m_VoxelInformationDockWidget, 0, 1 );
		m_MainWindow->getUI().bottomGridLayout->addWidget( m_ImageStackDockWidget, 0, 0 );
		break;
	case top:
		m_MainWindow->getUI().topGridLayout->addWidget( m_ImageStackDockWidget, 0, 1 );
		m_MainWindow->getUI().topGridLayout->addWidget( m_VoxelInformationDockWidget, 0, 0 );
		break;
	case central11:
		QGridLayout *layout = new QGridLayout(  );
		layout->setVerticalSpacing( 0 );
		layout->setHorizontalSpacing( 0 );
		layout->setMargin( 0 );
		layout->setContentsMargins( 0, 0, 0, 0 );
		QFrame *frame = new QFrame( m_MainWindow );
		frame->setLayout( layout );
		m_MainWindow->getUI().centralGridLayout->addWidget( frame, 1, 1 );
		layout->addWidget( m_VoxelInformationDockWidget, 0, 0 );
		layout->addWidget( m_ImageStackDockWidget, 1, 0 );
		break;
	}
}


void UICore::showMainWindow()
{

	m_MainWindow->show();

}


QDockWidget *UICore::createDockingEnsemble( QWidget *widget )
{
	QDockWidget *dockWidget = new QDockWidget( m_MainWindow );
	dockWidget->setFloating( false );
	widget->setContentsMargins( 0, 0, 0, 0 );
	dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
	dockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	dockWidget->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum ) );
	dockWidget->setWidget( widget );
	dockWidget->setContentsMargins( 0, 0, 0, 0 );
	return dockWidget;

}

UICore::ViewWidgetEnsembleType UICore::createViewWidgetEnsemble( const std::string &widgetType, boost::shared_ptr< ImageHolder > image, bool show )
{
	ViewWidgetEnsembleType ensemble = createViewWidgetEnsemble( widgetType, show );
	ensemble[0].widgetImplementation->addImage( image );
	ensemble[1].widgetImplementation->addImage( image );
	ensemble[2].widgetImplementation->addImage( image );
	return ensemble;
}


UICore::ViewWidgetEnsembleType UICore::createViewWidgetEnsemble( const std::string &widgetType, bool show )
{
	ViewWidgetEnsembleType ensemble;
	ensemble[0] =  createViewWidget( widgetType, axial );
	ensemble[1] =  createViewWidget( widgetType, sagittal );
	ensemble[2] =  createViewWidget( widgetType, coronal );
	if( show ) {
		attachViewWidgetEnsemble(ensemble);
	}
	m_EnsembleList.push_back( ensemble );
	return ensemble;
}

void UICore::removeViewWidgetEnsemble( isis::viewer::QWidgetImplementationBase *widgetImplementation )
{
	BOOST_FOREACH( ViewWidgetEnsembleListType::reference ref, m_EnsembleList ) {
		if( ref[0].widgetImplementation == widgetImplementation
			|| ref[1].widgetImplementation == widgetImplementation
			|| ref[2].widgetImplementation == widgetImplementation ) {
			removeViewWidgetEnsemble( ref );
		}
	}
}

void UICore::removeViewWidgetEnsemble( UICore::ViewWidgetEnsembleType ensemble )
{
	for( unsigned short i = 0; i < 3; i++ ) {
		m_MainWindow->getUI().centralGridLayout->removeWidget( ensemble[i].dockWidget );
	}
	m_EnsembleList.erase( std::find( m_EnsembleList.begin(), m_EnsembleList.end(), ensemble) );
}

UICore::ViewWidgetEnsembleType UICore::detachViewWidgetEnsemble(QWidgetImplementationBase* widgetImplementation)
{
		BOOST_FOREACH( ViewWidgetEnsembleListType::reference ref, m_EnsembleList ) {
		if( ref[0].widgetImplementation == widgetImplementation
			|| ref[1].widgetImplementation == widgetImplementation
			|| ref[2].widgetImplementation == widgetImplementation ) {
			return detachViewWidgetEnsemble( ref );
		}
	}
}

UICore::ViewWidgetEnsembleType  UICore::detachViewWidgetEnsemble(UICore::ViewWidgetEnsembleType ensemble)
{
	for( unsigned short i = 0; i < 3; i++ ) {
		m_MainWindow->getUI().centralGridLayout->removeWidget( ensemble[i].dockWidget );
	}
	return ensemble;
}

void UICore::attachViewWidgetEnsemble(UICore::ViewWidgetEnsembleType ensemble)
{
	switch ( m_ViewWidgetArrangement ) {
		case Default: {
			if( m_EnsembleList.size() > 0 ) {
				m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[0].dockWidget, m_RowCount, 0 );
				m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[1].dockWidget, m_RowCount, 1 );
				m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[2].dockWidget, m_RowCount, 2 );
			} else {
				m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[0].dockWidget, 0, 0 );
				m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[1].dockWidget, 0, 1 );
				m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[2].dockWidget, 1, 0 );
			}
			break;
		}
		case InRow: {
			m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[0].dockWidget, m_RowCount, 0 );
			m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[1].dockWidget, m_RowCount, 1 );
			m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[2].dockWidget, m_RowCount, 2 );
			break;
		}
		case InColumn: {
			int currentColumn = m_MainWindow->getUI().centralGridLayout->columnCount() ;
			m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[0].dockWidget, 0, currentColumn );
			m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[1].dockWidget, 1, currentColumn );
			m_MainWindow->getUI().centralGridLayout->addWidget( ensemble[2].dockWidget, 2, currentColumn );
		}
	}
	m_RowCount++;
}

void UICore::rearrangeViewWidgets()
{
	m_RowCount = 0;
	BOOST_FOREACH( ViewWidgetEnsembleListType::const_reference ensemble, m_EnsembleList ) 
	{
		attachViewWidgetEnsemble( detachViewWidgetEnsemble( ensemble ) );
	}
	if( m_EnsembleList.size() > 1 ) {
		setOptionPosition( bottom );
	} else {
		setOptionPosition( central11 );
	}
}

UICore::ViewWidget UICore::createViewWidget( const std::string &widgetType, PlaneOrientation planeOrientation )
{

	QFrame *frameWidget = new QFrame();

	QDockWidget *dockWidget = createDockingEnsemble( frameWidget );
	dockWidget->setMinimumHeight( m_UICoreProperties.getPropertyAs<uint16_t>( "maxWidgetHeight" ) );
	dockWidget->setMinimumWidth( m_UICoreProperties.getPropertyAs<uint16_t>( "maxWidgetWidth" ) );
	dockWidget->setWidget( frameWidget );
	frameWidget->setParent( dockWidget );

#warning this has to be done with the help of a widget factor. nasty this way
	QWidgetImplementationBase *widgetImpl = new qt::QImageWidgetImplementation( m_Core, frameWidget, planeOrientation );
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


bool UICore::registerWidget( QWidgetImplementationBase *widget )
{
	if( std::find( m_WidgetList.begin(), m_WidgetList.end(), widget ) != m_WidgetList.end() ) {
		LOG( Runtime, warning ) << "Widget with id" << widget->getWidgetName() << "!";
		return false;
	}

	m_WidgetList.push_back( widget );
	return true;

}



}
}