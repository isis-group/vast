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
	preferencesDialog( new widget::PreferencesDialog( this, core ) ),
	loggingDialog( new widget::LoggingDialog( this, core ) ),
	fileDialog( new widget::FileDialog( this, core ) ),
	startWidget( new widget::StartWidget( this, core ) ),
	scalingWidget( new widget::ScalingWidget( this, core ) ),
	keyCommandsdialog( new widget::KeyCommandsDialog( this ) ),
	m_ViewerCore( core ),
	m_Toolbar( new QToolBar( this ) ),
	m_RadiusSpin( new QSpinBox( this ) ),
	m_LogButton( new QPushButton( this ) )
{
	m_Interface.setupUi( this );
	setWindowIcon( QIcon( ":/common/vast.jpg" ) );
	m_ActionReset_Scaling = new QAction( this );
	m_ActionAuto_Scaling = new QAction( this );
	m_ActionReset_Scaling->setShortcut( QKeySequence( tr( "R, S" ) ) );
	m_ActionAuto_Scaling->setShortcut( QKeySequence( tr( "A, S" ) ) );
	addAction( m_ActionReset_Scaling );
	addAction( m_ActionAuto_Scaling );

	m_Interface.actionInformation_Areas->setChecked( true );

	m_Interface.action_Save_Image->setShortcut( QKeySequence::Save );
	m_Interface.action_Save_Image->setIconVisibleInMenu( true );
	m_Interface.actionSave_Image->setIconVisibleInMenu( true );
	m_Interface.actionOpen_image->setShortcut( QKeySequence::Open );
	m_Interface.actionOpen_image->setIconVisibleInMenu( true );
	m_Interface.action_Exit->setIconVisibleInMenu( true );

#if QT_VERSION >= 0x040500
	m_Interface.actionSave_Image->setShortcut( QKeySequence::SaveAs );
	m_Interface.action_Exit->setShortcut( QKeySequence::Quit );
#endif

	m_Interface.actionSave_all_Images->setIconVisibleInMenu( true );
	m_Interface.actionIgnore_Orientation->setShortcut( QKeySequence( tr( "I, O" ) ) );
	m_Interface.action_Preferences->setShortcut( QKeySequence( tr( "S, P" ) ) );
	m_Interface.actionFind_Global_Max->setShortcut( QKeySequence( tr( "F, M, A" ) ) );
	m_Interface.actionFind_Global_Min->setShortcut( QKeySequence( tr( "F, M, I" ) ) );
	m_Interface.actionShow_scaling_option->setShortcut( QKeySequence( tr( "S, S" ) ) );
	m_Interface.actionPropagate_Zooming->setShortcut( QKeySequence( tr( "P, Z" ) ) );
	m_Interface.actionShow_Labels->setShortcut( QKeySequence( tr( "S, L" ) ) );
	m_Interface.actionShow_Crosshair->setShortcut( QKeySequence( tr( "S, C" ) ) );

	connect( m_Interface.action_Save_Image, SIGNAL( triggered() ), this, SLOT( saveImage() ) );
	connect( m_Interface.actionSave_Image, SIGNAL( triggered() ), this, SLOT( saveImageAs() ) );
	connect( m_Interface.actionOpen_image, SIGNAL( triggered() ), this, SLOT( openImage() ) );
	connect( m_Interface.action_Preferences, SIGNAL( triggered() ), preferencesDialog, SLOT( show() ) );
	connect( m_Interface.actionFind_Global_Min, SIGNAL( triggered() ), this, SLOT( findGlobalMin() ) );
	connect( m_Interface.actionFind_Global_Max, SIGNAL( triggered() ), this, SLOT( findGlobalMax() ) );
	connect( m_Interface.actionShow_Labels, SIGNAL( triggered( bool ) ), m_ViewerCore, SLOT( setShowLabels( bool ) ) );
	connect( m_RadiusSpin, SIGNAL( valueChanged( int ) ), this, SLOT( spinRadiusChanged( int ) ) );
	connect( m_Interface.actionShow_scaling_option, SIGNAL( triggered() ), this, SLOT( showScalingOption() ) );
	connect( m_Interface.actionIgnore_Orientation, SIGNAL( triggered( bool ) ), this, SLOT( ignoreOrientation( bool ) ) );
	connect( m_Interface.action_Exit, SIGNAL( triggered() ), this, SLOT( close() ) );
	connect( m_LogButton, SIGNAL( clicked() ), this, SLOT( showLoggingDialog() ) );
	connect( m_Interface.actionPropagate_Zooming, SIGNAL( triggered( bool ) ), this, SLOT( propagateZooming( bool ) ) );
	connect( m_ActionReset_Scaling, SIGNAL( triggered() ), this, SLOT( resetScaling() ) );
	connect( m_ActionAuto_Scaling, SIGNAL( triggered() ), this, SLOT( autoScaling() ) );
	connect( m_Interface.actionShow_Crosshair, SIGNAL( triggered( bool ) ), m_ViewerCore, SLOT( setShowCrosshair( bool ) ) );
	connect( m_Interface.actionSave_all_Images, SIGNAL( triggered() ), this, SLOT( saveAllImages() ) );
	connect( m_Interface.actionToggle_Zmap_Mode, SIGNAL( triggered( bool ) ), this, SLOT( toggleZMapMode( bool ) ) );
	connect( m_Interface.actionKey_Commands, SIGNAL( triggered() ), this, SLOT( showKeyCommandDialog() ) );
	connect( m_Interface.actionCreate_Screenshot, SIGNAL( triggered() ), this, SLOT( createScreenshot() ) );

	//toolbar stuff
	m_Toolbar->setOrientation( Qt::Horizontal );
	m_Toolbar->setMinimumHeight( 20 );
	m_Toolbar->setMaximumHeight( 30 );
	addToolBar( Qt::TopToolBarArea, m_Toolbar );
	m_Toolbar->addAction( m_Interface.actionOpen_image );
	m_Toolbar->addAction( m_Interface.action_Save_Image );
	m_Toolbar->addAction( m_Interface.actionSave_Image );
	m_Toolbar->addAction( m_Interface.actionSave_all_Images );
	m_Toolbar->addSeparator();
	m_Toolbar->addAction( m_Interface.actionShow_Labels );
	m_Toolbar->addAction( m_Interface.action_Preferences );
	m_Toolbar->addAction( m_Interface.actionShow_scaling_option );
	m_Toolbar->addSeparator();
	m_Toolbar->addAction( m_Interface.actionFind_Global_Min );
	m_Toolbar->addAction( m_Interface.actionFind_Global_Max );
	m_Toolbar->addWidget( m_RadiusSpin );
	m_Toolbar->addSeparator();
	m_RadiusSpin->setMinimum( 0 );
	m_RadiusSpin->setMaximum( 500 );
	m_RadiusSpin->setToolTip( "Search radius for finding local minimum/maximum. If radius is 0 it will search the entire image." );
	m_Interface.statusbar->addPermanentWidget( m_ViewerCore->getProgressFeedback()->getProgressBar() );

	m_LogButton->setText( "Show log" );
	m_Interface.statusbar->addPermanentWidget( m_LogButton );

	scalingWidget->setVisible( false );
	loadSettings();
}


void MainWindow::createScreenshot()
{
	if( m_ViewerCore->hasImage() ) {
		QString fileName = QFileDialog::getSaveFileName( this, tr( "Save Screenshot" ),
						   m_ViewerCore->getCurrentPath().c_str(),
						   tr( "Images (*.png *.xpm *.jpg)" ) );

		if( fileName.size() ) {
			m_ViewerCore->getUICore()->getScreenshot().save( fileName, 0, m_ViewerCore->getOptionMap()->getPropertyAs<uint8_t>( "screenshotQuality" ) );
		}
	}


}


void MainWindow::loadSettings()
{
	m_ViewerCore->getSettings()->beginGroup( "MainWindow" );
	move( m_ViewerCore->getSettings()->value( "pos", QPoint( 0, 0 ) ).toPoint() );

	if( m_ViewerCore->getSettings()->value( "maximized", false ).toBool() ) {
		showMaximized();
	}

	resize( m_ViewerCore->getSettings()->value( "size", QSize( 900, 900 ) ).toSize() );
	m_ViewerCore->getSettings()->endGroup();
}

void MainWindow::saveSettings()
{
	m_ViewerCore->getSettings()->beginGroup( "MainWindow" );
	m_ViewerCore->getSettings()->setValue( "size", size() );
	m_ViewerCore->getSettings()->setValue( "maximized", isMaximized() );
	m_ViewerCore->getSettings()->setValue( "pos", pos() );
	m_ViewerCore->getSettings()->endGroup();
	m_ViewerCore->getSettings()->sync();
}


void MainWindow::autoScaling()
{
	scalingWidget->autoScale();
}


void MainWindow::showKeyCommandDialog()
{
	keyCommandsdialog->show();
}

void MainWindow::toggleZMapMode( bool zmap )
{
	if( zmap ) {
		m_ViewerCore->setMode( ViewerCoreBase::zmap );
	} else {
		m_ViewerCore->setMode( ViewerCoreBase::standard );
	}

	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->updateScene();
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

	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->centerImages();

}


void MainWindow::showScalingOption()
{
	scalingWidget->move( QCursor::pos().x() + m_Toolbar->height() / 2, QCursor::pos().y() + m_Toolbar->height() / 2 );
	scalingWidget->showMe( m_Interface.actionShow_scaling_option->isChecked() );
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
		BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
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
			BOOST_FOREACH( util::slist::const_reference fileName, fileNames ) {
				text << fileName << std::endl;
			}
			msgBox.setText( text.str().c_str() );
			msgBox.setInformativeText( "Do you want to proceed?" );
			std::stringstream detailedText;
			detailedText << "Changed attributes: " << std::endl;
			BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
				if( image.second->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ).size() ) {
					detailedText << image.second->getFileNames().front() << " : " << std::endl;
					BOOST_FOREACH( util::slist::const_reference changedAttribute, image.second->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ) ) {
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
				BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
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
		getInterface().menu_Tools->addSeparator();
		getInterface().menu_Tools->addMenu( processMenu );

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
			processAction->setToolTip( QString( plugin->getTooltip().c_str() )  );
			signalMapper->setMapping( processAction, QString( plugin->getName().c_str() ) );
			tmpMenu->addAction( processAction );
			connect( processAction, SIGNAL( triggered() ), signalMapper, SLOT( map() ) );

		}
		connect( signalMapper, SIGNAL( mapped( QString ) ), m_ViewerCore, SLOT( callPlugin( QString ) ) );
	}
}

void MainWindow::refreshUI()
{
	m_Interface.actionShow_Labels->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showLabels" ) );
	m_Interface.actionShow_Crosshair->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showCrosshair" ) );
	m_ViewerCore->setShowLabels( m_Interface.actionShow_Labels->isChecked() );
	m_Interface.actionPropagate_Zooming->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "propagateZooming" ) );
	m_RadiusSpin->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "minMaxSearchRadius" ) );

	if( m_ViewerCore->getMode() == ViewerCoreBase::zmap ) {
		m_Interface.actionToggle_Zmap_Mode->setChecked( true );
		m_Interface.actionFind_Global_Max->setVisible(true);
		m_Interface.actionFind_Global_Min->setVisible(true);
		m_RadiusSpin->setVisible(true);
	} else {
		m_Interface.actionToggle_Zmap_Mode->setChecked( false );
		m_Interface.actionFind_Global_Max->setVisible(false);
		m_Interface.actionFind_Global_Min->setVisible(false);
		m_RadiusSpin->setVisible(false);
	}

	m_Interface.action_Save_Image->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionSave_all_Images->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionSave_Image->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionCreate_Screenshot->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionFind_Global_Max->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionFind_Global_Min->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionCenter_to_ca->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionToggle_Zmap_Mode->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionShow_Crosshair->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionShow_scaling_option->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionShow_Labels->setEnabled( m_ViewerCore->hasImage() );
}


void MainWindow::closeEvent( QCloseEvent * )
{
	saveSettings();
	m_ViewerCore->saveSettings();
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