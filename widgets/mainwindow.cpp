#include "mainwindow.hpp"
#include <iostream>
#include <QGridLayout>
#include "DataStorage/io_factory.hpp"
#include "uicore.hpp"
#include <qviewercore.hpp>


namespace isis
{
namespace viewer
{

MainWindow::MainWindow( QViewerCore *core ) :
	m_ViewerCore( core ),
	m_Toolbar( new QToolBar( this ) ),
	preferencesDialog( new widget::PreferencesDialog( this, core ) ),
	scalingWidget( new widget::ScalingWidget( this, core ) ),
	m_RadiusSpin( new QSpinBox( this ) ),
	m_LogButton( new QPushButton( this ) ),
	loggingDialog( new widget::LoggingDialog( this, core ) ),
	fileDialog( new widget::FileDialog( this, core ) ),
	startWidget( new widget::StartWidget( this, core ) )
{
	m_UI.setupUi( this );
	setWindowIcon( QIcon( ":/common/vast.jpg" ) );
	loadSettings();
	m_ActionReset_Scaling = new QAction( this );
	m_ActionReset_Scaling->setShortcut( QKeySequence( tr( "R, S" ) ) );
	addAction( m_ActionReset_Scaling );
	
	m_UI.action_Save_Image->setShortcut( QKeySequence::Save );
	m_UI.action_Save_Image->setIconVisibleInMenu(true);
	m_UI.actionSave_Image->setIconVisibleInMenu(true);
	m_UI.actionOpen_image->setShortcut( QKeySequence::Open );
	m_UI.actionOpen_image->setIconVisibleInMenu( true );
	m_UI.action_Exit->setIconVisibleInMenu( true );

#if QT_VERSION >= 0x040500 
	m_UI.actionSave_Image->setShortcut( QKeySequence::SaveAs );
	m_UI.action_Exit->setShortcut( QKeySequence::Quit );	
#endif

	m_UI.actionSave_all_Images->setIconVisibleInMenu( true );
	m_UI.actionIgnore_Orientation->setShortcut( QKeySequence( tr( "I, O" ) ) );
	m_UI.action_Preferences->setShortcut( QKeySequence( tr( "S, P" ) ) );
	m_UI.actionFind_Global_Max->setShortcut( QKeySequence( tr( "F, M, A" ) ) );
	m_UI.actionFind_Global_Min->setShortcut( QKeySequence( tr( "F, M, I" ) ) );
	m_UI.actionShow_scaling_option->setShortcut( QKeySequence( tr("S, S" ) ) );
	m_UI.actionPropagate_Zooming->setShortcut( QKeySequence( tr("P, Z" ) ) );
	m_UI.actionShow_Labels->setShortcut( QKeySequence( tr("S, L" ) ) );
	m_UI.actionShow_Crosshair->setShortcut( QKeySequence( tr("S, C" ) ) );

	connect( m_UI.action_Save_Image, SIGNAL( triggered() ), this, SLOT( saveImage() ) );
	connect( m_UI.actionSave_Image, SIGNAL( triggered() ), this, SLOT( saveImageAs() ) );
	connect( m_UI.actionOpen_image, SIGNAL( triggered() ), this, SLOT( openImage() ) );
	connect( m_UI.action_Preferences, SIGNAL( triggered() ), preferencesDialog, SLOT( show() ) );
	connect( m_UI.actionFind_Global_Min, SIGNAL( triggered() ), this, SLOT( findGlobalMin() ) );
	connect( m_UI.actionFind_Global_Max, SIGNAL( triggered() ), this, SLOT( findGlobalMax() ) );
	connect( m_UI.actionShow_Labels, SIGNAL( triggered( bool ) ), m_ViewerCore, SLOT( setShowLabels( bool ) ) );
	connect( m_RadiusSpin, SIGNAL( valueChanged( int ) ), this, SLOT( spinRadiusChanged( int ) ) );
	connect( m_UI.actionShow_scaling_option, SIGNAL( triggered() ), this, SLOT( showScalingOption() ) );
	connect( m_UI.actionIgnore_Orientation, SIGNAL( triggered( bool ) ), this, SLOT( ignoreOrientation( bool ) ) );
	connect( m_UI.action_Exit, SIGNAL( triggered() ), this, SLOT( close() ) );
	connect( m_LogButton, SIGNAL( clicked() ), this, SLOT( showLoggingDialog() ) );
	connect( m_UI.actionPropagate_Zooming, SIGNAL( triggered( bool ) ), this, SLOT( propagateZooming( bool ) ) );
	connect( m_ActionReset_Scaling, SIGNAL( triggered() ), this, SLOT( resetScaling() ) );
	connect( m_UI.actionShow_Crosshair, SIGNAL( triggered(bool)), m_ViewerCore, SLOT( setShowCrosshair(bool)));
	connect( m_UI.actionSave_all_Images, SIGNAL( triggered()), this, SLOT( saveAllImages()));
	connect( m_UI.actionToggle_Zmap_Mode, SIGNAL( triggered(bool)), this, SLOT( toggleZMapMode(bool)));

	//toolbar stuff
	m_Toolbar->setOrientation( Qt::Horizontal );
	m_Toolbar->setMinimumHeight( 20 );
	m_Toolbar->setMaximumHeight( 30 );
	addToolBar( Qt::TopToolBarArea, m_Toolbar );
	m_Toolbar->addAction( m_UI.actionOpen_image );
	m_Toolbar->addAction( m_UI.action_Save_Image );
	m_Toolbar->addAction( m_UI.actionSave_Image );
	m_Toolbar->addAction( m_UI.actionSave_all_Images );
	m_Toolbar->addSeparator();
	m_Toolbar->addAction( m_UI.actionShow_Labels );
	m_Toolbar->addAction( m_UI.action_Preferences );
	m_Toolbar->addAction( m_UI.actionShow_scaling_option );
	m_Toolbar->addSeparator();
	m_Toolbar->addAction( m_UI.actionFind_Global_Min );
	m_Toolbar->addAction( m_UI.actionFind_Global_Max );
	m_Toolbar->addWidget( m_RadiusSpin );
	m_Toolbar->addSeparator();
	m_RadiusSpin->setMinimum( 0 );
	m_RadiusSpin->setMaximum( 500 );
	m_RadiusSpin->setToolTip( "Search radius for finding local minimum/maximum. If radius is 0 it will search the entire image." );
	m_UI.statusbar->addPermanentWidget( m_ViewerCore->getProgressFeedback()->getProgressBar() );
	
	m_LogButton->setText( "Show log" );
	m_UI.statusbar->addPermanentWidget( m_LogButton );

	scalingWidget->setVisible( false );
}

void MainWindow::toggleZMapMode(bool zmap)
{
	if( zmap ) {
		m_ViewerCore->setMode( ViewerCoreBase::zmap );
	} else {
		m_ViewerCore->setMode( ViewerCoreBase::standard );
	}
	m_ViewerCore->getUI()->refreshUI();
	m_ViewerCore->updateScene();
}



void MainWindow::keyPressEvent( QKeyEvent *e )
{
	if( e->key() == Qt::Key_Space ) {
		m_ViewerCore->centerImages();
	}
}


void MainWindow::resetScaling()
{
	BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) {
		image.second->scaling = 1.0;
		image.second->offset = 0.0;
	}
	m_ViewerCore->updateScene();
	scalingWidget->synchronize();
}


void MainWindow::propagateZooming( bool propagate )
{
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "propagateZooming", propagate );
	m_ViewerCore->updateScene();
}


void MainWindow::showLoggingDialog()
{
	loggingDialog->synchronize();
	loggingDialog->setVisible( true );
}


void MainWindow::ignoreOrientation( bool ignore )
{
	BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) {
		if( ignore ) {
			setOrientationToIdentity( *image.second->getISISImage() );
			checkForCaCp( image.second );
			image.second->updateOrientation();
		} else {
			image.second->getISISImage()->setPropertyAs<util::fvector4>( "rowVec", image.second->getPropMap().getPropertyAs<util::fvector4>( "originalRowVec" ) );
			image.second->getISISImage()->setPropertyAs<util::fvector4>( "columnVec", image.second->getPropMap().getPropertyAs<util::fvector4>( "originalColumnVec" ) );
			image.second->getISISImage()->setPropertyAs<util::fvector4>( "sliceVec", image.second->getPropMap().getPropertyAs<util::fvector4>( "originalSliceVec" ) );
			image.second->getISISImage()->setPropertyAs<util::fvector4>( "indexOrigin", image.second->getPropMap().getPropertyAs<util::fvector4>( "originalIndexOrigin" ) );
			image.second->updateOrientation();
			checkForCaCp( image.second );
		}
		image.second->physicalCoords = image.second->getISISImage()->getPhysicalCoordsFromIndex( image.second->voxelCoords );
	}
	
	m_ViewerCore->getUI()->refreshUI();
	m_ViewerCore->centerImages();
	
}


void MainWindow::showScalingOption()
{
	scalingWidget->move( QCursor::pos().x() + m_Toolbar->height() / 2, QCursor::pos().y() + m_Toolbar->height() / 2 );
	scalingWidget->showMe( m_UI.actionShow_scaling_option->isChecked() );
}


void MainWindow::spinRadiusChanged( int radius )
{
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>( "minMaxSearchRadius", radius );
}

void MainWindow::openImage()
{
	fileDialog->setMode( isis::viewer::widget::FileDialog::OPEN_FILE );
	fileDialog->show();
}

void MainWindow::saveImage()
{
	if( m_ViewerCore->hasImage() ) {
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
				isis::data::IOFactory::write( *m_ViewerCore->getCurrentImage()->getISISImage(), m_ViewerCore->getCurrentImage()->getFileNames().front(), "", "" );
				break;
			}
		}
	}
}

void MainWindow::saveAllImages()
{
	if( m_ViewerCore->hasImage() ) {
		std::list<util::slist> changedAttributesList;
		util::slist fileNames;
		BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) 
		{
			fileNames.push_back( image.second->getFileNames().front() );
			if( image.second->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ).size() ) {
				changedAttributesList.push_back( image.second->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ) );
			}
		}
		if( changedAttributesList.size() ) {
			QMessageBox msgBox;
			msgBox.setIcon( QMessageBox::Information );
			std::stringstream text;
			text << "This will overwrite:" << std::endl;;
			BOOST_FOREACH( util::slist::const_reference fileName, fileNames ) 
			{
				text << fileName << std::endl;
			}
			msgBox.setText( text.str().c_str() );
			msgBox.setInformativeText( "Do you want to proceed?" );
			std::stringstream detailedText;
			detailedText << "Changed attributes: " << std::endl;
			BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) 
			{
				if( image.second->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ).size() ) {
					detailedText << image.second->getFileNames().front() << " : " << std::endl;
					BOOST_FOREACH( util::slist::const_reference changedAttribute, image.second->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ) ) 
					{
						detailedText << changedAttribute << std::endl;
					}
				}
			}
			msgBox.setDetailedText( detailedText.str().c_str() );
			msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
			msgBox.setDefaultButton( QMessageBox::No );

			switch ( msgBox.exec() ) {
			case QMessageBox::No:
				return;
				break;
			case QMessageBox::Yes:
				BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) 
				{
					isis::data::IOFactory::write( *image.second->getISISImage(), image.second->getFileNames().front(), "", "" );
				}
				
				break;
			}
			
		} else {
			QMessageBox msgBox;
			msgBox.setText( "No image has changed attributes! Won´t save anything." );
			msgBox.exec();
		}
		
	}
}


void MainWindow::saveImageAs()
{
	if( m_ViewerCore->hasImage() ) {
		std::stringstream fileFormats;
		fileFormats << "Image files (" << getFileFormatsAsString( isis::image_io::FileFormat::write_only, std::string( "*." ) ) << ")";
		QString filename = QFileDialog::getSaveFileName( this,
						tr( "Save Image As..." ),
						m_ViewerCore->getCurrentPath().c_str(),
						tr( fileFormats.str().c_str() ) );

		if( filename.size() ) {
			isis::data::IOFactory::write( *m_ViewerCore->getCurrentImage()->getISISImage(), filename.toStdString(), "", "" );
		}
	}

}


void MainWindow::reloadPluginsToGUI()
{
	//adding all processes to the process (plugin) menu and connect the action to the respective call functions
	QMenu *processMenu = new QMenu( QString( "Plugins" ) );

	if( m_ViewerCore->getPlugins().size() ) {
		getUI().menu_Tools->addSeparator();
		getUI().menu_Tools->addMenu( processMenu );

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
			processAction->setShortcut( plugin->getShortcut() );
			processAction->setToolTip(QString( plugin->getTooltip().c_str() )  );
			signalMapper->setMapping( processAction, QString( plugin->getName().c_str() ) );
			tmpMenu->addAction( processAction );
			connect( processAction, SIGNAL( triggered() ), signalMapper, SLOT( map() ) );

		}
		connect( signalMapper, SIGNAL( mapped( QString ) ), m_ViewerCore, SLOT( callPlugin( QString ) ) );
	}
}

void MainWindow::loadSettings()
{
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	resize( m_ViewerCore->getSettings()->value( "size", QSize( 900, 900 ) ).toSize() );
	move( m_ViewerCore->getSettings()->value( "pos", QPoint( 0, 0 ) ).toPoint() );

	if( m_ViewerCore->getSettings()->value( "maximized", false ).toBool() ) {
		showMaximized();
	}

	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "propagateZooming", m_ViewerCore->getSettings()->value( "propagateZooming", false ).toBool() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "showLabels", m_ViewerCore->getSettings()->value( "showLabels", false ).toBool() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "showCrosshair", m_ViewerCore->getSettings()->value( "showCrosshair", true ).toBool() );
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>( "minMaxSearchRadius",
			m_ViewerCore->getSettings()->value( "minMaxSearchRadius", m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "minMaxSearchRadius" ) ).toInt() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "showAdvancedFileDialogOptions", m_ViewerCore->getSettings()->value( "showAdvancedFileDialogOptions", false ).toBool() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "showFavoriteFileList", m_ViewerCore->getSettings()->value( "showFavoriteFileList", false).toBool() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "showStartWidget", m_ViewerCore->getSettings()->value("showStartWidget", true).toBool() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "showLoadingWidget", m_ViewerCore->getSettings()->value("showLoadingWidget", true).toBool() );
	m_ViewerCore->getSettings()->endGroup();
}

void MainWindow::refreshUI()
{
	m_UI.actionShow_Labels->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showLabels" ) );
	m_UI.actionShow_Crosshair->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showCrosshair" ) );
	m_ViewerCore->setShowLabels( m_UI.actionShow_Labels->isChecked() );
	m_UI.actionPropagate_Zooming->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "propagateZooming" ) );
	m_RadiusSpin->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "minMaxSearchRadius" ) );
	if( m_ViewerCore->getMode() == ViewerCoreBase::zmap ) {
		m_UI.actionToggle_Zmap_Mode->setChecked( true );
	} else {
		m_UI.actionToggle_Zmap_Mode->setChecked( false );
	}
}


void MainWindow::closeEvent( QCloseEvent * )
{
	saveSettings();
}

void MainWindow::saveSettings()
{
	//saving the preferences to the profile file
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	m_ViewerCore->getSettings()->setValue( "size", size() );
	m_ViewerCore->getSettings()->setValue( "maximized", isMaximized() );
	m_ViewerCore->getSettings()->setValue( "pos", pos() );
	m_ViewerCore->getSettings()->setValue( "propagateZooming", m_UI.actionPropagate_Zooming->isChecked() );
	m_ViewerCore->getSettings()->setValue( "minMaxSearchRadius", m_RadiusSpin->value() );
	m_ViewerCore->getSettings()->setValue( "showLabels", m_UI.actionShow_Labels->isChecked() );
	m_ViewerCore->getSettings()->setValue( "showCrosshair", m_UI.actionShow_Crosshair->isChecked() );
	m_ViewerCore->getSettings()->setValue( "showAdvancedFileDialogOptions", m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showAdvancedFileDialogOptions" ) );
	m_ViewerCore->getSettings()->setValue( "showFavoriteFileList", m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showFavoriteFileList" ) );
	m_ViewerCore->getSettings()->setValue( "showStartWidget", m_ViewerCore->getOptionMap()->getPropertyAs<bool>("showStartWidget") );
	m_ViewerCore->getSettings()->setValue( "showLoadingWidget", m_ViewerCore->getOptionMap()->getPropertyAs<bool>("showLoadingWidget") );
	m_ViewerCore->getSettings()->endGroup();
	m_ViewerCore->getSettings()->sync();
}

void MainWindow::findGlobalMin()
{
	if( m_ViewerCore->hasImage() ) {
		const util::ivector4 minVoxel = operation::NativeImageOps::getGlobalMin( m_ViewerCore->getCurrentImage(),
										m_ViewerCore->getCurrentImage()->voxelCoords,
										m_RadiusSpin->value() );
		m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( minVoxel ) );
	}
}

void MainWindow::findGlobalMax()
{
	if( m_ViewerCore->hasImage() ) {
		const util::ivector4 maxVoxel = operation::NativeImageOps::getGlobalMax( m_ViewerCore->getCurrentImage(),
										m_ViewerCore->getCurrentImage()->voxelCoords,
										m_RadiusSpin->value() );
		m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( maxVoxel ) );
	}
}


}
}