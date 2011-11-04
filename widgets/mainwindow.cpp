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
	m_ViewerCore( core ),
	m_Toolbar( new QToolBar( this ) ),
	m_PreferencesDialog( new widget::PreferencesDialog( this, core ) ),
	m_ScalingWidget( new widget::ScalingWidget( this, core ) ),
	m_RadiusSpin(new QSpinBox(this))
{
	m_UI.setupUi( this );
	loadSettings();
	
	connect( m_UI.action_Open_image, SIGNAL( triggered() ), this, SLOT( openImage() ) );
	connect( m_UI.action_Save_Image, SIGNAL( triggered() ), this, SLOT( saveImage() ) );
	connect( m_UI.actionSave_Image, SIGNAL( triggered() ), this, SLOT( saveImageAs() ) );
	connect( m_UI.actionOpen_directory, SIGNAL( triggered() ), this, SLOT( openDir() ) );
	connect( m_UI.action_Preferences, SIGNAL( triggered() ), this, SLOT( showPreferences() ) );
	connect( m_UI.actionFind_Global_Min, SIGNAL( triggered()), this, SLOT( findGlobalMin()));
	connect( m_UI.actionFind_Global_Max, SIGNAL( triggered()), this, SLOT( findGlobalMax()));
	connect( m_UI.actionShow_Labels, SIGNAL( triggered(bool)), m_ViewerCore, SLOT( setShowLabels(bool)));
	connect( m_RadiusSpin, SIGNAL(valueChanged(int)), this, SLOT( spinRadiusChanged(int)));
	connect( m_UI.actionShow_scaling_option, SIGNAL( triggered()), this, SLOT( showScalingOption()));

	//toolbar stuff
	m_Toolbar->setOrientation( Qt::Horizontal );
	m_Toolbar->setMinimumHeight( 20 );
	m_Toolbar->setMaximumHeight( 30 );
	addToolBar( Qt::TopToolBarArea, m_Toolbar );
	m_Toolbar->addAction( m_UI.action_Open_image );
	m_Toolbar->addAction( m_UI.actionOpen_directory );
	m_Toolbar->addAction( m_UI.action_Save_Image );
	m_Toolbar->addAction( m_UI.actionSave_Image );
	m_Toolbar->addSeparator();
	m_Toolbar->addAction( m_UI.actionShow_Labels );
	m_Toolbar->addAction( m_UI.action_Preferences );
	m_Toolbar->addAction( m_UI.actionShow_scaling_option );
	m_Toolbar->addSeparator();
	m_Toolbar->addAction( m_UI.actionFind_Global_Min );
	m_Toolbar->addAction( m_UI.actionFind_Global_Max );
	m_Toolbar->addWidget( m_RadiusSpin );
	m_Toolbar->addSeparator();
	m_RadiusSpin->setMinimum(0);
	m_RadiusSpin->setMaximum(500);
	m_RadiusSpin->setToolTip("Search radius for finding local minimum/maximum. If radius is 0 it will search the entire image.");
	m_UI.statusbar->addPermanentWidget( m_ViewerCore->getProgressFeedback()->getProgressBar() );
	m_ScalingWidget->setVisible(false);

}

void MainWindow::showScalingOption()
{
	m_ScalingWidget->move( QCursor::pos().x() + m_Toolbar->height() / 2, QCursor::pos().y() + m_Toolbar->height() / 2 );
	if( m_UI.actionShow_scaling_option->isChecked() ) {
		m_ScalingWidget->synchronize();
	}
	m_ScalingWidget->setVisible( m_UI.actionShow_scaling_option->isChecked() );
}


void MainWindow::spinRadiusChanged(int radius)
{
	m_ViewerCore->getSettings()->beginGroup("UserProfile");
	m_ViewerCore->getSettings()->setValue("searchRadius", radius);
	m_ViewerCore->getSettings()->endGroup();
}

void MainWindow::openImage()
{
	ImageHolder::ImageType type = ImageHolder::anatomical_image;
	std::string title( "Open Image" );
	std::stringstream fileFormats;
	fileFormats << "Image files (" << getFileFormatsAsString(isis::image_io::FileFormat::read_only, std::string( "*." ) ) << ")";
	QStringList filenames = QFileDialog::getOpenFileNames( this,
							tr( title.c_str() ),
							m_ViewerCore->getCurrentPath().c_str(),
							tr( fileFormats.str().c_str() ) );

	if( !filenames.empty() ) {
		QDir dir;
		m_ViewerCore->setCurrentPath( dir.absoluteFilePath( filenames.front() ).toStdString() );
		bool isFirstImage = m_ViewerCore->getDataContainer().size() == 0;
		std::list<data::Image> imgList;
		util::slist pathList;

		if( ( m_ViewerCore->getDataContainer().size() + filenames.size() ) > 1 ) {
			m_ViewerCore->getUI()->setViewWidgetArrangement( isis::viewer::UICore::InRow );
		} else {
			m_ViewerCore->getUI()->setViewWidgetArrangement( isis::viewer::UICore::Default );
		}

		UICore::ViewWidgetEnsembleType ensemble = m_ViewerCore->getUI()->createViewWidgetEnsemble( "" );
		BOOST_FOREACH( QStringList::const_reference filename, filenames ) {
			std::stringstream ss;
			ss << "Loading image " << filename.toStdString() << "...";
			m_ViewerCore->getUI()->showStatus( ss.str() );
			std::list<data::Image> tempImgList = isis::data::IOFactory::load( filename.toStdString() , "", "" );
			pathList.push_back( filename.toStdString() );
			BOOST_FOREACH( std::list<data::Image>::const_reference image, tempImgList ) {
				imgList.push_back( image );
				boost::shared_ptr<ImageHolder> imageHolder = m_ViewerCore->addImage( image, ImageHolder::anatomical_image );
				m_ViewerCore->attachImageToWidget( imageHolder, ensemble[0].widgetImplementation );
				m_ViewerCore->attachImageToWidget( imageHolder, ensemble[1].widgetImplementation );
				m_ViewerCore->attachImageToWidget( imageHolder, ensemble[2].widgetImplementation );
			}
		}
		m_ViewerCore->getUI()->showStatus( "Done." );
		m_ViewerCore->getUI()->rearrangeViewWidgets();
		m_ViewerCore->getUI()->refreshUI();
		m_ViewerCore->updateScene( isFirstImage );
	}
}
void MainWindow::openDir()
{
	QString dir = QFileDialog::getExistingDirectory( this, tr( "Open directory" ), m_ViewerCore->getCurrentPath().c_str() );

	if( dir.size() ) {
		m_ViewerCore->setCurrentPath( dir.toStdString() );
		bool isFirstImage = m_ViewerCore->getDataContainer().size() == 0;
		std::stringstream ss;
		ss << "Opening directory " << dir.toStdString() << "...";
		m_ViewerCore->getUI()->showStatus( ss.str() );
		std::list<data::Image> imageList = data::IOFactory::load( dir.toStdString(), "", "" );

		if( imageList.size() ) {
			BOOST_FOREACH( std::list<data::Image>::const_reference image, imageList )
			{
				m_ViewerCore->getUI()->createViewWidgetEnsemble("",	m_ViewerCore->addImage( image, ImageHolder::anatomical_image ) );
			}
			m_ViewerCore->getUI()->refreshUI();
			m_ViewerCore->updateScene();
			m_ViewerCore->getUI()->showStatus( "Done." );
		}

	}
}


void MainWindow::saveImage()
{
	if( !m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ).size() ) {
		QMessageBox msgBox;
		msgBox.setText( "The image that is currently selected has no changes! Won´t save anything." );
		msgBox.exec();
	} else {
		QMessageBox msgBox;
		msgBox.setIcon( QMessageBox::Information );
		std::stringstream text;
		text << "This will overwrite" << m_ViewerCore->getCurrentImage()->getFileNames().front() << " !";
		msgBox.setText( text.str().c_str() );
		msgBox.setInformativeText( "Do you want to proceed?" );
		std::stringstream detailedText;
		detailedText << "Changed attributes: " << std::endl;
		BOOST_FOREACH( util::slist::const_reference attrChanged, m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ) ) {
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
			ss << "Saving image to " << m_ViewerCore->getCurrentImage()->getFileNames().front() << "...";
			m_ViewerCore->getUI()->showStatus( ss.str() );
			isis::data::IOFactory::write( *m_ViewerCore->getCurrentImage()->getISISImage(), m_ViewerCore->getCurrentImage()->getFileNames().front(), "", "" );
			m_ViewerCore->getUI()->showStatus( "Done." );
			break;
		}
	}
}

void MainWindow::saveImageAs()
{
	std::stringstream fileFormats;
	fileFormats << "Image files (" << getFileFormatsAsString(isis::image_io::FileFormat::write_only, std::string( "*." ) ) << ")";
	QString filename = QFileDialog::getSaveFileName( this,
					   tr( "Save Image As..." ),
					   m_ViewerCore->getCurrentPath().c_str(),
					   tr( fileFormats.str().c_str() ) );

	if( filename.size() ) {
		std::stringstream ss;
		ss << "Saving image to " << filename.toStdString() << "...";
		isis::data::IOFactory::write( *m_ViewerCore->getCurrentImage()->getISISImage(), filename.toStdString(), "", "" );
		m_ViewerCore->getUI()->showStatus( "Done." );
	}

}


void MainWindow::reloadPluginsToGUI()
{
	//adding all processes to the process (plugin) menu and connect the action to the respective call functions
	QMenu *processMenu = new QMenu( QString( "Plugins" ) );

	if( m_ViewerCore->getPlugins().size() ) {
		getUI().menubar->addMenu( processMenu );

		QSignalMapper *signalMapper = new QSignalMapper( this );
		BOOST_FOREACH( ViewerCoreBase::PluginListType::const_reference plugin, m_ViewerCore->getPlugins() ) {
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
		connect( signalMapper, SIGNAL( mapped( QString ) ), m_ViewerCore, SLOT( callPlugin( QString ) ) );
	}
}

void MainWindow::loadSettings()
{
	m_ViewerCore->getSettings()->beginGroup( "MainWindow" );
	resize( m_ViewerCore->getSettings()->value( "size", QSize( 900, 900 ) ).toSize() );
	move( m_ViewerCore->getSettings()->value( "pos", QPoint( 0, 0 ) ).toPoint() );

	if( m_ViewerCore->getSettings()->value( "maximized", false ).toBool() ) {
		showMaximized();
	}

	m_ViewerCore->getSettings()->endGroup();
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	m_ViewerCore->getOption()->propagateZooming = m_ViewerCore->getSettings()->value( "propagateZooming", false ).toBool();
	m_UI.actionShow_Labels->setChecked( m_ViewerCore->getSettings()->value("showLabels", false).toBool() );
	m_RadiusSpin->setValue( m_ViewerCore->getSettings()->value("searchRadius", 10).toInt());
	m_ViewerCore->getSettings()->endGroup();
}

void MainWindow::closeEvent( QCloseEvent * )
{
	saveSettings();
}

void MainWindow::saveSettings()
{
	//saving the preferences to the profile file
	m_ViewerCore->getSettings()->beginGroup( "MainWindow" );
	m_ViewerCore->getSettings()->setValue( "size", size() );
	m_ViewerCore->getSettings()->setValue( "maximized", isMaximized() );
	m_ViewerCore->getSettings()->setValue( "pos", pos() );
	m_ViewerCore->getSettings()->endGroup();
	m_ViewerCore->getSettings()->sync();
}

void MainWindow::showPreferences()
{
	m_PreferencesDialog->show();
}

void MainWindow::findGlobalMin()
{
	std::string attr;
	if ( m_RadiusSpin->text().toUInt() ) {
		attr = "local";
	} else {
		attr = "global";
	}
	m_ViewerCore->getUI()->showStatus( std::string("Searching for ") + attr + std::string("  minimum...") );
	const util::ivector4 minVoxel = operation::NativeImageOps::getGlobalMin( m_ViewerCore->getCurrentImage(), 
																			 m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<util::ivector4>("voxelCoords"), 
																			 m_RadiusSpin->value());
	m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( minVoxel ) );
	std::stringstream ss;
	ss << "Found " << attr << " minimum at " << minVoxel;
	m_ViewerCore->getUI()->showStatus( ss.str() );
}

void MainWindow::findGlobalMax()
{
	std::string attr;
	if ( m_RadiusSpin->text().toUInt() ) {
		attr = "local";
	} else {
		attr = "global";
	}
	m_ViewerCore->getUI()->showStatus( std::string("Searching for ") + attr + std::string("  maximum...") );
	const util::ivector4 maxVoxel = operation::NativeImageOps::getGlobalMax( m_ViewerCore->getCurrentImage(), 
																			 m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<util::ivector4>("voxelCoords"), 
																			 m_RadiusSpin->value() );
	m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( maxVoxel ) );
	std::stringstream ss;
	ss << "Found " << attr << " maximum at " << maxVoxel;
	m_ViewerCore->getUI()->showStatus( ss.str() );
}


}
}