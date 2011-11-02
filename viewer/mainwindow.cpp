#include "mainwindow.hpp"
#include <iostream>
#include <QGridLayout>
#include "DataStorage/io_factory.hpp"
#include "uicore.hpp"


namespace isis
{
namespace viewer
{

MainWindow::MainWindow( QViewerCore *core ) :
	m_Core( core )
{
	m_UI.setupUi( this );
	setupBasicElements();
	connect( m_UI.action_Open_image, SIGNAL( triggered() ), this, SLOT( openImage() ) );

}

void MainWindow::openImage()
{
	ImageHolder::ImageType type = ImageHolder::anatomical_image;
	std::string title( "Open Image" );
	std::stringstream fileFormats;
	fileFormats << "Image files (" << getFileFormatsAsString( std::string( "*." ) ) << ")";
	QStringList filenames = QFileDialog::getOpenFileNames( this,
							tr( title.c_str() ),
							m_Core->getCurrentPath().c_str(),
							tr( fileFormats.str().c_str() ) );


	if( !filenames.empty() ) {
		QDir dir;
		m_Core->setCurrentPath( dir.absoluteFilePath( filenames.front() ).toStdString() );
		bool isFirstImage = m_Core->getDataContainer().size() == 0;
		std::list<data::Image> imgList;
		util::slist pathList;
		
		if( (m_Core->getDataContainer().size() + filenames.size()) > 1 ) {
			m_Core->getUI()->setViewWidgetArrangement( isis::viewer::UICore::InRow );
		} else {
			m_Core->getUI()->setViewWidgetArrangement( isis::viewer::UICore::Default );
		}
		
		BOOST_FOREACH( QStringList::const_reference filename, filenames ) {
			std::list<data::Image> tempImgList = isis::data::IOFactory::load( filename.toStdString() , "", "" );
			pathList.push_back( filename.toStdString() );
			BOOST_FOREACH( std::list<data::Image>::const_reference image, tempImgList ) {
				imgList.push_back( image );
				m_Core->getUI()->createViewWidgetEnsemble("", m_Core->addImage( image, ImageHolder::anatomical_image ) );
			}
		}
		m_Core->getUI()->rearrangeViewWidgets();
		m_Core->getUI()->refreshUI();
		m_Core->updateScene( isFirstImage );
	}
}


void MainWindow::setupBasicElements()
{
	//here we setup the basic elements of the viewer
	loadSettings();


}

void MainWindow::reloadPluginsToGUI()
{
	//adding all processes to the process (plugin) menu and connect the action to the respective call functions
	QMenu *processMenu = new QMenu( QString( "Plugins" ) );

	if( m_Core->getPlugins().size() ) {
		getUI().menubar->addMenu( processMenu );

		QSignalMapper *signalMapper = new QSignalMapper( this );
		BOOST_FOREACH( ViewerCoreBase::PluginListType::const_reference plugin, m_Core->getPlugins() ) {
			std::list<std::string> sepName = isis::util::stringToList<std::string>( plugin->getName(), boost::regex( "/" ) );
			QMenu *tmpMenu = processMenu;
			std::list<std::string>::iterator iter = sepName.begin();

			for ( unsigned short i = 0; i < sepName.size() - 1; iter++, i++ ) {
				tmpMenu = tmpMenu->addMenu( iter->c_str() );

			}

			QAction *processAction = new QAction( QString( ( --sepName.end() )->c_str() ), this );

#warning add plugin icon to toolbar
			//          //optionally add plugin to the toolbar
			//          if( !plugin->getToolbarIcon()->isNull() ) {
			//              processAction->setIcon( *plugin->getToolbarIcon() );
			//              m_Toolbar->addAction( processAction );
			//          }

			processAction->setStatusTip( QString( plugin->getTooltip().c_str() ) );
			signalMapper->setMapping( processAction, QString( plugin->getName().c_str() ) );
			tmpMenu->addAction( processAction );
			connect( processAction, SIGNAL( triggered() ), signalMapper, SLOT( map() ) );

		}
		connect( signalMapper, SIGNAL( mapped( QString ) ), m_Core, SLOT( callPlugin( QString ) ) );
	}
}

void MainWindow::loadSettings()
{
	m_Core->getSettings()->beginGroup( "MainWindow" );
	resize( m_Core->getSettings()->value( "size", QSize( 900, 900 ) ).toSize() );
	move( m_Core->getSettings()->value( "pos", QPoint( 0, 0 ) ).toPoint() );
	if( m_Core->getSettings()->value( "maximized", false ).toBool() ) {
		showMaximized();
	}
	m_Core->getSettings()->endGroup();
	m_Core->getSettings()->beginGroup( "UserProfile" );
	m_Core->getOption()->propagateZooming = m_Core->getSettings()->value( "propagateZooming", false ).toBool();
	m_Core->getSettings()->endGroup();
}

void MainWindow::closeEvent(QCloseEvent* )
{
	saveSettings();
}

void MainWindow::saveSettings()
{
	//saving the preferences to the profile file
	m_Core->getSettings()->beginGroup( "MainWindow" );
	m_Core->getSettings()->setValue( "size", size() );
	m_Core->getSettings()->setValue( "maximized", isMaximized() );
	m_Core->getSettings()->setValue( "pos", pos() );
	m_Core->getSettings()->endGroup();
	m_Core->getSettings()->sync();
}


}
}