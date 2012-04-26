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
 * preferenceDialog.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "preferenceDialog.hpp"
#include <qviewercore.hpp>

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
	connect( m_Interface.lutStructural, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	connect( m_Interface.lutZmap, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	connect( m_Interface.comboInterpolation, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	connect( m_Interface.enableMultithreading, SIGNAL( clicked( bool ) ), this, SLOT( toggleMultithreading( bool ) ) );
	connect( m_Interface.useAllThreads, SIGNAL( clicked( bool ) ), this, SLOT( toggleUseAllThreads( bool ) ) );
	connect( m_Interface.numberOfThreads, SIGNAL( valueChanged( int ) ), this, SLOT( numberOfThreadsChanged( int ) ) );
	connect( m_Interface.sizeX, SIGNAL( valueChanged( int ) ), this, SLOT( screenshotXChanged( int ) ) );

	QSize size( QSize( m_Interface.lutStructural->size().width() / 4, m_Interface.lutStructural->height() - 10 ) );
	m_Interface.lutStructural->setIconSize( size );
	m_Interface.lutZmap->setIconSize( size );
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


void PreferencesDialog::numberOfThreadsChanged( int threads )
{
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "numberOfThreads", threads );
}


void PreferencesDialog::toggleMultithreading( bool toggle )
{
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "enableMultithreading", toggle );
	m_Interface.multithreadingFrame->setVisible( toggle );

	if( !toggle ) {
		m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "numberOfThreads", 1 );
	} else {
		m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "numberOfThreads", m_Interface.numberOfThreads->value() );

		if( m_Interface.useAllThreads->isChecked() ) {
			m_Interface.numberOfThreads->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "maxNumberOfThreads" ) );
		} else {
			m_Interface.numberOfThreads->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "numberOfThreads" ) );
		}
	}
}

void PreferencesDialog::toggleUseAllThreads( bool toggle )
{
	m_Interface.numberOfThreads->setEnabled( !toggle );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "useAllAvailableThreads", toggle );

	if( toggle ) {
		m_Interface.numberOfThreads->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "maxNumberOfThreads" ) );
	} else {
		m_Interface.numberOfThreads->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "numberOfThreads" ) );
	}
}


void PreferencesDialog::apply( int /*dummy*/ )
{
	saveSettings();
	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->settingsChanged();
	m_ViewerCore->updateScene();
#ifdef _OPENMP
	omp_set_num_threads( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "numberOfThreads" ) );
#endif
}


void PreferencesDialog::closeEvent( QCloseEvent * )
{
	apply();
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

	m_Interface.checkStartUpScreen->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "showStartWidget" ) );
	m_Interface.checkCrashMessage->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "showCrashMessage" ) );
	m_Interface.checkOnlyFirst->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "visualizeOnlyFirstVista" ) );
	m_Interface.enableMultithreading->setVisible( m_ViewerCore->getSettings()->getPropertyAs<bool>( "ompAvailable" ) );
	m_Interface.multithreadingFrame->setVisible( m_ViewerCore->getSettings()->getPropertyAs<bool>( "ompAvailable" ) );

	//screenshot
	m_Interface.screenshotQuality->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotQuality" ) );
	m_Interface.dpiX->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotDPIX" ) );
	m_Interface.dpiY->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotDPIY" ) );
	m_Interface.sizeX->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotWidth" ) );
	m_Interface.sizeY->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotHeight" ) );
	m_Interface.keepRatio->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "screenshotKeepAspectRatio" ) );
	m_Interface.manualScaling->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "screenshotManualScaling" ) );
	m_Interface.scalingFrame->setVisible( m_ViewerCore->getSettings()->getPropertyAs<bool>( "screenshotManualScaling" ) );

	if( m_ViewerCore->getSettings()->getPropertyAs<bool>( "ompAvailable" ) ) {
		m_Interface.enableMultithreading->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "enableMultithreading" ) );
		m_Interface.numberOfThreads->setValue( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "numberOfThreads" ) );
		m_Interface.useAllThreads->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "useAllAvailablethreads" ) );
		m_Interface.multithreadingFrame->setVisible( m_ViewerCore->getSettings()->getPropertyAs<bool>( "enableMultithreading" ) );
		m_Interface.numberOfThreads->setEnabled( !m_ViewerCore->getSettings()->getPropertyAs<bool>( "useAllAvailableThreads" ) );
		m_Interface.numberOfThreads->setMinimum( 1 );
		m_Interface.numberOfThreads->setMaximum( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "maxNumberOfThreads" ) );
	}

	screenshotXChanged( m_Interface.sizeX->value() );

	m_Interface.defaultViewWidgetComboBox->clear();
	const widget::WidgetLoader::WidgetMapType &widgetMap = util::Singletons::get<widget::WidgetLoader, 10>().getWidgetMap();
	const widget::WidgetLoader::WidgetPropertyMapType &optionsMap = util::Singletons::get<widget::WidgetLoader, 10>().getWidgetPropertyMap();	
	m_Interface.defaultViewWidgetFrame->setVisible( widgetMap.size() > 1 );
	BOOST_FOREACH( widget::WidgetLoader::WidgetMapType::const_reference w, widgetMap ) {
		m_Interface.defaultViewWidgetComboBox->addItem( optionsMap.at( w.first)->getPropertyAs<std::string>("widgetName").c_str(), QVariant( w.first.c_str() ) );
	}
	m_Interface.defaultViewWidgetComboBox->setCurrentIndex( m_Interface.defaultViewWidgetComboBox->findData( QVariant( m_ViewerCore->getSettings()->getPropertyAs<std::string>( "defaultViewWidgetIdentifier" ).c_str() ) ) );
}

void PreferencesDialog::saveSettings()
{
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "interpolationType", m_Interface.comboInterpolation->currentIndex() );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "showStartWidget", m_Interface.checkStartUpScreen->isChecked() );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "showCrashMessage", m_Interface.checkCrashMessage->isChecked() );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "visualizeOnlyFirstVista", m_Interface.checkOnlyFirst->isChecked() );
	//screenshot
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "screenshotKeepAspectRatio", m_Interface.keepRatio->isChecked() );
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "screenshotQuality", m_Interface.screenshotQuality->value() );
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "screenshotDPIX", m_Interface.dpiX->value() );
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "screenshotDPIY", m_Interface.dpiY->value() );
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "screenshotWidth", m_Interface.sizeX->value() );
	m_ViewerCore->getSettings()->setPropertyAs<uint16_t>( "screenshotHeight", m_Interface.sizeY->value() );
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "screenshotManualScaling", m_Interface.manualScaling->isChecked() );
	
	m_ViewerCore->getSettings()->setPropertyAs<std::string>( "defaultViewWidgetIdentifier", m_Interface.defaultViewWidgetComboBox->itemData(m_Interface.defaultViewWidgetComboBox->currentIndex()).toString().toStdString() );

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