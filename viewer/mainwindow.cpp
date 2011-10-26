#include "mainwindow.hpp"
#include <iostream>
#include <QGridLayout>


namespace isis {
namespace viewer {
namespace ui {
	
MainWindow::MainWindow( QWidget *parent ) :
	QMainWindow( parent )
{
	m_UI.setupUi(this);
	setupBasicElements();
}


void MainWindow::setupBasicElements()
{
	//here we setup the basic elements of the viewer
	
	
}

void MainWindow::reloadPluginsToGUI(QViewerCore* core)
{
		//adding all processes to the process (plugin) menu and connect the action to the respective call functions
	QMenu *processMenu = new QMenu( QString( "Plugins" ) );

	if( core->getPlugins().size() ) {
		getUI().menubar->addMenu( processMenu );

		QSignalMapper *signalMapper = new QSignalMapper( this );
		BOOST_FOREACH( ViewerCoreBase::PluginListType::const_reference plugin, core->getPlugins() ) {
			std::list<std::string> sepName = isis::util::stringToList<std::string>( plugin->getName(), boost::regex( "/" ) );
			QMenu *tmpMenu = processMenu;
			std::list<std::string>::iterator iter = sepName.begin();

			for ( unsigned short i = 0; i < sepName.size() - 1; iter++, i++ ) {
				tmpMenu = tmpMenu->addMenu( iter->c_str() );

			}

			QAction *processAction = new QAction( QString( ( --sepName.end() )->c_str() ), this );

#warning add plugin icon to toolbar
// 			//optionally add plugin to the toolbar
// 			if( !plugin->getToolbarIcon()->isNull() ) {
// 				processAction->setIcon( *plugin->getToolbarIcon() );
// 				m_Toolbar->addAction( processAction );
// 			}

			processAction->setStatusTip( QString( plugin->getTooltip().c_str() ) );
			signalMapper->setMapping( processAction, QString( plugin->getName().c_str() ) );
			tmpMenu->addAction( processAction );
			connect( processAction, SIGNAL( triggered() ), signalMapper, SLOT( map() ) );

		}
		connect( signalMapper, SIGNAL( mapped( QString ) ), core, SLOT( callPlugin( QString ) ) );
	}
}

	
}}}