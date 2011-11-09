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
	m_RadiusSpin(new QSpinBox(this)),
	m_LogButton( new QPushButton( this ) ),
	m_LoggingDialog( new widget::LoggingDialog( this, core ) ),
	m_FileDialog( new widget::FileDialog( this, core ) )
{
	m_UI.setupUi( this );
	loadSettings();
	connect( m_UI.action_Save_Image, SIGNAL( triggered() ), this, SLOT( saveImage() ) );
	connect( m_UI.actionSave_Image, SIGNAL( triggered()), this, SLOT( saveImageAs() ) );
	connect( m_UI.actionOpen_image, SIGNAL( triggered()), this, SLOT( openImage()));
	connect( m_UI.action_Preferences, SIGNAL( triggered() ), this, SLOT( showPreferences() ) );
	connect( m_UI.actionFind_Global_Min, SIGNAL( triggered()), this, SLOT( findGlobalMin()));
	connect( m_UI.actionFind_Global_Max, SIGNAL( triggered()), this, SLOT( findGlobalMax()));
	connect( m_UI.actionShow_Labels, SIGNAL( triggered(bool)), m_ViewerCore, SLOT( setShowLabels(bool)));
	connect( m_RadiusSpin, SIGNAL(valueChanged(int)), this, SLOT( spinRadiusChanged(int)));
	connect( m_UI.actionShow_scaling_option, SIGNAL( triggered()), this, SLOT( showScalingOption()));
	connect( m_UI.actionIgnore_Orientation, SIGNAL( triggered(bool)), this, SLOT( ignoreOrientation(bool)));
	connect( m_UI.action_Exit, SIGNAL( triggered()), this, SLOT( close()));
	connect( m_LogButton, SIGNAL( clicked()), this, SLOT( showLoggingDialog()) );
	connect( m_UI.actionPropagate_Zooming, SIGNAL(triggered(bool)), this, SLOT(propagateZooming(bool)));

	//toolbar stuff
	m_Toolbar->setOrientation( Qt::Horizontal );
	m_Toolbar->setMinimumHeight( 20 );
	m_Toolbar->setMaximumHeight( 30 );
	addToolBar( Qt::TopToolBarArea, m_Toolbar );
	m_Toolbar->addAction( m_UI.actionOpen_image );
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
	
	m_LogButton->setText( "Show log" );
	m_UI.statusbar->addPermanentWidget( m_LogButton );
	
	m_ScalingWidget->setVisible(false);
	m_WorkingInformationLabel = new QLabel( this );
	m_WorkingInformationLabel->setFrameShape(QFrame::Box);
	m_WorkingInformationLabel->setAlignment(Qt::AlignCenter);
	m_WorkingInformationLabel->setFont( QFont("Times", 15 ) );
	m_WorkingInformationLabel->setVisible( false );

}

void MainWindow::propagateZooming(bool propagate)
{
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>("propagateZooming", propagate);
	m_ViewerCore->updateScene();
}


void MainWindow::showLoggingDialog()
{
	m_LoggingDialog->synchronize();
	m_LoggingDialog->setVisible( true );
}


void MainWindow::ignoreOrientation(bool ignore)
{
	BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) {
		if( ignore ) {
			setOrientationToIdentity( *image.second->getISISImage() );
		} else {
			image.second->getISISImage()->setPropertyAs<util::fvector4>("rowVec", image.second->getPropMap().getPropertyAs<util::fvector4>("originalRowVec"));
			image.second->getISISImage()->setPropertyAs<util::fvector4>("columnVec", image.second->getPropMap().getPropertyAs<util::fvector4>("originalColumnVec"));
			image.second->getISISImage()->setPropertyAs<util::fvector4>("sliceVec", image.second->getPropMap().getPropertyAs<util::fvector4>("originalSliceVec"));
			image.second->getISISImage()->setPropertyAs<util::fvector4>("indexOrigin", image.second->getPropMap().getPropertyAs<util::fvector4>("originalIndexOrigin"));
		}
	}
	m_ViewerCore->updateScene();
}


void MainWindow::showScalingOption()
{
	m_ScalingWidget->move( QCursor::pos().x() + m_Toolbar->height() / 2, QCursor::pos().y() + m_Toolbar->height() / 2 );
	m_ScalingWidget->showMe( m_UI.actionShow_scaling_option->isChecked() );
}


void MainWindow::spinRadiusChanged(int radius)
{
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>("minMaxSearchRadius", radius);
}

void MainWindow::openImage()
{
	
	m_FileDialog->setMode( isis::viewer::widget::FileDialog::OPEN_FILE );
	m_FileDialog->show();
	

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
			isis::data::IOFactory::write( *m_ViewerCore->getCurrentImage()->getISISImage(), m_ViewerCore->getCurrentImage()->getFileNames().front(), "", "" );
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
	
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>("propagateZooming", m_ViewerCore->getSettings()->value( "propagateZooming", false ).toBool() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>("showLabels", m_ViewerCore->getSettings()->value("showLabels", false).toBool() );
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>("minMaxSearchRadius", 
		m_ViewerCore->getSettings()->value("minMaxSearchRadius", m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("minMaxSearchRadius") ).toInt() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>("showAdvancedFileDialogOptions", m_ViewerCore->getSettings()->value("showAdvancedFileDialogOptions", false).toBool() );
	m_ViewerCore->getSettings()->endGroup();
}

void MainWindow::refreshUI()
{
	m_UI.actionShow_Labels->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>("showLabels") );
	m_ViewerCore->setShowLabels(m_UI.actionShow_Labels->isChecked());
	m_UI.actionPropagate_Zooming->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>("propagateZooming"));
	m_RadiusSpin->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("minMaxSearchRadius"));
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
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	m_ViewerCore->getSettings()->setValue( "propagateZooming", m_UI.actionPropagate_Zooming->isChecked() );
	m_ViewerCore->getSettings()->setValue( "minMaxSearchRadius", m_RadiusSpin->value() );
	m_ViewerCore->getSettings()->setValue( "showLabels", m_UI.actionShow_Labels->isChecked() );
	m_ViewerCore->getSettings()->setValue( "showAdvancedFileDialogOptions", m_ViewerCore->getOptionMap()->getPropertyAs<bool>("showAdvancedFileDialogOptions") );
	m_ViewerCore->getSettings()->endGroup();
	m_ViewerCore->getSettings()->sync();
}

void MainWindow::showPreferences()
{
	m_PreferencesDialog->show();
}

void MainWindow::findGlobalMin()
{
	const util::ivector4 minVoxel = operation::NativeImageOps::getGlobalMin( m_ViewerCore->getCurrentImage(), 
																			 m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<util::ivector4>("voxelCoords"), 
																			 m_RadiusSpin->value());
	m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( minVoxel ) );

}

void MainWindow::findGlobalMax()
{
	const util::ivector4 maxVoxel = operation::NativeImageOps::getGlobalMax( m_ViewerCore->getCurrentImage(), 
																			 m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<util::ivector4>("voxelCoords"), 
																			 m_RadiusSpin->value() );
	m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( maxVoxel ) );
}


}
}