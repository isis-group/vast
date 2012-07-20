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
 * settings.cpp
 *
 * Description:
 *
 *  Created on: Mar 14, 2012
 *      Author: tuerke
 ******************************************************************/

#include "settings.hpp"
#include "viewercorebase.hpp"

namespace isis
{
namespace viewer
{

Settings::Settings()
	: m_QSettings( new QSettings() )
{
	initializeWithDefaultSettings();
}

void Settings::save()
{
	LOG( Dev, info ) << "Saving settings to " << m_QSettings->fileName().toStdString();
	m_QSettings->beginGroup ( "ViewerCore" );
	m_QSettings->setValue ( "lutZMap", getPropertyAs<std::string> ( "lutZMap" ).c_str() );
	m_QSettings->setValue( "visualizeOnlyFirstVista", getPropertyAs<bool>( "visualizeOnlyFirstVista" ) );
	m_QSettings->setValue( "ignoreOrientationAlways", getPropertyAs<bool>( "ignoreOrientationAlways" ) );
	m_QSettings->setValue ( "interpolationType", getPropertyAs<uint16_t> ( "interpolationType" ) );
	m_QSettings->setValue ( "checkCACP", getPropertyAs<bool> ( "checkCACP" ) );
	m_QSettings->setValue ( "useStyleSheet", getPropertyAs<bool> ( "useStyleSheet" ) );
	m_QSettings->setValue ( "setZeroToBlackStatistical", getPropertyAs<bool> ( "setZeroToBlackStatistical" ) );
	m_QSettings->setValue ( "setZeroToBlackStructural", getPropertyAs<bool> ( "setZeroToBlackStructural" ) );
	m_QSettings->setValue ( "propagateZooming", getPropertyAs<bool> ( "propagateZooming" ) );
	m_QSettings->setValue ( "latchSingleImage", getPropertyAs<bool> ( "latchSingleImage" ) );
	m_QSettings->setValue ( "showFullFilePath", getPropertyAs<bool> ( "showFullFilePath" ) );
	m_QSettings->setValue ( "viewAllImagesInStack", getPropertyAs<bool> ( "viewAllImagesInStack" ) );
	m_QSettings->setValue ( "showImagesGeometricalView", getPropertyAs<bool> ( "showImagesGeometricalView" ) );
	m_QSettings->setValue ( "propagateTimestepChange", getPropertyAs<bool> ( "propagateTimestepChange" ) );
	m_QSettings->setValue ( "minMaxSearchRadius", getPropertyAs<uint16_t> ( "minMaxSearchRadius" ) );
	m_QSettings->setValue ( "showLabels", getPropertyAs<bool> ( "showLabels" ) );
	m_QSettings->setValue ( "showCrosshair", getPropertyAs<bool> ( "showCrosshair" ) );
	m_QSettings->setValue ( "showAdvancedFileDialogOptions", getPropertyAs<bool> ( "showAdvancedFileDialogOptions" ) );
	m_QSettings->setValue ( "showFavoriteFileList", getPropertyAs<bool> ( "showFavoriteFileList" ) );
	m_QSettings->setValue ( "showStartWidget", getPropertyAs<bool> ( "showStartWidget" ) );
	m_QSettings->setValue ( "showCrashMessage", getPropertyAs<bool> ( "showCrashMessage" ) );
	m_QSettings->setValue ( "numberOfThreads", getPropertyAs<uint16_t> ( "numberOfThreads" ) );
	m_QSettings->setValue ( "enableMultithreading", getPropertyAs<bool> ( "enableMultithreading" ) );
	m_QSettings->setValue ( "useAllAvailablethreads", getPropertyAs<bool> ( "useAllAvailableThreads" ) );
	m_QSettings->setValue ( "histogramOmitZero", getPropertyAs<bool> ( "histogramOmitZero" ) );
	m_QSettings->setValue ( "defaultViewWidgetIdentifier", getPropertyAs<std::string>( "defaultViewWidgetIdentifier" ).c_str() );
	m_QSettings->setValue ( "styleSheet", getPropertyAs<std::string>( "styleSheet" ).c_str() );
	//screenshot stuff
	m_QSettings->setValue ( "screenshotWidth", getPropertyAs<uint16_t> ( "screenshotWidth" ) );
	m_QSettings->setValue ( "screenshotHeight", getPropertyAs<uint16_t> ( "screenshotHeight" ) );
	m_QSettings->setValue ( "screenshotKeepAspectRatio", getPropertyAs<bool> ( "screenshotKeepAspectRatio" ) );
	m_QSettings->setValue ( "screenshotQuality", getPropertyAs<uint16_t> ( "screenshotQuality" ) );
	m_QSettings->setValue ( "screenshotDPIX", getPropertyAs<uint16_t> ( "screenshotDPIX" ) );
	m_QSettings->setValue ( "screenshotDPIY", getPropertyAs<uint16_t> ( "screenshotDPIY" ) );
	m_QSettings->setValue ( "screenshotManualScaling", getPropertyAs<bool> ( "screenshotManualScaling" ) );

	m_QSettings->endGroup();
	m_RecentFiles.writeFileInformationMap( m_QSettings, "RecentImages" );
	m_FavFiles.writeFileInformationMap( m_QSettings, "FavoriteImages" );
	m_QSettings->sync();
	LOG( Dev, info ) << "Finished saving settings";
}

void Settings::load()
{
	LOG( Dev, info ) << "Loading settings from " << m_QSettings->fileName().toStdString();
	m_QSettings->beginGroup ( "ViewerCore" );

	setPropertyAs<std::string> ( "lutZMap", m_QSettings->value ( "lutZMap", getPropertyAs<std::string> ( "lutZMap" ).c_str() ).toString().toStdString() );
	setPropertyAs<bool> ( "setZeroToBlackStatistical", m_QSettings->value ( "setZeroToBlackStatistical", getPropertyAs<bool>( "setZeroToBlackStatistical" ) ).toBool() );
	setPropertyAs<bool> ( "setZeroToBlackStructural", m_QSettings->value ( "setZeroToBlackStructural", getPropertyAs<bool>( "setZeroToBlackStructural" ) ).toBool() );
	setPropertyAs<bool> ( "latchSingleImage", m_QSettings->value ( "latchSingleImage", getPropertyAs<bool>( "latchSingleImage" ) ).toBool() );
	setPropertyAs<bool> ( "showFullFilePath", m_QSettings->value ( "showFullFilePath", getPropertyAs<bool>( "showFullFilePath" ) ).toBool() );
	setPropertyAs<bool> ( "checkCACP", m_QSettings->value ( "checkCACP", getPropertyAs<bool>( "checkCACP" ) ).toBool() );
	setPropertyAs<bool> ( "propagateZooming", m_QSettings->value ( "propagateZooming", getPropertyAs<bool>( "propagateZooming" ) ).toBool() );
	setPropertyAs<bool> ( "viewAllImagesInStack", m_QSettings->value ( "viewAllImagesInStack", false ).toBool() );
	setPropertyAs<bool> ( "propagateTimestepChange", m_QSettings->value ( "propagateTimestepChange", false ).toBool() );
	setPropertyAs<bool> ( "useStyleSheet", m_QSettings->value ( "useStyleSheet", getPropertyAs<bool>( "useStyleSheet" ) ).toBool() );
	setPropertyAs<bool> ( "ignoreOrientationAlways", m_QSettings->value ( "ignoreOrientationAlways", getPropertyAs<bool>( "ignoreOrientationAlways" ) ).toBool() );
	setPropertyAs<uint16_t> ( "interpolationType", m_QSettings->value ( "interpolationType", getPropertyAs<uint16_t> ( "interpolationType" ) ).toUInt() );
	setPropertyAs<bool> ( "showLabels", m_QSettings->value ( "showLabels", false ).toBool() );
	setPropertyAs<bool> ( "showCrosshair", m_QSettings->value ( "showCrosshair", true ).toBool() );
	setPropertyAs<bool> ( "showImagesGeometricalView", m_QSettings->value ( "showImagesGeometricalView", getPropertyAs<bool>( "showImagesGeometricalView" ) ).toBool() );
	setPropertyAs<uint16_t> ( "minMaxSearchRadius",
							  m_QSettings->value ( "minMaxSearchRadius", getPropertyAs<uint16_t> ( "minMaxSearchRadius" ) ).toUInt() );
	setPropertyAs<bool> ( "showAdvancedFileDialogOptions", m_QSettings->value ( "showAdvancedFileDialogOptions", false ).toBool() );
	setPropertyAs<bool> ( "showFavoriteFileList", m_QSettings->value ( "showFavoriteFileList", false ).toBool() );
	setPropertyAs<bool> ( "showStartWidget", m_QSettings->value ( "showStartWidget", true ).toBool() );
	setPropertyAs<bool> ( "showCrashMessage", m_QSettings->value ( "showCrashMessage", true ).toBool() );
	setPropertyAs<uint16_t> ( "numberOfThreads", m_QSettings->value ( "numberOfThreads" ).toUInt() );
	setPropertyAs<bool> ( "enableMultithreading", m_QSettings->value ( "enableMultithreading" ).toBool() );
	setPropertyAs<bool> ( "useAllAvailablethreads", m_QSettings->value ( "useAllAvailableThreads" ).toBool() );
	setPropertyAs<bool> ( "histogramOmitZero", m_QSettings->value ( "histogramOmitZero" ).toBool() );
	setPropertyAs<bool>( "visualizeOnlyFirstVista", m_QSettings->value( "visualizeOnlyFirstVista", getPropertyAs<bool>( "visualizeOnlyFirstVista" ) ).toBool() );
	setPropertyAs<std::string>( "defaultViewWidgetIdentifier", m_QSettings->value( "defaultViewWidgetIdentifier", getPropertyAs<std::string>( "defaultViewWidgetIdentifier" ).c_str() ).toString().toStdString() );
	setPropertyAs<std::string>( "styleSheet", m_QSettings->value( "styleSheet", getPropertyAs<std::string>( "styleSheet" ).c_str() ).toString().toStdString() );
	//screenshot stuff
	setPropertyAs<uint16_t> ( "screenshotWidth", m_QSettings->value ( "screenshotWidth", getPropertyAs<uint16_t> ( "screenshotWidth" ) ).toUInt() );
	setPropertyAs<uint16_t> ( "screenshotHeight", m_QSettings->value ( "screenshotHeight", getPropertyAs<uint16_t> ( "screenshotHeight" ) ).toUInt() );
	setPropertyAs<bool> ( "screenshotKeepAspectRatio", m_QSettings->value ( "screenshotKeepAspectRatio", getPropertyAs<bool> ( "screenshotKeepAspectRatio" ) ).toBool() );
	setPropertyAs<uint16_t> ( "screenshotQuality", m_QSettings->value ( "screenshotQuality", getPropertyAs<uint16_t> ( "screenshotQuality" ) ).toUInt() );
	setPropertyAs<uint16_t> ( "screenshotDPIX", m_QSettings->value ( "screenshotDPIX", getPropertyAs<uint16_t> ( "screenshotDPIX" ) ).toUInt() );
	setPropertyAs<uint16_t> ( "screenshotDPIY", m_QSettings->value ( "screenshotDPIY", getPropertyAs<uint16_t> ( "screenshotDPIY" ) ).toUInt() );
	setPropertyAs<bool> ( "screenshotManualScaling", m_QSettings->value ( "screenshotManualScaling", getPropertyAs<bool> ( "screenshotManualScaling" ) ).toBool() );
	m_QSettings->endGroup();
	m_RecentFiles.readFileInfortmationMap( m_QSettings, "RecentImages" );
	m_FavFiles.readFileInfortmationMap( m_QSettings, "FavoriteImages" );
}


void Settings::initializeWithDefaultSettings()
{
	setPropertyAs<bool>( "checkCACP", false );
	setPropertyAs<bool>( "setZeroToBlackStatistical", true );
	setPropertyAs<bool>( "setZeroToBlackStructural", false );
	setPropertyAs<bool>( "latchSingleImage", false );
	setPropertyAs<bool>( "showFullFilePath", true );
	setPropertyAs<bool>( "zmapGlobal", false );
	setPropertyAs<bool>( "viewAllImagesInStack", false );
	setPropertyAs<bool>( "visualizeOnlyFirstVista", false );
	setPropertyAs<bool>( "propagateZooming", false );
	setPropertyAs<bool>( "propagateTimestepChange", false );
	setPropertyAs<uint16_t>( "interpolationType" , 0 );
	setPropertyAs<bool>( "showLables", false );
	setPropertyAs<bool>( "showCrosshair", true );
	setPropertyAs<float>( "currentGlobalZoom", 1.0 );
	setPropertyAs<uint16_t>( "minMaxSearchRadius", 20 );
	setPropertyAs<bool>( "showAdvancedFileDialogOptions", false );
	setPropertyAs<bool>( "showFavoriteFileList", false );
	setPropertyAs<uint16_t>( "maxWidgetHeight", 200 );
	setPropertyAs<uint16_t>( "maxWidgetWidth", 200 );
	setPropertyAs<uint16_t>( "maxOptionWidgetHeight", 120 );
	setPropertyAs<uint16_t>( "minOptionWidgetHeight", 90 );
	setPropertyAs<bool>( "showStartWidget", true );
	setPropertyAs<bool>( "showCrashMessage", true );
	setPropertyAs<uint16_t>( "startWidgetHeight", 600 );
	setPropertyAs<uint16_t>( "startWidgetWidth", 400 );
	setPropertyAs<uint16_t>( "viewerWidgetMargin", 5 );
	setPropertyAs<std::string>( "fallbackWidgetIdentifier", "qt4_plane_widget" );
	setPropertyAs<std::string>( "defaultViewWidgetIdentifier", "qt4_geometrical_plane_widget" );
	setPropertyAs<std::string>( "widgetLatched", "qt4_plane_widget" );
	setPropertyAs<std::string>( "widgetGeometrical", "qt4_geometrical_plane_widget" );
	setPropertyAs<bool>( "showImagesGeometricalView", false );
	setPropertyAs<bool>("ignoreOrientationAlways", false );

	setPropertyAs<bool>( "useStyleSheet", false );
	setPropertyAs<std::string>( "styleSheet", "fancy" );
	//screenshot
	setPropertyAs<uint16_t>( "screenshotQuality", 70 );
	setPropertyAs<uint16_t>( "screenshotWidth", 700 );
	setPropertyAs<uint16_t>( "screenshotHeight", 700 );
	setPropertyAs<uint16_t>( "screenshotDPIX", 300 );
	setPropertyAs<uint16_t>( "screenshotDPIY", 300 );
	setPropertyAs<bool>( "screenshotManualScaling", false );
	setPropertyAs<bool>( "screenshotKeepAspectRatio", true );
	//lut
	setPropertyAs<std::string>( "lutStructural", "standard_grey_values" );
	setPropertyAs<std::string>( "lutZMap", "standard_zmap" );
	//misc
	setPropertyAs<uint16_t>( "timeseriesPlayDelayTime", 50 );
	setPropertyAs<bool>( "histogramOmitZero", true );
	setPropertyAs<uint16_t>( "maxRecentOpenListSize", 10 );

	setPropertyAs<std::string>( "vastSymbol", std::string( ":/common/vast.png" ) );

	std::stringstream signature;
	signature << "vast v" << ViewerCoreBase::getVersion() ;
	setPropertyAs<std::string>( "signature", signature.str() );
	setPropertyAs<std::string>( "copyright", "2012 Max Planck Institute for Human and Brain Science Leipzig" );
}




}
}//end namespace