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
 * Author: Erik Türke, tuerke@cbs.mpg.de
 *
 * qviewercore.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "qviewercore.hpp"
#include <DataStorage/io_factory.hpp>
#include <DataStorage/fileptr.hpp>
#include "nativeimageops.hpp"
#include "uicore.hpp"
#include <mainwindow.hpp>

#include <fstream>

namespace isis
{
namespace viewer
{

QViewerCore::QViewerCore ( const std::string &appName, const std::string &orgName, QWidget *parent )
	: ViewerCoreBase( ),
	  m_Settings ( new QSettings ( appName.c_str(), orgName.c_str() ) ),
	  m_Parent ( parent ),
	  m_CurrentPath ( QDir::currentPath().toStdString() ),
	  m_ProgressFeedback ( boost::shared_ptr<QProgressFeedback> ( new QProgressFeedback() ) ),
	  m_UI ( new isis::viewer::UICore ( this ) )
{
	m_FavFiles.setLimit(1000);
	m_RecentFiles.setLimit( getOptionMap()->getPropertyAs<uint16_t>("maxRecentOpenListSize") );
	QCoreApplication::setApplicationName ( QString ( appName.c_str() ) );
	QCoreApplication::setOrganizationName ( QString ( orgName.c_str() ) );
	QApplication::setStartDragTime ( 1000 );

	setParentWidget ( m_UI->getMainWindow() );
	data::IOFactory::setProgressFeedback ( m_ProgressFeedback );
	operation::NativeImageOps::setProgressFeedBack ( m_ProgressFeedback );
	loadSettings();
	getUICore()->refreshUI();
#ifdef _OPENMP
	const uint8_t nMaxThreads = omp_get_num_procs();
	if( getOptionMap()->getPropertyAs<uint8_t> ( "numberOfThreads" ) == 0 ) {
		if( nMaxThreads <= getOptionMap()->getPropertyAs<uint8_t>("initialMaxNumberThreads" ) ) {
			getOptionMap()->setPropertyAs<uint8_t> ( "numberOfThreads", nMaxThreads );
			getOptionMap()->setPropertyAs<bool> ( "useAllAvailableThreads", true );
		} else {
			getOptionMap()->setPropertyAs<uint8_t> ( "numberOfThreads", getOptionMap()->getPropertyAs<uint8_t>("initialMaxNumberThreads" ) );
		}
		getOptionMap()->setPropertyAs<bool> ( "enableMultithreading", true );
	}
	if( getOptionMap()->getPropertyAs<bool>( "useAllAvailableThreads" ) ) {
		getOptionMap()->setPropertyAs<uint8_t> ( "numberOfThreads", nMaxThreads );
	}
	omp_set_num_threads ( getOptionMap()->getPropertyAs<uint8_t> ( "numberOfThreads" ) );
	getOptionMap()->setPropertyAs<bool> ( "ompAvailable", true );
#else
	getOptionMap()->setPropertyAs<bool> ( "ompAvailable", false );
#endif
	checkForErrors();
}

void QViewerCore::checkForErrors()
{
	getSettings()->beginGroup("ErrorHandling");
	const bool vastExitedSuccessfully = getSettings()->value( "vastExitedSuccessfully", true ).toBool();
	if( !vastExitedSuccessfully && getOptionMap()->getPropertyAs<bool>("showCrashMessage") ) {
		QMessageBox msgBox;
		msgBox.setWindowFlags(Qt::WindowSystemMenuHint);
		
		QSpacerItem* horizontalSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
		QGridLayout* layout = (QGridLayout*)msgBox.layout();
		layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
		msgBox.setIcon( QMessageBox::Information );
		std::stringstream text;
		text << "vast crashed last time!";
		msgBox.setText( text.str().c_str() );
		msgBox.setInformativeText( "Below you see the developer information. Feel free to copy it and send it to the vast developer." );
		std::stringstream detailedText;
		detailedText << "Developer information: " << std::endl;
		data::FilePtr logFile( getCrashLogFilePath().c_str() );
		if( !logFile.good() ) {
			QMessageBox errorMessage;
			errorMessage.setIcon( QMessageBox::Critical );
			errorMessage.setText( "Could not open the crash log file!" );
			errorMessage.exec();
		} else {
			msgBox.setDetailedText( reinterpret_cast<const char*>( &logFile.at<uint8_t>(0)[0] ) ); //do not do this at home!!
			msgBox.setStandardButtons( QMessageBox::Ok );
			msgBox.exec();
		}
		
	} else {
		getSettings()->setValue("vastExitedSuccessfully", false );
		getSettings()->sync();
	}
	
	getSettings()->endGroup();
}


void QViewerCore::addMessageHandler ( qt4::QDefaultMessagePrint *handler )
{
	connect ( handler, SIGNAL ( commitMessage ( qt4::QMessage ) ) , this, SLOT ( receiveMessage ( qt4::QMessage ) ) );
}

void QViewerCore::addMessageHandlerDev ( qt4::QDefaultMessagePrint *handler )
{
	connect ( handler, SIGNAL ( commitMessage ( qt4::QMessage ) ) , this, SLOT ( receiveMessageDev(qt4::QMessage)) );
}

void QViewerCore::receiveMessage ( qt4::QMessage message )
{
	m_MessageLog.push_back ( message );
	getUICore()->showMessage ( message );
}

void QViewerCore::receiveMessageDev ( qt4::QMessage message )
{
	m_DevMessageLog.push_back( message );
}

void QViewerCore::receiveMessage ( std::string message )
{
	qt4::QMessage qmessage;
	qmessage.message = message;
	receiveMessage ( qmessage );
}


void QViewerCore::physicalCoordsChanged ( util::fvector4 physicalCoords )
{
	emitPhysicalCoordsChanged ( physicalCoords );
}


void QViewerCore::timestepChanged ( int timestep )
{
	if ( hasImage() )
	{

		if ( !getCurrentImage()->getImageSize() [3] > timestep )
		{
			timestep = getCurrentImage()->getImageSize() [3] - 1;
		}

		BOOST_FOREACH ( DataContainer::reference image, getDataContainer() )
		{
			if ( static_cast<size_t> ( timestep ) < image.second->getImageSize() [3] )
			{
				image.second->voxelCoords[3] = timestep;
			}
		}
		updateScene();
	}
}

std::list<boost::shared_ptr<ImageHolder> > QViewerCore::addImageList ( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType )
{
	getUICore()->getMainWindow()->setCursor( Qt::WaitCursor );
	QApplication::processEvents();
	std::list<boost::shared_ptr<ImageHolder> > retList = isis::viewer::ViewerCoreBase::addImageList ( imageList, imageType );
	getUICore()->getMainWindow()->setCursor( Qt::ArrowCursor );
	return retList;

}

void QViewerCore::setImageList ( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType )
{
	isis::viewer::ViewerCoreBase::setImageList ( imageList, imageType );


}
void QViewerCore::centerImages ( bool ca )
{
	if ( hasImage() )
	{
		if ( !ca )
		{
			const util::ivector4 size = getCurrentImage()->getImageSize();
			const util::ivector4 center ( size[0] / 2, size[1] / 2, size[2] / 2,
										  getCurrentImage()->voxelCoords[3] );
			getCurrentImage()->voxelCoords = center;
		}
		else
		{
			getCurrentImage()->physicalCoords = util::fvector4();
		}

		updateScene();
	}
}


void QViewerCore::setShowLabels ( bool l )
{
	getOptionMap()->setPropertyAs<bool> ( "showLabels", l );
	emitShowLabels ( l );
	updateScene();
}

void QViewerCore::setShowCrosshair ( bool c )
{
	getOptionMap()->setPropertyAs<bool> ( "showCrosshair", c );
	emitSetEnableCrosshair ( c );
	updateScene();
}


void QViewerCore::settingsChanged()
{
	BOOST_FOREACH ( UICore::WidgetMap::const_reference widget, getUICore()->getWidgets() )
	{
		widget.first->setInterpolationType ( static_cast<InterpolationType> ( getOptionMap()->getPropertyAs<uint8_t> ( "interpolationType" ) ) );
	}
	emitShowLabels ( getOptionMap()->getPropertyAs<bool> ( "showLabels" ) );
	m_UI->getMainWindow()->getInterface().actionPropagate_Zooming->setChecked ( getOptionMap()->getPropertyAs<bool> ( "propagateZooming" ) );

	if ( hasImage() )
	{
		if ( getCurrentImage()->imageType == ImageHolder::z_map )
		{
			getCurrentImage()->lut = getOptionMap()->getPropertyAs<std::string> ( "lutZMap" );
		}
		else
		{
			getCurrentImage()->lut = getOptionMap()->getPropertyAs<std::string> ( "lutStructural" );
		}

		if ( getMode() == ViewerCoreBase::zmap && getCurrentAnatomicalRefernce().get() )
		{
			getCurrentAnatomicalRefernce()->lut = getOptionMap()->getPropertyAs<std::string> ( "lutStructural" );
			getCurrentAnatomicalRefernce()->updateColorMap();
		}

		getCurrentImage()->updateColorMap();
	}
	if ( getMode() == ViewerCoreBase::zmap )
	{
		BOOST_FOREACH ( DataContainer::reference image, getDataContainer() )
		{
			if ( image.second->imageType == ImageHolder::structural_image )
			{
				image.second->lut = getOptionMap()->getPropertyAs<std::string> ( "lutStructural" );
				image.second->updateColorMap();
			}
		}
	}

	if ( getMode() == ViewerCoreBase::zmap && getOptionMap()->getPropertyAs<bool> ( "zmapGlobal" ) )
	{
		BOOST_FOREACH ( DataContainer::reference image, getDataContainer() )
		{
			if ( image.second->imageType == ImageHolder::z_map )
			{
				image.second->lut = getOptionMap()->getPropertyAs<std::string> ( "lutZMap" );
				image.second->updateColorMap();
			}
		}

	}

	updateScene();
	m_UI->refreshUI();
}

void QViewerCore::updateScene()
{
	emitUpdateScene();
}

void QViewerCore::zoomChanged ( float zoomFactor )
{
	if ( m_OptionsMap->getPropertyAs<bool> ( "propagateZooming" ) )
	{
		emitZoomChanged ( zoomFactor );
	}
}


void QViewerCore::addPlugin ( boost::shared_ptr< plugin::PluginInterface > plugin )
{
	if ( !m_Parent && plugin->isGUI() )
	{
		LOG ( Runtime, error )
				<< "Core does not own a parent. Before calling addPlugin/addPlugins you have to use setParentWidget!";
	}
	else
	{
		plugin->setViewerCore ( this );
		plugin->setParentWidget ( m_Parent );
		m_PluginList.push_back ( plugin );
	}
}

void QViewerCore::addPlugins ( isis::viewer::plugin::PluginLoader::PluginListType plugins )
{
	BOOST_FOREACH ( PluginListType::const_reference plugin, plugins )
	{
		addPlugin ( plugin );
	}
}


bool QViewerCore::callPlugin ( QString name )
{
	BOOST_FOREACH ( PluginListType::const_reference plugin, m_PluginList )
	{
		if ( plugin->getName() == name.toStdString() )
		{
			return plugin->call();
		}
	}
	LOG ( Runtime, error ) << "No such plugin " << name.toStdString() << "!";
	return false;
}

bool QViewerCore::attachImageToWidget ( boost::shared_ptr<ImageHolder> image, WidgetInterface *widget )
{
	if ( getUICore()->getWidgets().find ( widget ) == getUICore()->getWidgets().end() )
	{
		LOG ( Runtime, error ) << "There is no such widget "
							   << widget << ", so will not add image " << image->getFileNames().front() << " to it.";
		return false;
	}

	if ( std::find ( m_ImageList.begin(), m_ImageList.end(), image ) == m_ImageList.end() )
	{
		LOG ( Runtime, error ) << "There is no such image "
							   << image->getFileNames().front() << ", so will not add it to widget " << widget << ".";
		return false;
	}

	widget->addImage ( image );
	return true;
}

void QViewerCore::openPath ( const _internal::FileInformation &fileInfo )
{
	if ( !fileInfo.getFileName().empty() )
	{
		getUICore()->getMainWindow()->toggleLoadingIcon( true, QString( "Opening image " ) + fileInfo.getFileName().c_str() + QString("...") );
		QDir dir;
		setCurrentPath ( dir.absoluteFilePath ( fileInfo.getFileName().c_str() ).toStdString() );
		UICore::ViewWidgetEnsembleType ensemble;

		if ( getUICore()->getEnsembleList().size() )
		{
			ensemble = getUICore()->getEnsembleList().front();
		}
		boost::filesystem::path p ( fileInfo.getFileName() );

		std::list<data::Image> tempImgList = isis::data::IOFactory::load ( fileInfo.getFileName() , fileInfo.getReadFormat(), fileInfo.getDialect() );
		if( !tempImgList.empty() ) {
			m_RecentFiles.insert( std::make_pair<std::string, _internal::FileInformation>(fileInfo.getFileName(), fileInfo ) );
		}
		BOOST_FOREACH ( std::list<data::Image>::const_reference image, tempImgList )
		{
			boost::shared_ptr<ImageHolder> imageHolder = addImage ( image, fileInfo.getImageType() );
			checkForCaCp ( imageHolder );

			if ( ! ( getMode() == ViewerCoreBase::zmap && imageHolder->imageType == ImageHolder::structural_image ) )
			{
				if ( fileInfo.isNewEnsemble() )
				{
					ensemble = getUICore()->createViewWidgetEnsemble ( "" );

					//if we load a zmap we additionally add an anatomical image to the widget to make things easier for the user....
					if ( fileInfo.getImageType() == ImageHolder::z_map && m_CurrentAnatomicalReference.get() )
					{
						attachImageToWidget ( m_CurrentAnatomicalReference, ensemble[0].widgetImplementation );
						attachImageToWidget ( m_CurrentAnatomicalReference, ensemble[1].widgetImplementation );
						attachImageToWidget ( m_CurrentAnatomicalReference, ensemble[2].widgetImplementation );
					}
				}

				attachImageToWidget ( imageHolder, ensemble[0].widgetImplementation );
				attachImageToWidget ( imageHolder, ensemble[1].widgetImplementation );
				attachImageToWidget ( imageHolder, ensemble[2].widgetImplementation );
				setCurrentImage ( imageHolder );
			}
		}
		getUICore()->rearrangeViewWidgets();
		getUICore()->refreshUI();
		centerImages();
		getUICore()->getMainWindow()->toggleLoadingIcon( false );
	}
	
}

void QViewerCore::closeImage ( boost::shared_ptr<ImageHolder> image, bool refreshUI )
{
	BOOST_FOREACH ( std::list< WidgetInterface *>::const_reference widget, image->getWidgetList() )
	{
		widget->removeImage ( image );
	}

	if ( getCurrentImage().get() == image.get() )
	{
		std::list<boost::shared_ptr< ImageHolder > > tmpList;
		BOOST_FOREACH ( DataContainer::const_reference image, getDataContainer() )
		{
			tmpList.push_back ( image.second );
		}
		tmpList.erase ( std::find ( tmpList.begin(), tmpList.end(), image ) );

		if ( tmpList.size() )
		{
			setCurrentImage ( tmpList.front() );
		}
		else
		{
			setCurrentImage ( boost::shared_ptr<ImageHolder>() );
		}
	}

	getDataContainer().erase ( image->getFileNames().front() );

	if ( refreshUI )
	{
		getUICore()->refreshUI();
	}

	updateScene();
}

void QViewerCore::loadSettings()
{
	getUICore()->getMainWindow()->toggleLoadingIcon(true, QString("Loading user settings..." ) );
	getSettings()->beginGroup ( "ViewerCore" );
	getOptionMap()->setPropertyAs<std::string> ( "lutZMap", getSettings()->value ( "lutZMap", getOptionMap()->getPropertyAs<std::string> ( "lutZMap" ).c_str() ).toString().toStdString() );
	getOptionMap()->setPropertyAs<std::string> ( "lutStructural", getSettings()->value ( "lutStructural", getOptionMap()->getPropertyAs<std::string> ( "lutStructural" ).c_str() ).toString().toStdString() );
	getOptionMap()->setPropertyAs<bool> ( "propagateZooming", getSettings()->value ( "propagateZooming", false ).toBool() );
	getOptionMap()->setPropertyAs<uint8_t> ( "interpolationType", getSettings()->value ( "interpolationType", getOptionMap()->getPropertyAs<uint8_t> ( "interpolationType" ) ).toUInt() );
	getOptionMap()->setPropertyAs<bool> ( "showLabels", getSettings()->value ( "showLabels", false ).toBool() );
	getOptionMap()->setPropertyAs<bool> ( "showCrosshair", getSettings()->value ( "showCrosshair", true ).toBool() );
	getOptionMap()->setPropertyAs<uint16_t> ( "minMaxSearchRadius",
			getSettings()->value ( "minMaxSearchRadius", getOptionMap()->getPropertyAs<uint16_t> ( "minMaxSearchRadius" ) ).toUInt() );
	getOptionMap()->setPropertyAs<bool> ( "showAdvancedFileDialogOptions", getSettings()->value ( "showAdvancedFileDialogOptions", false ).toBool() );
	getOptionMap()->setPropertyAs<bool> ( "showFavoriteFileList", getSettings()->value ( "showFavoriteFileList", false ).toBool() );
	getOptionMap()->setPropertyAs<bool> ( "showStartWidget", getSettings()->value ( "showStartWidget", true ).toBool() );
	getOptionMap()->setPropertyAs<bool> ( "showCrashMessage", getSettings()->value ( "showCrashMessage", true ).toBool() );
	getOptionMap()->setPropertyAs<uint8_t> ( "numberOfThreads", getSettings()->value ( "numberOfThreads" ).toUInt() );
	getOptionMap()->setPropertyAs<bool> ( "enableMultithreading", getSettings()->value ( "enableMultithreading" ).toBool() );
	getOptionMap()->setPropertyAs<bool> ( "useAllAvailablethreads", getSettings()->value ( "useAllAvailableThreads" ).toBool() );
	getOptionMap()->setPropertyAs<bool> ( "histogramOmitZero", getSettings()->value ( "histogramOmitZero" ).toBool() );
	//screenshot stuff
	getOptionMap()->setPropertyAs<uint16_t> ( "screenshotWidth", getSettings()->value ( "screenshotWidth", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotWidth" ) ).toUInt() );
	getOptionMap()->setPropertyAs<uint16_t> ( "screenshotHeight", getSettings()->value ( "screenshotHeight", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotHeight" ) ).toUInt() );
	getOptionMap()->setPropertyAs<bool> ( "screenshotKeepAspectRatio", getSettings()->value ( "screenshotKeepAspectRatio", getOptionMap()->getPropertyAs<bool> ( "screenshotKeepAspectRatio" ) ).toBool() );
	getOptionMap()->setPropertyAs<uint8_t> ( "screenshotQuality", getSettings()->value ( "screenshotQuality", getOptionMap()->getPropertyAs<uint8_t> ( "screenshotQuality" ) ).toUInt() );
	getOptionMap()->setPropertyAs<uint16_t> ( "screenshotDPIX", getSettings()->value ( "screenshotDPIX", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotDPIX" ) ).toUInt() );
	getOptionMap()->setPropertyAs<uint16_t> ( "screenshotDPIY", getSettings()->value ( "screenshotDPIY", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotDPIY" ) ).toUInt() );
	getOptionMap()->setPropertyAs<bool> ( "screenshotManualScaling", getSettings()->value ( "screenshotManualScaling", getOptionMap()->getPropertyAs<bool> ( "screenshotManualScaling" ) ).toBool() );
	getSettings()->endGroup();
	m_RecentFiles.readFileInfortmationMap(getSettings(), "RecentImages");
	m_FavFiles.readFileInfortmationMap(getSettings(), "FavoriteImages");
	getUICore()->getMainWindow()->toggleLoadingIcon(false);
}


void QViewerCore::saveSettings()
{
	//saving the preferences to the profile file
	
	getSettings()->beginGroup ( "ViewerCore" );
	getSettings()->setValue ( "lutZMap", getOptionMap()->getPropertyAs<std::string> ( "lutZMap" ).c_str() );
	getSettings()->setValue ( "lutStructural", getOptionMap()->getPropertyAs<std::string> ( "lutStructural" ).c_str() );
	getSettings()->setValue ( "interpolationType", getOptionMap()->getPropertyAs<uint8_t> ( "interpolationType" ) );
	getSettings()->setValue ( "propagateZooming", getOptionMap()->getPropertyAs<bool> ( "propagateZooming" ) );
	getSettings()->setValue ( "minMaxSearchRadius", getOptionMap()->getPropertyAs<uint16_t> ( "minMaxSearchRadius" ) );
	getSettings()->setValue ( "showLabels", getOptionMap()->getPropertyAs<bool> ( "showLabels" ) );
	getSettings()->setValue ( "showCrosshair", getOptionMap()->getPropertyAs<bool> ( "showCrosshair" ) );
	getSettings()->setValue ( "showAdvancedFileDialogOptions", getOptionMap()->getPropertyAs<bool> ( "showAdvancedFileDialogOptions" ) );
	getSettings()->setValue ( "showFavoriteFileList", getOptionMap()->getPropertyAs<bool> ( "showFavoriteFileList" ) );
	getSettings()->setValue ( "showStartWidget", getOptionMap()->getPropertyAs<bool> ( "showStartWidget" ) );
	getSettings()->setValue ( "showCrashMessage", getOptionMap()->getPropertyAs<bool> ( "showCrashMessage" ) );	
	getSettings()->setValue ( "numberOfThreads", getOptionMap()->getPropertyAs<uint8_t> ( "numberOfThreads" ) );
	getSettings()->setValue ( "enableMultithreading", getOptionMap()->getPropertyAs<bool> ( "enableMultithreading" ) );
	getSettings()->setValue ( "useAllAvailablethreads", getOptionMap()->getPropertyAs<bool> ( "useAllAvailableThreads" ) );
	getSettings()->setValue ( "histogramOmitZero", getOptionMap()->getPropertyAs<bool> ( "histogramOmitZero" ) );
	//screenshot stuff
	getSettings()->setValue ( "screenshotWidth", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotWidth" ) );
	getSettings()->setValue ( "screenshotHeight", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotHeight" ) );
	getSettings()->setValue ( "screenshotKeepAspectRatio", getOptionMap()->getPropertyAs<bool> ( "screenshotKeepAspectRatio" ) );
	getSettings()->setValue ( "screenshotQuality", getOptionMap()->getPropertyAs<uint8_t> ( "screenshotQuality" ) );
	getSettings()->setValue ( "screenshotDPIX", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotDPIX" ) );
	getSettings()->setValue ( "screenshotDPIY", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotDPIY" ) );
	getSettings()->setValue ( "screenshotManualScaling", getOptionMap()->getPropertyAs<bool> ( "screenshotManualScaling" ) );

	getSettings()->endGroup();
	m_RecentFiles.writeFileInformationMap(getSettings(), "RecentImages" );
	m_FavFiles.writeFileInformationMap(getSettings(), "FavoriteImages" );
	getSettings()->sync();
}

void QViewerCore::close ()
{
	getSettings()->beginGroup("ErrorHandling");
	getSettings()->setValue( "vastExitedSuccessfully", true );
	getSettings()->sync();
	getSettings()->endGroup();
	
}

void QViewerCore::setMode ( ViewerCoreBase::Mode mode )
{
	m_Mode = mode;
	if( m_Mode == zmap ) {
		getUICore()->getMainWindow()->setWindowTitle( QString( m_OptionsMap->getPropertyAs<std::string>("signature").c_str() ) + QString("(zmap mode)" ) );
	} else {
		getUICore()->getMainWindow()->setWindowTitle( QString( m_OptionsMap->getPropertyAs<std::string>("signature").c_str() ) );
	}
}



}
}
