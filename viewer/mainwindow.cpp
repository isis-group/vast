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
	m_Core( core ),
	m_Toolbar( new QToolBar(this) )
{
	m_UI.setupUi( this );
	setupBasicElements();
	
	connect( m_UI.action_Open_image, SIGNAL( triggered() ), this, SLOT( openImage() ) );
	connect( m_UI.action_Save_Image, SIGNAL( triggered()), this, SLOT(saveImage()));
	connect( m_UI.actionSave_Image, SIGNAL( triggered()), this, SLOT( saveImageAs()));
	connect( m_UI.actionOpen_directory, SIGNAL( triggered()), this, SLOT( openDir()));
		
	//toolbar stuff
	m_Toolbar->setOrientation( Qt::Horizontal );
	m_Toolbar->setMinimumHeight( 20 );
	m_Toolbar->setMaximumHeight( 30 );
	addToolBar( Qt::TopToolBarArea, m_Toolbar );
	m_Toolbar->addAction( m_UI.action_Open_image );
	m_Toolbar->addAction( m_UI.actionOpen_directory);
	m_Toolbar->addAction( m_UI.action_Save_Image);
	m_Toolbar->addAction( m_UI.actionSave_Image);
	m_Toolbar->addSeparator();
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

		UICore::ViewWidgetEnsembleType ensemble = m_Core->getUI()->createViewWidgetEnsemble("");
		BOOST_FOREACH( QStringList::const_reference filename, filenames ) {
			std::stringstream ss;
			ss << "Loading image " << filename.toStdString() << "...";
			m_Core->getUI()->showStatus( ss.str() );
			std::list<data::Image> tempImgList = isis::data::IOFactory::load( filename.toStdString() , "", "" );
			pathList.push_back( filename.toStdString() );
			BOOST_FOREACH( std::list<data::Image>::const_reference image, tempImgList ) {
				imgList.push_back( image );
				boost::shared_ptr<ImageHolder> imageHolder = m_Core->addImage( image, ImageHolder::anatomical_image );
				m_Core->attachImageToWidget( imageHolder, ensemble[0].widgetImplementation );
				m_Core->attachImageToWidget( imageHolder, ensemble[1].widgetImplementation );
				m_Core->attachImageToWidget( imageHolder, ensemble[2].widgetImplementation );
			}
		}
		m_Core->getUI()->showStatus( "Done." );
		m_Core->getUI()->rearrangeViewWidgets();
		m_Core->getUI()->refreshUI();
		m_Core->updateScene( isFirstImage );
	}
}
void MainWindow::openDir()
{
	QString dir = QFileDialog::getExistingDirectory( this, tr( "Open directory" ), m_Core->getCurrentPath().c_str() );

	if( dir.size() ) {
		m_Core->setCurrentPath( dir.toStdString() );
		bool isFirstImage = m_Core->getDataContainer().size() == 0;
		std::stringstream ss;
		ss << "Opening directory " << dir.toStdString() << "...";
		m_Core->getUI()->showStatus( ss.str() );
		std::list<data::Image> imageList = data::IOFactory::load( dir.toStdString(), "", "" );
		m_Core->getUI()->showStatus( "Done." );
		if( imageList.size() ) {
			m_Core->addImageList( imageList, ImageHolder::anatomical_image );
			m_Core->getUI()->showStatus( "Loading images..." );
			m_Core->updateScene();
		} else {
			m_Core->getUI()->showStatus( "No images to load" );
		}
		
	}
}


void MainWindow::saveImage()
{
	if( !m_Core->getCurrentImage()->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ).size() ) {
		QMessageBox msgBox;
		msgBox.setText( "The image that is currently selected has no changes! Won´t save anything." );
		msgBox.exec();
	} else {
		QMessageBox msgBox;
		msgBox.setIcon( QMessageBox::Information );
		std::stringstream text;
		text << "This will overwrite" << m_Core->getCurrentImage()->getFileNames().front() << " !";
		msgBox.setText( text.str().c_str() );
		msgBox.setInformativeText( "Do you want to proceed?" );
		std::stringstream detailedText;
		detailedText << "Changed attributes: " << std::endl;
		BOOST_FOREACH( util::slist::const_reference attrChanged, m_Core->getCurrentImage()->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ) ) {
			detailedText << " >> " << attrChanged << std::endl;
		}
		msgBox.setDetailedText( detailedText.str().c_str() );
		msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
		msgBox.setDefaultButton( QMessageBox::No );

		switch ( msgBox.exec() ) {
			case QMessageBox::No:
			return;
			break;
		case QMessageBox::Yes:
			std::stringstream ss;
			ss << "Saving image to " << m_Core->getCurrentImage()->getFileNames().front() << "...";
			m_Core->getUI()->showStatus( ss.str() );
			isis::data::IOFactory::write( *m_Core->getCurrentImage()->getISISImage(), m_Core->getCurrentImage()->getFileNames().front(), "", "" );
			m_Core->getUI()->showStatus("Done.");
			break;
		}
	}
}

void MainWindow::saveImageAs()
{
	std::stringstream fileFormats;
	fileFormats << "Image files (" << getFileFormatsAsString( std::string( "*." ) ) << ")";
	QString filename = QFileDialog::getSaveFileName( this,
					tr( "Save Image As..." ),
					m_Core->getCurrentPath().c_str(),
					tr( fileFormats.str().c_str() ) );

	if( filename.size() ) {
		std::stringstream ss;
		ss << "Saving image to " << filename.toStdString() << "...";
		isis::data::IOFactory::write( *m_Core->getCurrentImage()->getISISImage(), filename.toStdString(), "", "" );
		m_Core->getUI()->showStatus("Done.");
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

			//optionally add plugin to the toolbar
			if( !plugin->getToolbarIcon()->isNull() ) {
				processAction->setIcon( *plugin->getToolbarIcon() );
				m_Toolbar->addAction( processAction );
			}

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