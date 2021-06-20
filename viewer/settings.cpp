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
	m_QSettings->setValue ( "lutZMap", getValueAs<std::string> ( "lutZMap" ).c_str() );
	m_QSettings->setValue( "visualizeOnlyFirstVista", getValueAs<bool>( "visualizeOnlyFirstVista" ) );
	m_QSettings->setValue( "ignoreOrientationAlways", getValueAs<bool>( "ignoreOrientationAlways" ) );
	m_QSettings->setValue ( "interpolationType", getValueAs<uint16_t> ( "interpolationType" ) );
	m_QSettings->setValue ( "checkCACP", getValueAs<bool> ( "checkCACP" ) );
	m_QSettings->setValue ( "useStyleSheet", getValueAs<bool> ( "useStyleSheet" ) );
	m_QSettings->setValue ( "setZeroToBlackStatistical", getValueAs<bool> ( "setZeroToBlackStatistical" ) );
	m_QSettings->setValue ( "setZeroToBlackStructural", getValueAs<bool> ( "setZeroToBlackStructural" ) );
	m_QSettings->setValue ( "propagateZooming", getValueAs<bool> ( "propagateZooming" ) );
	m_QSettings->setValue ( "latchSingleImage", getValueAs<bool> ( "latchSingleImage" ) );
	m_QSettings->setValue ( "showFullFilePath", getValueAs<bool> ( "showFullFilePath" ) );
	m_QSettings->setValue ( "viewAllImagesInStack", getValueAs<bool> ( "viewAllImagesInStack" ) );
	m_QSettings->setValue ( "showImagesGeometricalView", getValueAs<bool> ( "showImagesGeometricalView" ) );
	m_QSettings->setValue ( "propagateTimestepChange", getValueAs<bool> ( "propagateTimestepChange" ) );
	m_QSettings->setValue ( "minMaxSearchRadius", getValueAs<uint16_t> ( "minMaxSearchRadius" ) );
	m_QSettings->setValue ( "showLabels", getValueAs<bool> ( "showLabels" ) );
	m_QSettings->setValue ( "showCrosshair", getValueAs<bool> ( "showCrosshair" ) );
	m_QSettings->setValue ( "showAdvancedFileDialogOptions", getValueAs<bool> ( "showAdvancedFileDialogOptions" ) );
	m_QSettings->setValue ( "showFavoriteFileList", getValueAs<bool> ( "showFavoriteFileList" ) );
	m_QSettings->setValue ( "showStartWidget", getValueAs<bool> ( "showStartWidget" ) );
	m_QSettings->setValue ( "showCrashMessage", getValueAs<bool> ( "showCrashMessage" ) );
	m_QSettings->setValue ( "numberOfThreads", getValueAs<uint16_t> ( "numberOfThreads" ) );
	m_QSettings->setValue ( "enableMultithreading", getValueAs<bool> ( "enableMultithreading" ) );
	m_QSettings->setValue ( "useAllAvailablethreads", getValueAs<bool> ( "useAllAvailableThreads" ) );
	m_QSettings->setValue ( "histogramOmitZero", getValueAs<bool> ( "histogramOmitZero" ) );
	m_QSettings->setValue ( "defaultViewWidgetIdentifier", getValueAs<std::string>( "defaultViewWidgetIdentifier" ).c_str() );
	m_QSettings->setValue ( "styleSheet", getValueAs<std::string>( "styleSheet" ).c_str() );
	//screenshot stuff
	m_QSettings->setValue ( "screenshotWidth", getValueAs<uint16_t> ( "screenshotWidth" ) );
	m_QSettings->setValue ( "screenshotHeight", getValueAs<uint16_t> ( "screenshotHeight" ) );
	m_QSettings->setValue ( "screenshotKeepAspectRatio", getValueAs<bool> ( "screenshotKeepAspectRatio" ) );
	m_QSettings->setValue ( "screenshotQuality", getValueAs<uint16_t> ( "screenshotQuality" ) );
	m_QSettings->setValue ( "screenshotDPIX", getValueAs<uint16_t> ( "screenshotDPIX" ) );
	m_QSettings->setValue ( "screenshotDPIY", getValueAs<uint16_t> ( "screenshotDPIY" ) );
	m_QSettings->setValue ( "screenshotManualScaling", getValueAs<bool> ( "screenshotManualScaling" ) );

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

	setValueAs( "lutZMap", m_QSettings->value ( "lutZMap", getValueAs<std::string> ( "lutZMap" ).c_str() ).toString().toStdString() );
	setValueAs ( "setZeroToBlackStatistical", m_QSettings->value ( "setZeroToBlackStatistical", getValueAs<bool>( "setZeroToBlackStatistical" ) ).toBool() );
	setValueAs ( "setZeroToBlackStructural", m_QSettings->value ( "setZeroToBlackStructural", getValueAs<bool>( "setZeroToBlackStructural" ) ).toBool() );
	setValueAs ( "latchSingleImage", m_QSettings->value ( "latchSingleImage", getValueAs<bool>( "latchSingleImage" ) ).toBool() );
	setValueAs ( "showFullFilePath", m_QSettings->value ( "showFullFilePath", getValueAs<bool>( "showFullFilePath" ) ).toBool() );
	setValueAs ( "checkCACP", m_QSettings->value ( "checkCACP", getValueAs<bool>( "checkCACP" ) ).toBool() );
	setValueAs ( "propagateZooming", m_QSettings->value ( "propagateZooming", getValueAs<bool>( "propagateZooming" ) ).toBool() );
	setValueAs ( "viewAllImagesInStack", m_QSettings->value ( "viewAllImagesInStack", false ).toBool() );
	setValueAs ( "propagateTimestepChange", m_QSettings->value ( "propagateTimestepChange", false ).toBool() );
	setValueAs ( "useStyleSheet", m_QSettings->value ( "useStyleSheet", getValueAs<bool>( "useStyleSheet" ) ).toBool() );
	setValueAs ( "ignoreOrientationAlways", m_QSettings->value ( "ignoreOrientationAlways", getValueAs<bool>( "ignoreOrientationAlways" ) ).toBool() );
	setValueAs ( "interpolationType", m_QSettings->value ( "interpolationType", getValueAs<uint16_t> ( "interpolationType" ) ).toUInt() );
	setValueAs ( "showLabels", m_QSettings->value ( "showLabels", false ).toBool() );
	setValueAs ( "showCrosshair", m_QSettings->value ( "showCrosshair", true ).toBool() );
	setValueAs ( "showImagesGeometricalView", m_QSettings->value ( "showImagesGeometricalView", getValueAs<bool>( "showImagesGeometricalView" ) ).toBool() );
	setValueAs ( "minMaxSearchRadius",
							  m_QSettings->value ( "minMaxSearchRadius", getValueAs<uint16_t> ( "minMaxSearchRadius" ) ).toUInt() );
	setValueAs ( "showAdvancedFileDialogOptions", m_QSettings->value ( "showAdvancedFileDialogOptions", false ).toBool() );
	setValueAs ( "showFavoriteFileList", m_QSettings->value ( "showFavoriteFileList", false ).toBool() );
	setValueAs ( "showStartWidget", m_QSettings->value ( "showStartWidget", true ).toBool() );
	setValueAs ( "showCrashMessage", m_QSettings->value ( "showCrashMessage", true ).toBool() );
	setValueAs<uint16_t> ( "numberOfThreads", m_QSettings->value ( "numberOfThreads" ).toUInt() );
	setValueAs ( "enableMultithreading", m_QSettings->value ( "enableMultithreading" ).toBool() );
	setValueAs ( "useAllAvailablethreads", m_QSettings->value ( "useAllAvailableThreads" ).toBool() );
	setValueAs ( "histogramOmitZero", m_QSettings->value ( "histogramOmitZero" ).toBool() );
	setValueAs( "visualizeOnlyFirstVista", m_QSettings->value( "visualizeOnlyFirstVista", getValueAs<bool>( "visualizeOnlyFirstVista" ) ).toBool() );
	setValueAs<std::string>( "defaultViewWidgetIdentifier", m_QSettings->value( "defaultViewWidgetIdentifier", getValueAs<std::string>( "defaultViewWidgetIdentifier" ).c_str() ).toString().toStdString() );
	setValueAs<std::string>( "styleSheet", m_QSettings->value( "styleSheet", getValueAs<std::string>( "styleSheet" ).c_str() ).toString().toStdString() );
	//screenshot stuff
	setValueAs<uint16_t> ( "screenshotWidth", m_QSettings->value ( "screenshotWidth", getValueAs<uint16_t> ( "screenshotWidth" ) ).toUInt() );
	setValueAs<uint16_t> ( "screenshotHeight", m_QSettings->value ( "screenshotHeight", getValueAs<uint16_t> ( "screenshotHeight" ) ).toUInt() );
	setValueAs ( "screenshotKeepAspectRatio", m_QSettings->value ( "screenshotKeepAspectRatio", getValueAs<bool> ( "screenshotKeepAspectRatio" ) ).toBool() );
	setValueAs<uint16_t> ( "screenshotQuality", m_QSettings->value ( "screenshotQuality", getValueAs<uint16_t> ( "screenshotQuality" ) ).toUInt() );
	setValueAs<uint16_t> ( "screenshotDPIX", m_QSettings->value ( "screenshotDPIX", getValueAs<uint16_t> ( "screenshotDPIX" ) ).toUInt() );
	setValueAs<uint16_t> ( "screenshotDPIY", m_QSettings->value ( "screenshotDPIY", getValueAs<uint16_t> ( "screenshotDPIY" ) ).toUInt() );
	setValueAs ( "screenshotManualScaling", m_QSettings->value ( "screenshotManualScaling", getValueAs<bool> ( "screenshotManualScaling" ) ).toBool() );
	m_QSettings->endGroup();
	m_RecentFiles.readFileInfortmationMap( m_QSettings, "RecentImages" );
	m_FavFiles.readFileInfortmationMap( m_QSettings, "FavoriteImages" );
}


void Settings::initializeWithDefaultSettings()
{
	setValueAs( "checkCACP", false );
	setValueAs( "setZeroToBlackStatistical", true );
	setValueAs( "setZeroToBlackStructural", false );
	setValueAs( "latchSingleImage", false );
	setValueAs( "showFullFilePath", true );
	setValueAs( "zmapGlobal", false );
	setValueAs( "viewAllImagesInStack", false );
	setValueAs( "visualizeOnlyFirstVista", false );
	setValueAs( "propagateZooming", false );
	setValueAs( "propagateTimestepChange", false );
	setValueAs<uint16_t>( "interpolationType" , 0 );
	setValueAs( "showLables", false );
	setValueAs( "showCrosshair", true );
	setValueAs<float>( "currentGlobalZoom", 1.0 );
	setValueAs<uint16_t>( "minMaxSearchRadius", 20 );
	setValueAs( "showAdvancedFileDialogOptions", false );
	setValueAs( "showFavoriteFileList", false );
	setValueAs<uint16_t>( "maxWidgetHeight", 200 );
	setValueAs<uint16_t>( "maxWidgetWidth", 200 );
	setValueAs<uint16_t>( "maxOptionWidgetHeight", 120 );
	setValueAs<uint16_t>( "minOptionWidgetHeight", 90 );
	setValueAs( "showStartWidget", true );
	setValueAs( "showCrashMessage", true );
	setValueAs<uint16_t>( "startWidgetHeight", 600 );
	setValueAs<uint16_t>( "startWidgetWidth", 400 );
	setValueAs<uint16_t>( "viewerWidgetMargin", 5 );
	setValueAs<std::string>( "fallbackWidgetIdentifier", "qt4_plane_widget" );
	setValueAs<std::string>( "defaultViewWidgetIdentifier", "qt4_geometrical_plane_widget" );
	setValueAs<std::string>( "widgetLatched", "qt4_plane_widget" );
	setValueAs<std::string>( "widgetGeometrical", "qt4_geometrical_plane_widget" );
	setValueAs( "showImagesGeometricalView", false );
	setValueAs("ignoreOrientationAlways", false );

	setValueAs( "useStyleSheet", false );
	setValueAs<std::string>( "styleSheet", "fancy" );
	//screenshot
	setValueAs<uint16_t>( "screenshotQuality", 70 );
	setValueAs<uint16_t>( "screenshotWidth", 700 );
	setValueAs<uint16_t>( "screenshotHeight", 700 );
	setValueAs<uint16_t>( "screenshotDPIX", 300 );
	setValueAs<uint16_t>( "screenshotDPIY", 300 );
	setValueAs( "screenshotManualScaling", false );
	setValueAs( "screenshotKeepAspectRatio", true );
	//lut
	setValueAs<std::string>( "lutStructural", "standard_grey_values" );
	setValueAs<std::string>( "lutZMap", "standard_zmap" );
	//misc
	setValueAs<uint16_t>( "timeseriesPlayDelayTime", 50 );
	setValueAs( "histogramOmitZero", true );
	setValueAs<uint16_t>( "maxRecentOpenListSize", 10 );

	setValueAs<std::string>( "vastSymbol", std::string( ":/common/vast.png" ) );

	std::stringstream signature;
	signature << "vast v" << ViewerCoreBase::getVersion() ;
	setValueAs<std::string>( "signature", signature.str() );
	setValueAs<std::string>( "copyright", "2012 Max Planck Institute for Human and Brain Science Leipzig" );
}




}
}//end namespace