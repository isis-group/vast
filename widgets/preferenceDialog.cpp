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
 * preferenceDialog.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "preferenceDialog.hpp"
#include "../viewer/qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace ui
{

PreferencesDialog::PreferencesDialog( QWidget *parent, QViewerCore *core ):
	QDialog( parent ),
	m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	connect( m_Interface.sizeX, SIGNAL( valueChanged( int ) ), this, SLOT( screenshotXChanged( int ) ) );

	QSize size( QSize( m_Interface.lutStructural->size().width() / 4, m_Interface.lutStructural->height() - 10 ) );
	m_Interface.lutStructural->setIconSize( size );
	m_Interface.lutZmap->setIconSize( size );
}

void PreferencesDialog::showEvent ( QShowEvent * )
{
	disconnectSignals();
	loadSettings();
	connectSignals();
}

void PreferencesDialog::connectSignals()
{
	connect( m_Interface.lutStructural, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	connect( m_Interface.lutZmap, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	connect( m_Interface.comboInterpolation, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	connect( m_Interface.checkLatchOrientation, SIGNAL( toggled( bool ) ), this, SLOT( apply() ) );
	connect( m_Interface.checkCACP, SIGNAL( toggled( bool ) ), this, SLOT( apply() ) );

}

void PreferencesDialog::disconnectSignals()
{
	disconnect( m_Interface.lutStructural, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	disconnect( m_Interface.lutZmap, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	disconnect( m_Interface.comboInterpolation, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	disconnect( m_Interface.checkLatchOrientation, SIGNAL( toggled( bool ) ), this, SLOT( apply() ) );
	disconnect( m_Interface.checkCACP, SIGNAL( toggled( bool ) ), this, SLOT( apply() ) );
}


void PreferencesDialog::screenshotXChanged( int val )
{
	if( m_Interface.keepRatio->isChecked() ) {
		if( m_ViewerCore->hasImage() ) {
			WidgetEnsemble::Vector ensembleList = m_ViewerCore->getUICore()->getEnsembleList();
			//preparation
			ensembleList.front()->front()->getFrame()->setFrameStyle( 0 );
			ensembleList.front()->front()->getFrame()->setAutoFillBackground( false );
			const int widgetHeight = ensembleList.size() *  ensembleList.front()->front()->getPlaceHolder()->height() + ( m_ViewerCore->getMode() == ViewerCoreBase::statistical_mode ? 100 : 0 ) ;
			const int widgetWidth = 3 * ensembleList.front()->front()->getPlaceHolder()->width();

			const double ratio = ( double )widgetHeight / widgetWidth;
			m_Interface.sizeY->setValue( val * ratio );
		}
	}
}


void PreferencesDialog::apply( int /*dummy*/ )
{
	saveSettings();
	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->settingsChanged();
	m_ViewerCore->updateScene();
}


void PreferencesDialog::closeEvent( QCloseEvent * )
{
	apply();
	m_ViewerCore->getSettings()->save();
}

void PreferencesDialog::loadSettings()
{
	m_Interface.lutStructural->clear();
	m_Interface.lutZmap->clear();
	const QSize size = m_Interface.lutStructural->iconSize();
	unsigned short index = 0;
	color::Color::ColormapMapType colorMap = util::Singletons::get<color::Color, 10>().getColormapMap();
	BOOST_FOREACH( color::Color::ColormapMapType::const_reference lut, colorMap ) {
		if( lut.first != std::string( "fallback" ) ) {
			m_Interface.lutStructural->insertItem( index++, util::Singletons::get<color::Color, 10>().getIcon( lut.first, size.width() , size.height() ), QString( lut.first.c_str() ) ) ;
			m_Interface.lutZmap->insertItem( index++, util::Singletons::get<color::Color, 10>().getIcon( lut.first, size.width() , size.height() ), QString( lut.first.c_str() ) ) ;
		}
	}
	m_Interface.comboInterpolation->setCurrentIndex( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "interpolationType" ) );

	if( m_ViewerCore->hasImage() ) {
		if( m_ViewerCore->getCurrentImage()->getImageProperties().imageType == ImageHolder::statistical_image ) {
			m_Interface.lutZmap->setCurrentIndex( m_Interface.lutZmap->findText( m_ViewerCore->getCurrentImage()->getImageProperties().lut.c_str() ) );
			m_Interface.lutStructural->setCurrentIndex( m_Interface.lutStructural->findText( m_ViewerCore->getSettings()->getPropertyAs<std::string>( "lutStructural" ).c_str() ) );
		} else if ( m_ViewerCore->getCurrentImage()->getImageProperties().imageType == ImageHolder::structural_image ) {
			m_Interface.lutStructural->setCurrentIndex( m_Interface.lutStructural->findText( m_ViewerCore->getCurrentImage()->getImageProperties().lut.c_str() ) );
			m_Interface.lutZmap->setCurrentIndex( m_Interface.lutZmap->findText( m_ViewerCore->getSettings()->getPropertyAs<std::string>( "lutZMap" ).c_str() ) );
		}
	}

	m_Interface.fancyStyle->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "useStyleSheet" ) );
	m_Interface.checkStartUpScreen->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "showStartWidget" ) );
	m_Interface.checkCrashMessage->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "showCrashMessage" ) );
	m_Interface.checkOnlyFirst->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "visualizeOnlyFirstVista" ) );
	m_Interface.checkCACP->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "checkCACP" ) );
	m_Interface.checkVoxelSpace->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>("ignoreOrientationAlways") );

	//view tab
	m_Interface.checkLatchOrientation->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "latchSingleImage" ) );
	m_Interface.checkSetZeroStatistical->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "setZeroToBlackStatistical" ) );
	m_Interface.checkSetZeroStructural->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "setZeroToBlackStructural" ) );

	//screenshot
	m_Interface.screenshotQuality->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotQuality" ) );
	m_Interface.dpiX->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotDPIX" ) );
	m_Interface.dpiY->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotDPIY" ) );
	m_Interface.sizeX->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotWidth" ) );
	m_Interface.sizeY->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotHeight" ) );
	m_Interface.keepRatio->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "screenshotKeepAspectRatio" ) );
	m_Interface.manualScaling->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "screenshotManualScaling" ) );
	m_Interface.scalingFrame->setVisible( m_ViewerCore->getSettings()->getPropertyAs<bool>( "screenshotManualScaling" ) );

	screenshotXChanged( m_Interface.sizeX->value() );

	m_Interface.defaultViewWidgetComboBox->clear();
	const widget::WidgetLoader::WidgetMapType &widgetMap = util::Singletons::get<widget::WidgetLoader, 10>().getWidgetMap();
	const widget::WidgetLoader::WidgetPropertyMapType &optionsMap = util::Singletons::get<widget::WidgetLoader, 10>().getWidgetPropertyMap();
	m_Interface.defaultViewWidgetFrame->setVisible( widgetMap.size() > 1 );
	BOOST_FOREACH( widget::WidgetLoader::WidgetMapType::const_reference w, widgetMap ) {
		m_Interface.defaultViewWidgetComboBox->addItem( optionsMap.at( w.first )->getPropertyAs<std::string>( "widgetName" ).c_str(), QVariant( w.first.c_str() ) );
	}
	m_Interface.defaultViewWidgetComboBox->setCurrentIndex( m_Interface.defaultViewWidgetComboBox->findData( QVariant( m_ViewerCore->getSettings()->getPropertyAs<std::string>( "defaultViewWidgetIdentifier" ).c_str() ) ) );
}

void PreferencesDialog::saveSettings()
{
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "interpolationType", m_Interface.comboInterpolation->currentIndex() );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "showStartWidget", m_Interface.checkStartUpScreen->isChecked() );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "showCrashMessage", m_Interface.checkCrashMessage->isChecked() );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "useStyleSheet", m_Interface.fancyStyle->isChecked() );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "ignoreOrientationAlways", m_Interface.checkVoxelSpace->isChecked() );

	//view
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "latchSingleImage", m_Interface.checkLatchOrientation->isChecked() );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "setZeroToBlackStatistical", m_Interface.checkSetZeroStatistical->isChecked() );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "setZeroToBlackStructural", m_Interface.checkSetZeroStructural->isChecked() );

	//file formats
	//vista
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "visualizeOnlyFirstVista", m_Interface.checkOnlyFirst->isChecked() );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "checkCACP", m_Interface.checkCACP->isChecked() );

	//screenshot
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "screenshotKeepAspectRatio", m_Interface.keepRatio->isChecked() );
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "screenshotQuality", m_Interface.screenshotQuality->value() );
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "screenshotDPIX", m_Interface.dpiX->value() );
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "screenshotDPIY", m_Interface.dpiY->value() );
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "screenshotWidth", m_Interface.sizeX->value() );
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "screenshotHeight", m_Interface.sizeY->value() );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "screenshotManualScaling", m_Interface.manualScaling->isChecked() );

	m_ViewerCore->getSettings()->setPropertyAs<std::string>( "defaultViewWidgetIdentifier", m_Interface.defaultViewWidgetComboBox->itemData( m_Interface.defaultViewWidgetComboBox->currentIndex() ).toString().toStdString() );

	if( m_ViewerCore->hasImage() ) {
		if( m_ViewerCore->getCurrentImage()->getImageProperties().imageType == ImageHolder::statistical_image ) {
			m_ViewerCore->getCurrentImage()->getImageProperties().lut = m_Interface.lutZmap->currentText().toStdString() ;
		} else if ( m_ViewerCore->getCurrentImage()->getImageProperties().imageType == ImageHolder::structural_image ) {
			m_ViewerCore->getCurrentImage()->getImageProperties().lut = m_Interface.lutStructural->currentText().toStdString() ;
		}

		m_ViewerCore->getSettings()->setPropertyAs<std::string>( "lutZMap", m_Interface.lutZmap->currentText().toStdString() );
		m_ViewerCore->getSettings()->setPropertyAs<std::string>( "lutStructural", m_Interface.lutStructural->currentText().toStdString() );
	}
}


}
}
}
