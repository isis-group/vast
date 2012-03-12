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
	const uint16_t nMaxThreads = omp_get_num_procs();
	getOptionMap()->setPropertyAs<uint16_t>("maxNumberOfThreads", nMaxThreads);
	if( getOptionMap()->getPropertyAs<uint16_t> ( "numberOfThreads" ) == 0 ) {
		if( nMaxThreads <= getOptionMap()->getPropertyAs<uint16_t>("initialMaxNumberThreads" ) ) {
			getOptionMap()->setPropertyAs<uint16_t> ( "numberOfThreads", nMaxThreads );
			getOptionMap()->setPropertyAs<bool> ( "useAllAvailableThreads", true );
		} else {
			getOptionMap()->setPropertyAs<uint16_t> ( "numberOfThreads", getOptionMap()->getPropertyAs<uint16_t>("initialMaxNumberThreads" ) );
		}
		getOptionMap()->setPropertyAs<bool> ( "enableMultithreading", true );
	}
	if( getOptionMap()->getPropertyAs<bool>( "useAllAvailableThreads" ) ) {
		getOptionMap()->setPropertyAs<uint16_t> ( "numberOfThreads", nMaxThreads );
	}
	omp_set_num_threads ( getOptionMap()->getPropertyAs<uint16_t> ( "numberOfThreads" ) );
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
				image.second->getImageProperties().voxelCoords[3] = timestep;
			}
		}
		updateScene();
	}
}

ImageHolder::ImageListType QViewerCore::addImageList ( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType )
{
	std::list<boost::shared_ptr<ImageHolder> > retList = isis::viewer::ViewerCoreBase::addImageList ( imageList, imageType );
	return retList;

}

void QViewerCore::centerImages ( bool ca )
{
	if ( hasImage() )
	{
		if ( !ca )
		{
			const util::ivector4 size = getCurrentImage()->getImageSize();
			const util::ivector4 center ( size[0] / 2, size[1] / 2, size[2] / 2,
										  getCurrentImage()->getImageProperties().voxelCoords[3] );
			getCurrentImage()->getImageProperties().voxelCoords = center;
		}
		else
		{
			getCurrentImage()->getImageProperties().physicalCoords = util::fvector4();
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
	BOOST_FOREACH ( WidgetMap::const_reference widget, getUICore()->getWidgets() )
	{
		widget.first->setInterpolationType ( static_cast<InterpolationType> ( getOptionMap()->getPropertyAs<uint16_t> ( "interpolationType" ) ) );
	}
	emitShowLabels ( getOptionMap()->getPropertyAs<bool> ( "showLabels" ) );
	m_UI->getMainWindow()->getInterface().actionPropagate_Zooming->setChecked ( getOptionMap()->getPropertyAs<bool> ( "propagateZooming" ) );

	if ( hasImage() )
	{
		if ( getCurrentImage()->getImageProperties().imageType == ImageHolder::statistical_image )
		{
			getCurrentImage()->getImageProperties().lut = getOptionMap()->getPropertyAs<std::string> ( "lutZMap" );
		}
		else
		{
			getCurrentImage()->getImageProperties().lut = getOptionMap()->getPropertyAs<std::string> ( "lutStructural" );
		}

		if ( getMode() == ViewerCoreBase::statistical_mode && getCurrentAnatomicalRefernce().get() )
		{
			getCurrentAnatomicalRefernce()->getImageProperties().lut = getOptionMap()->getPropertyAs<std::string> ( "lutStructural" );
			getCurrentAnatomicalRefernce()->updateColorMap();
		}

		getCurrentImage()->updateColorMap();
	}
	if ( getMode() == ViewerCoreBase::statistical_mode )
	{
		BOOST_FOREACH ( DataContainer::reference image, getDataContainer() )
		{
			if ( image.second->getImageProperties().imageType == ImageHolder::structural_image )
			{
				image.second->getImageProperties().lut = getOptionMap()->getPropertyAs<std::string> ( "lutStructural" );
				image.second->updateColorMap();
			}
		}
	}

	if ( getMode() == ViewerCoreBase::statistical_mode && getOptionMap()->getPropertyAs<bool> ( "zmapGlobal" ) )
	{
		BOOST_FOREACH ( DataContainer::reference image, getDataContainer() )
		{
			if ( image.second->getImageProperties().imageType == ImageHolder::statistical_image )
			{
				image.second->getImageProperties().lut = getOptionMap()->getPropertyAs<std::string> ( "lutZMap" );
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

widget::WidgetInterface* QViewerCore::getWidget ( const std::string& identifier ) throw( std::runtime_error & )
{
	widget::WidgetLoader::WidgetMapType widgetMap = util::Singletons::get<widget::WidgetLoader, 10>().getWidgetMap();
	if( widgetMap.empty() ) {
		throw( std::runtime_error( "Could not find any widget!" ) );
	}
	if( widgetMap.find(identifier) != widgetMap.end() ) {
		LOG(Dev, info ) << "Loading widget of identifier \"" << identifier << "\".";
		return widgetMap.at(identifier)();
	} else {
		LOG( Dev, error ) << "Can not find any widget with identifier \"" << identifier
			<< "\"! Returning fallback widget.";
		return getWidget( getOptionMap()->getPropertyAs<std::string>("fallbackWidgetIdentifier") );
	}
}
const util::PropertyMap* QViewerCore::getWidgetProperties ( const std::string& identifier ) 
{
	widget::WidgetLoader::WidgetPropertyMapType widgetPropertyMap = util::Singletons::get<widget::WidgetLoader, 10>().getWidgetPropertyMap();
	if( widgetPropertyMap.empty() ) {
		throw( std::runtime_error( "Could not find any widget!" ) );
	}
	if( widgetPropertyMap.find(identifier) != widgetPropertyMap.end() ) {
		LOG(Dev, info ) << "Loading widget properties of identifier \"" << identifier << "\".";
		return widgetPropertyMap.at(identifier);
	} else {
		LOG( Dev, error ) << "Can not find any widget properties with identifier \"" << identifier
			<< "\"! Returning properties of fallback widget i can find.";
		return getWidgetProperties( getOptionMap()->getPropertyAs<std::string>("fallbackWidgetIdentifier") );
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
	BOOST_FOREACH ( plugin::PluginLoader::PluginListType::const_reference plugin, plugins )
	{
		addPlugin ( plugin );
	}
}


bool QViewerCore::callPlugin ( QString name )
{
	BOOST_FOREACH ( plugin::PluginLoader::PluginListType::const_reference plugin, m_PluginList )
	{
		if ( plugin->getName() == name.toStdString() )
		{
			return plugin->call();
		}
	}
	LOG ( Runtime, error ) << "No such plugin " << name.toStdString() << "!";
	return false;
}

bool QViewerCore::attachImageToWidget ( boost::shared_ptr<ImageHolder> image, widget::WidgetInterface *widget )
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

void QViewerCore::openPath ( const FileInformation &fileInfo )
{
	if ( !fileInfo.getFileName().empty() )
	{
		util::istring dialect = fileInfo.getDialect();
		LOG( Dev, info ) << "Opening path " << fileInfo.getFileName() << " with rdialect: "
						<< fileInfo.getDialect() << ", rf: " << fileInfo.getReadFormat()
						<< ", widget: " << fileInfo.getWidgetIdentifier();
		getUICore()->getMainWindow()->toggleLoadingIcon( true, QString( "Opening image " ) + fileInfo.getFileName().c_str() + QString("...") );
		QDir dir;
		setCurrentPath ( dir.absoluteFilePath ( fileInfo.getFileName().c_str() ).toStdString() );
		//add this file to the recent opened files

		boost::filesystem::path p ( fileInfo.getFileName() );
		//this is a vista thing. if we load a vista image and the option "visualizeOnlyFirstVista" is enabled we should do so
		if( boost::filesystem::extension(p) == std::string("v") && getOptionMap()->getPropertyAs<bool>("visualizeOnlyFirstVista") && !dialect.size() )
		{
			dialect = util::istring("onlyfirst");
		}
		//show loading information
		std::stringstream loadingStream;
		loadingStream << "Loading image " << fileInfo.getFileName() << "...";
		getUICore()->getMainWindow()->toggleLoadingIcon(true, loadingStream.str().c_str() );
		//load the file into an isis image
		std::list<data::Image> tempImgList = isis::data::IOFactory::load ( fileInfo.getFileName() , fileInfo.getReadFormat(), dialect );
		if( !tempImgList.empty() ) {
			m_RecentFiles.insertSave( fileInfo );
			LOG( Dev, info ) << "Loaded " << tempImgList.size() << " images from path " << fileInfo.getFileName();
		} else {
			LOG( Dev, error ) << "Tried to load " << fileInfo.getFileName() << ", but image list is empty.";
		}

		//creating the viewer image objects
		ImageHolder::ImageListType imgList = addImageList( tempImgList, fileInfo.getImageType() );

		//ok we are in statistical_mode mode
		if( getMode() == ViewerCoreBase::statistical_mode ) {
			//and we are getting a statistical_image
			
		} else if ( getMode() == ViewerCoreBase::default_mode ) {
		}
		
		
		

		


	} else {
		LOG( Dev, warning ) << "Tried to open path without any given filename!";
	}
	
}

void QViewerCore::closeImage ( boost::shared_ptr<ImageHolder> image, bool refreshUI )
{
	BOOST_FOREACH ( std::list< widget::WidgetInterface *>::const_reference widget, image->getWidgetList() )
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
	getOptionMap()->setPropertyAs<bool> ( "propagateZooming", getSettings()->value ( "propagateZooming", false ).toBool() );
	getOptionMap()->setPropertyAs<uint16_t> ( "interpolationType", getSettings()->value ( "interpolationType", getOptionMap()->getPropertyAs<uint16_t> ( "interpolationType" ) ).toUInt() );
	getOptionMap()->setPropertyAs<bool> ( "showLabels", getSettings()->value ( "showLabels", false ).toBool() );
	getOptionMap()->setPropertyAs<bool> ( "showCrosshair", getSettings()->value ( "showCrosshair", true ).toBool() );
	getOptionMap()->setPropertyAs<uint16_t> ( "minMaxSearchRadius",
			getSettings()->value ( "minMaxSearchRadius", getOptionMap()->getPropertyAs<uint16_t> ( "minMaxSearchRadius" ) ).toUInt() );
	getOptionMap()->setPropertyAs<bool> ( "showAdvancedFileDialogOptions", getSettings()->value ( "showAdvancedFileDialogOptions", false ).toBool() );
	getOptionMap()->setPropertyAs<bool> ( "showFavoriteFileList", getSettings()->value ( "showFavoriteFileList", false ).toBool() );
	getOptionMap()->setPropertyAs<bool> ( "showStartWidget", getSettings()->value ( "showStartWidget", true ).toBool() );
	getOptionMap()->setPropertyAs<bool> ( "showCrashMessage", getSettings()->value ( "showCrashMessage", true ).toBool() );
	getOptionMap()->setPropertyAs<uint16_t> ( "numberOfThreads", getSettings()->value ( "numberOfThreads" ).toUInt() );
	getOptionMap()->setPropertyAs<bool> ( "enableMultithreading", getSettings()->value ( "enableMultithreading" ).toBool() );
	getOptionMap()->setPropertyAs<bool> ( "useAllAvailablethreads", getSettings()->value ( "useAllAvailableThreads" ).toBool() );
	getOptionMap()->setPropertyAs<bool> ( "histogramOmitZero", getSettings()->value ( "histogramOmitZero" ).toBool() );
	getOptionMap()->setPropertyAs<bool>( "visualizeOnlyFirstVista", getSettings()->value( "visualizeOnlyFirstVista", getOptionMap()->getPropertyAs<bool>("visualizeOnlyFirstVista") ).toBool() );
	getOptionMap()->setPropertyAs<std::string>( "defaultViewWidgetIdentifier", getSettings()->value( "defaultViewWidgetIdentifier", getOptionMap()->getPropertyAs<std::string>( "defaultViewWidgetIdentifier" ).c_str() ).toString().toStdString() );
	//screenshot stuff
	getOptionMap()->setPropertyAs<uint16_t> ( "screenshotWidth", getSettings()->value ( "screenshotWidth", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotWidth" ) ).toUInt() );
	getOptionMap()->setPropertyAs<uint16_t> ( "screenshotHeight", getSettings()->value ( "screenshotHeight", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotHeight" ) ).toUInt() );
	getOptionMap()->setPropertyAs<bool> ( "screenshotKeepAspectRatio", getSettings()->value ( "screenshotKeepAspectRatio", getOptionMap()->getPropertyAs<bool> ( "screenshotKeepAspectRatio" ) ).toBool() );
	getOptionMap()->setPropertyAs<uint16_t> ( "screenshotQuality", getSettings()->value ( "screenshotQuality", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotQuality" ) ).toUInt() );
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
	getSettings()->setValue( "visualizeOnlyFirstVista", getOptionMap()->getPropertyAs<bool>("visualizeOnlyFirstVista") );
	getSettings()->setValue ( "interpolationType", getOptionMap()->getPropertyAs<uint16_t> ( "interpolationType" ) );
	getSettings()->setValue ( "propagateZooming", getOptionMap()->getPropertyAs<bool> ( "propagateZooming" ) );
	getSettings()->setValue ( "minMaxSearchRadius", getOptionMap()->getPropertyAs<uint16_t> ( "minMaxSearchRadius" ) );
	getSettings()->setValue ( "showLabels", getOptionMap()->getPropertyAs<bool> ( "showLabels" ) );
	getSettings()->setValue ( "showCrosshair", getOptionMap()->getPropertyAs<bool> ( "showCrosshair" ) );
	getSettings()->setValue ( "showAdvancedFileDialogOptions", getOptionMap()->getPropertyAs<bool> ( "showAdvancedFileDialogOptions" ) );
	getSettings()->setValue ( "showFavoriteFileList", getOptionMap()->getPropertyAs<bool> ( "showFavoriteFileList" ) );
	getSettings()->setValue ( "showStartWidget", getOptionMap()->getPropertyAs<bool> ( "showStartWidget" ) );
	getSettings()->setValue ( "showCrashMessage", getOptionMap()->getPropertyAs<bool> ( "showCrashMessage" ) );	
	getSettings()->setValue ( "numberOfThreads", getOptionMap()->getPropertyAs<uint16_t> ( "numberOfThreads" ) );
	getSettings()->setValue ( "enableMultithreading", getOptionMap()->getPropertyAs<bool> ( "enableMultithreading" ) );
	getSettings()->setValue ( "useAllAvailablethreads", getOptionMap()->getPropertyAs<bool> ( "useAllAvailableThreads" ) );
	getSettings()->setValue ( "histogramOmitZero", getOptionMap()->getPropertyAs<bool> ( "histogramOmitZero" ) );
	getSettings()->setValue ( "defaultViewWidgetIdentifier", getOptionMap()->getPropertyAs<std::string>("defaultViewWidgetIdentifier").c_str() );
	//screenshot stuff
	getSettings()->setValue ( "screenshotWidth", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotWidth" ) );
	getSettings()->setValue ( "screenshotHeight", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotHeight" ) );
	getSettings()->setValue ( "screenshotKeepAspectRatio", getOptionMap()->getPropertyAs<bool> ( "screenshotKeepAspectRatio" ) );
	getSettings()->setValue ( "screenshotQuality", getOptionMap()->getPropertyAs<uint16_t> ( "screenshotQuality" ) );
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
	if( m_Mode == statistical_mode ) {
		getUICore()->getMainWindow()->setWindowTitle( QString(  m_OptionsMap->getPropertyAs<std::string>("signature").c_str() ) + QString("(zmap mode)" ) );
	} else {
		getUICore()->getMainWindow()->setWindowTitle( QString( m_OptionsMap->getPropertyAs<std::string>("signature").c_str() ) );
	}
}



}
}
