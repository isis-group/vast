/****************************************************************
 *
 * <Copyright information>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Author: Erik Tuerke, tuerke@cbs.mpg.de
 *
 * mainwindow.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "mainwindow.hpp"
#include <iostream>
#include <QGridLayout>
#include <DataStorage/io_factory.hpp>
#include "../viewer/uicore.hpp"
#include "../viewer/qviewercore.hpp"
#include "../viewer/fileinformation.hpp"
#include "../widgets/scalingWidget.hpp"


namespace isis
{
namespace viewer
{

MainWindow::MainWindow( QViewerCore *core ) :
	preferencesDialog( new ui::PreferencesDialog( this, core ) ),
	loggingDialog( new ui::LoggingDialog( this, core ) ),
	fileDialog( new ui::FileDialog( this, core ) ),
	startWidget( new ui::StartWidget( this, core ) ),
	scalingWidget( new ui::ScalingWidget( this, core ) ),
	keyCommandsdialog( new ui::KeyCommandsDialog( this ) ),
#ifdef HAVE_WEBKIT
	helpDialog( new ui::HelpDialog( this ) ),
#endif //HAVE_WEBKIT
	aboutDialog( new ui::AboutDialog( this, core ) ),
	m_ViewerCore( core ),
	m_Toolbar( new QToolBar( this ) ),
	m_RadiusSpin( new QSpinBox( this ) ),
	m_StatusTextLabel( new QLabel( this ) ),
	m_StatusMovieLabel( new QLabel( this ) ),
	m_StatusMovie( new QMovie( this ) )
{
	m_Interface.setupUi( this );
	setWindowIcon( QIcon( m_ViewerCore->getSettings()->getPropertyAs<std::string>( "vastSymbol" ).c_str() ) );
	m_ActionReset_Scaling = new QAction( this );
	m_ActionAuto_Scaling = new QAction( this );
	m_ActionReset_Scaling->setShortcut( QKeySequence( tr( "R, S" ) ) );
	m_ActionAuto_Scaling->setShortcut( QKeySequence( tr( "A, S" ) ) );
	addAction( m_ActionReset_Scaling );
	addAction( m_ActionAuto_Scaling );

	m_Interface.actionInformation_Areas->setChecked( true );
	m_Interface.actionAxial_View->setChecked( true );
	m_Interface.actionSagittal_View->setChecked( true );
	m_Interface.actionCoronal_View->setChecked( true );



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
	m_Interface.actionGeometrical_View->setShortcut( QKeySequence( tr( "G, V" ) ) );

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
	connect( m_Interface.actionPropagate_Zooming, SIGNAL( triggered( bool ) ), this, SLOT( propagateZooming( bool ) ) );
	connect( m_ActionReset_Scaling, SIGNAL( triggered() ), this, SLOT( resetScaling() ) );
	connect( m_Interface.actionShow_Crosshair, SIGNAL( triggered( bool ) ), m_ViewerCore, SLOT( setShowCrosshair( bool ) ) );
	connect( m_Interface.actionSave_all_Images, SIGNAL( triggered() ), this, SLOT( saveAllImages() ) );
	connect( m_Interface.actionKey_Commands, SIGNAL( triggered() ), this, SLOT( showKeyCommandDialog() ) );
	connect( m_Interface.actionCreate_Screenshot, SIGNAL( triggered() ), this, SLOT( createScreenshot() ) );
#ifdef HAVE_WEBKIT
	connect( m_Interface.actionHelp, SIGNAL( triggered() ), helpDialog, SLOT( show() ) );
#endif //HAVE_WEBKIT
	connect( m_Interface.actionAbout_Dialog, SIGNAL( triggered() ), aboutDialog, SLOT( show() ) );
	connect( m_Interface.actionLogging, SIGNAL( triggered() ), this, SLOT( showLoggingDialog() ) );
	connect( m_Interface.actionAxial_View, SIGNAL( triggered( bool ) ), this, SLOT( toggleAxialView( bool ) ) );
	connect( m_Interface.actionSagittal_View, SIGNAL( triggered( bool ) ), this, SLOT( toggleSagittalView( bool ) ) );
	connect( m_Interface.actionCoronal_View, SIGNAL( triggered( bool ) ), this, SLOT( toggleCoronalView( bool ) ) );
	connect( m_Interface.actionGeometrical_View, SIGNAL( toggled( bool ) ), this, SLOT( toggleGeometrical( bool ) ) );


	//toolbar stuff
	m_Toolbar->setOrientation( Qt::Horizontal );
	m_Toolbar->setMinimumHeight( 20 );
	m_Toolbar->setMaximumHeight( 40 );
	m_Toolbar->adjustSize();
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
	m_RadiusSpinAction = m_Toolbar->addWidget( m_RadiusSpin );
	m_Toolbar->addSeparator();
	m_RadiusSpin->setMinimum( 0 );
	m_RadiusSpin->setMaximum( 500 );
	m_RadiusSpin->setToolTip( "Search radius for finding local minimum/maximum. If radius is 0 it will search the entire image." );

	m_Interface.statusbar->addPermanentWidget( m_ViewerCore->getProgressFeedback()->getProgressBar() );
	m_Interface.statusbar->addPermanentWidget( m_StatusTextLabel );
	m_Interface.statusbar->addPermanentWidget( m_StatusMovieLabel );
	m_StatusMovie->setFileName( ":/common/loading.gif" );
	m_StatusMovie->setScaledSize( QSize( m_Interface.statusbar->height(), m_Interface.statusbar->height() ) );
	m_StatusMovieLabel->setMovie( m_StatusMovie );
	m_StatusMovieLabel->setVisible( false );
	m_Interface.statusbar->setVisible( false );

	scalingWidget->setVisible( false );
	loadSettings();
	m_Interface.actionOpen_recent->setMenu( new QMenu() );
}


void MainWindow::toggleGeometrical ( bool geometrical )
{
	std::vector<ImageHolder::Vector> images;
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "showImagesGeometricalView", geometrical );
	std::string widgetIdentifier;

	if( geometrical ) {
		widgetIdentifier = m_ViewerCore->getSettings()->getPropertyAs<std::string>( "widgetGeometrical" );
	} else {
		widgetIdentifier = m_ViewerCore->getSettings()->getPropertyAs<std::string>( "widgetLatched" );
	}

	WidgetEnsemble::Vector eVec = m_ViewerCore->getUICore()->getEnsembleList();
	BOOST_FOREACH( WidgetEnsemble::Vector::const_reference ensemble,  eVec ) {
		images.push_back( m_ViewerCore->getUICore()->closeWidgetEnsemble( ensemble ) );
	}
	BOOST_FOREACH( std::vector<ImageHolder::Vector>::const_reference imVec, images ) {
		m_ViewerCore->getUICore()->createViewWidgetEnsemble( widgetIdentifier, imVec, true );
	}
	m_ViewerCore->getUICore()->refreshUI();
}


void MainWindow::createScreenshot()
{
	if( m_ViewerCore->hasImage() ) {

		QString fileName = QFileDialog::getSaveFileName( this, tr( "Save Screenshot" ),
						   m_ViewerCore->getCurrentPath().c_str(),
						   tr( "Images (*.png *.xpm *.jpg)" ) );

		if( fileName.size() ) {
			m_ViewerCore->getUICore()->toggleLoadingIcon( true, QString( "Creating and saving screenshot to " ) + fileName );
			m_ViewerCore->getUICore()->getScreenshot().save( fileName, 0, m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotQuality" ) );
			m_ViewerCore->setCurrentPath( fileName.toStdString() );
		}

		m_ViewerCore->getUICore()->toggleLoadingIcon( false );
	}


}

void MainWindow::toggleAxialView( bool visible )
{
	m_ViewerCore->getUICore()->setViewPlaneOrientation( axial, visible );
}

void MainWindow::toggleCoronalView( bool visible )
{
	m_ViewerCore->getUICore()->setViewPlaneOrientation( coronal, visible );
}

void MainWindow::toggleSagittalView( bool visible )
{
	m_ViewerCore->getUICore()->setViewPlaneOrientation( sagittal, visible );
}



void MainWindow::loadSettings()
{
	m_ViewerCore->getSettings()->getQSettings()->beginGroup( "MainWindow" );
	move( m_ViewerCore->getSettings()->getQSettings()->value( "pos", QPoint( 0, 0 ) ).toPoint() );

	if( m_ViewerCore->getSettings()->getQSettings()->value( "maximized", false ).toBool() ) {
		showMaximized();
	}

	resize( m_ViewerCore->getSettings()->getQSettings()->value( "size", QSize( 900, 900 ) ).toSize() );
	m_ViewerCore->getSettings()->getQSettings()->endGroup();
}

void MainWindow::saveSettings()
{
	m_ViewerCore->getSettings()->getQSettings()->beginGroup( "MainWindow" );
	m_ViewerCore->getSettings()->getQSettings()->setValue( "size", size() );
	m_ViewerCore->getSettings()->getQSettings()->setValue( "maximized", isMaximized() );
	m_ViewerCore->getSettings()->getQSettings()->setValue( "pos", pos() );
	m_ViewerCore->getSettings()->getQSettings()->endGroup();
	m_ViewerCore->getSettings()->getQSettings()->sync();
}


void MainWindow::showKeyCommandDialog()
{
	keyCommandsdialog->show();
}


void MainWindow::resetScaling()
{
	BOOST_FOREACH( ImageHolder::Vector::const_reference image, m_ViewerCore->getImageVector() ) {
		image->getImageProperties().scaling = 1.0;
		image->getImageProperties().offset = 0.0;
		image->getImageProperties().scalingMinMax = operation::NativeImageOps::getMinMaxFromScalingOffset( std::make_pair<double, double>( 1.0, 0.0 ), image );
		image->updateColorMap();
	}
	m_ViewerCore->updateScene();
	m_ViewerCore->getUICore()->refreshUI();
}


void MainWindow::propagateZooming( bool propagate )
{
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "propagateZooming", propagate );
	m_ViewerCore->updateScene();
}


void MainWindow::showLoggingDialog()
{
	loggingDialog->synchronize();
	loggingDialog->setVisible( true );
}


void MainWindow::ignoreOrientation( bool ignore )
{
	BOOST_FOREACH( ImageHolder::Vector::const_reference image, m_ViewerCore->getImageVector() ) {
		if( ignore ) {
			setOrientationToIdentity( *image->getISISImage() );
			image->updateOrientation();
		} else {
			image->getISISImage()->setPropertyAs<util::fvector3>( "rowVec", image->getPropMap().getPropertyAs<util::fvector3>( "originalRowVec" ) );
			image->getISISImage()->setPropertyAs<util::fvector3>( "columnVec", image->getPropMap().getPropertyAs<util::fvector3>( "originalColumnVec" ) );
			image->getISISImage()->setPropertyAs<util::fvector3>( "sliceVec", image->getPropMap().getPropertyAs<util::fvector3>( "originalSliceVec" ) );
			image->getISISImage()->setPropertyAs<util::fvector3>( "indexOrigin", image->getPropMap().getPropertyAs<util::fvector3>( "originalIndexOrigin" ) );
			image->updateOrientation();

			if( m_ViewerCore->getSettings()->getPropertyAs<bool>( "checkCACP" ) ) {
				checkForCaCp( image );
			}
		}

		image->getImageProperties().physicalCoords = image->getISISImage()->getPhysicalCoordsFromIndex( image->getImageProperties().voxelCoords );
	}
	m_ViewerCore->getUICore()->getMainWindow()->getInterface().actionIgnore_Orientation->setChecked(ignore);
	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->centerImages();
	m_ViewerCore->updateScene();

}


void MainWindow::showScalingOption()
{
	scalingWidget->move( QCursor::pos().x() + m_Toolbar->height() / 2, QCursor::pos().y() + m_Toolbar->height() / 2 );
	scalingWidget->synchronize();
	scalingWidget->show();
}


void MainWindow::spinRadiusChanged( int radius )
{
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "minMaxSearchRadius", radius );
}

void MainWindow::openImage()
{
	fileDialog->setMode( isis::viewer::ui::FileDialog::OPEN_FILE );
	fileDialog->show();
}

void MainWindow::saveImage()
{
	if( m_ViewerCore->hasImage() ) {
		QMessageBox msgBox;
		msgBox.setIcon( QMessageBox::Information );
		std::stringstream text;
		text << "This will overwrite " << m_ViewerCore->getCurrentImage()->getImageProperties().filePath << " !";
		msgBox.setText( text.str().c_str() );
		const util::slist changedAttributes = m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<util::slist>( "changedAttributes" );

		if( changedAttributes.size() ) {
			std::stringstream detailedText;
			detailedText << "Changed attributes: " << std::endl;
			BOOST_FOREACH( util::slist::const_reference attrChanged, m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ) ) {
				detailedText << " >> " << attrChanged << std::endl;
			}
			msgBox.setDetailedText( detailedText.str().c_str() );
		}

		msgBox.setInformativeText( "Do you want to proceed?" );
		msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
		msgBox.setDefaultButton( QMessageBox::No );

		switch ( msgBox.exec() ) {
		case QMessageBox::No:
			return;
			break;
		case QMessageBox::Yes:
			m_ViewerCore->getUICore()->toggleLoadingIcon( true, QString( "Saving image to " ) + m_ViewerCore->getCurrentImage()->getImageProperties().fileName.c_str() );
			isis::data::IOFactory::write( *m_ViewerCore->getCurrentImage()->getISISImage(), m_ViewerCore->getCurrentImage()->getImageProperties().filePath, "", "" );
			m_ViewerCore->getUICore()->toggleLoadingIcon( false );
			break;
		}
	}
}

void MainWindow::saveAllImages()
{
	if( m_ViewerCore->hasImage() ) {
		std::list<util::slist> changedAttributesList;
		util::slist fileNames;
		BOOST_FOREACH( ImageHolder::Vector::const_reference image, m_ViewerCore->getImageVector() ) {
			fileNames.push_back( image->getImageProperties().fileName );

			if( image->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ).size() ) {
				changedAttributesList.push_back( image->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ) );
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
			BOOST_FOREACH( ImageHolder::Vector::const_reference image, m_ViewerCore->getImageVector() ) {
				if( image->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ).size() ) {
					detailedText << image->getImageProperties().fileName << " : " << std::endl;
					BOOST_FOREACH( util::slist::const_reference changedAttribute, image->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ) ) {
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
				BOOST_FOREACH( ImageHolder::Vector::const_reference image, m_ViewerCore->getImageVector() ) {
					m_ViewerCore->getUICore()->toggleLoadingIcon( true, QString( "Saving image to " ) + image->getImageProperties().fileName.c_str() );
					isis::data::IOFactory::write( *image->getISISImage(), image->getImageProperties().fileName, "", "" );
				}
				m_ViewerCore->getUICore()->toggleLoadingIcon( false );
				break;
			}

		} else {
			QMessageBox msgBox;
			msgBox.setText( "No image has changed attributes! Will not save anything." );
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
			m_ViewerCore->getUICore()->toggleLoadingIcon( true, QString( "Saving image to " ) + filename );
			isis::data::IOFactory::write( *m_ViewerCore->getCurrentImage()->getISISImage(), filename.toStdString(), "", "" );
			m_ViewerCore->setCurrentPath( filename.toStdString() );
			m_ViewerCore->getUICore()->toggleLoadingIcon( false );
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
		BOOST_FOREACH( plugin::PluginLoader::PluginListType::const_reference plugin, m_ViewerCore->getPlugins() ) {
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

void MainWindow::updateRecentOpenList()
{
	QSignalMapper *signalMapper = new QSignalMapper( this );
	m_Interface.actionOpen_recent->setEnabled( !m_ViewerCore->getSettings()->getRecentFiles().empty() );
	//first we have to remove all actions
	BOOST_FOREACH( QList<QAction *>::const_reference action, m_Interface.actionOpen_recent->menu()->actions() ) {
		m_Interface.actionOpen_recent->menu()->removeAction( action );
	}
	BOOST_FOREACH( FileInformationMap::const_reference path, m_ViewerCore->getSettings()->getRecentFiles() ) {
		std::stringstream recentFileName;
		recentFileName << path.first;

		if( path.second.getImageType() == ImageHolder::statistical_image ) {
			recentFileName << " (zmap)";
		}

		if( !path.second.getDialect().empty() ) {
			recentFileName << " (Dialect: " << path.second.getDialect() <<  ")";
		}

		if( !path.second.getReadFormat().empty() ) {
			recentFileName << " (Readformat: " << path.second.getReadFormat() << ")";
		}

		QAction *recentAction = new QAction( recentFileName.str().c_str() , this );
		recentAction->setData( QVariant( path.first.c_str() ) );
		signalMapper->setMapping( recentAction, recentAction->data().toString() );

		m_Interface.actionOpen_recent->menu()->addAction( recentAction );
		connect( recentAction, SIGNAL( triggered() ), signalMapper, SLOT( map() ) );
	}
	connect( signalMapper, SIGNAL( mapped( QString ) ), this, SLOT( openRecentPath( QString ) ) );
}

void MainWindow::openRecentPath ( QString path )
{
	m_ViewerCore->openFile( m_ViewerCore->getSettings()->getRecentFiles().at( path.toStdString() ) );
}

void MainWindow::refreshUI()
{
	m_Interface.actionShow_Labels->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "showLabels" ) );
	m_Interface.actionShow_Crosshair->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "showCrosshair" ) );
	m_ViewerCore->setShowLabels( m_Interface.actionShow_Labels->isChecked() );
	m_Interface.actionPropagate_Zooming->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "propagateZooming" ) );
	m_RadiusSpin->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "minMaxSearchRadius" ) );
	m_Interface.actionGeometrical_View->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "showImagesGeometricalView" ) );
	m_Interface.actionGeometrical_View->setVisible( m_ViewerCore->hasWidget( m_ViewerCore->getSettings()->getPropertyAs<std::string>( "widgetLatched" ) )
			&& m_ViewerCore->hasWidget( m_ViewerCore->getSettings()->getPropertyAs<std::string>( "widgetGeometrical" ) )
												  );


	m_Interface.action_Save_Image->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionSave_all_Images->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionSave_Image->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionCreate_Screenshot->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionFind_Global_Max->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionFind_Global_Min->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionCenter_to_ca->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionShow_Crosshair->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionShow_scaling_option->setEnabled( m_ViewerCore->hasImage() );
	m_Interface.actionShow_Labels->setEnabled( m_ViewerCore->hasImage() );
	updateRecentOpenList();

	if( m_ViewerCore->getMode() == ViewerCoreBase::statistical_mode ) {
		setWindowTitle( QString(  m_ViewerCore->getSettings()->getPropertyAs<std::string>( "signature" ).c_str() ) + QString( "(statistical mode)" ) );
	} else  if ( m_ViewerCore->getMode() == ViewerCoreBase::default_mode ) {
		setWindowTitle( QString(  m_ViewerCore->getSettings()->getPropertyAs<std::string>( "signature" ).c_str() ) );
	}

	scalingWidget->synchronize();
}


void MainWindow::closeEvent( QCloseEvent * )
{
	m_ViewerCore->close();
	saveSettings();
	m_ViewerCore->getSettings()->save();
}


void MainWindow::findGlobalMin()
{
	if( m_ViewerCore->hasImage() ) {
		const int radius = m_RadiusSpin->value();
		const util::ivector4 minVoxel = operation::NativeImageOps::getGlobalMin( m_ViewerCore->getCurrentImage(),
										m_ViewerCore->getCurrentImage()->getImageProperties().voxelCoords,
										radius );
		m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( minVoxel ) );
	}
}

void MainWindow::findGlobalMax()
{
	if( m_ViewerCore->hasImage() ) {
		const int radius = m_RadiusSpin->value();


		const util::ivector4 maxVoxel = operation::NativeImageOps::getGlobalMax( m_ViewerCore->getCurrentImage(),
										m_ViewerCore->getCurrentImage()->getImageProperties().voxelCoords,
										radius );

		m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getISISImage()->getPhysicalCoordsFromIndex( maxVoxel ) );
	}
}

void MainWindow::dropEvent ( QDropEvent *e )
{
	m_ViewerCore->getUICore()->openFromDropEvent( e );
}


}
}
