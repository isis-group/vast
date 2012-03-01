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
	preferencesUi.setupUi( this );
	connect( preferencesUi.lutStructural, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	connect( preferencesUi.lutZmap, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	connect( preferencesUi.comboInterpolation, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	connect( preferencesUi.enableMultithreading, SIGNAL( clicked( bool ) ), this, SLOT( toggleMultithreading( bool ) ) );
	connect( preferencesUi.useAllThreads, SIGNAL( clicked( bool ) ), this, SLOT( toggleUseAllThreads( bool ) ) );
	connect( preferencesUi.numberOfThreads, SIGNAL( valueChanged( int ) ), this, SLOT( numberOfThreadsChanged( int ) ) );
	connect( preferencesUi.sizeX, SIGNAL( valueChanged( int ) ), this, SLOT( screenshotXChanged( int ) ) );

	QSize size( QSize( preferencesUi.lutStructural->size().width() / 4, preferencesUi.lutStructural->height() - 10 ) );
	preferencesUi.lutStructural->setIconSize( size );
	preferencesUi.lutZmap->setIconSize( size );
}

void PreferencesDialog::screenshotXChanged( int val )
{
	if( preferencesUi.keepRatio->isChecked() ) {
		if( m_ViewerCore->hasImage() ) {
			UICore::ViewWidgetEnsembleListType ensembleList = m_ViewerCore->getUICore()->getEnsembleList();
			//preparation
			ensembleList.front()[0].frame->setFrameStyle( 0 );
			ensembleList.front()[0].frame->setAutoFillBackground( false );
			const int widgetHeight = ensembleList.size() *  ensembleList.front()[0].placeHolder->height() + ( m_ViewerCore->getMode() == ViewerCoreBase::zmap ? 100 : 0 ) ;
			const int widgetWidth = 3 * ensembleList.front()[0].placeHolder->width();

			const double ratio = ( double )widgetHeight / widgetWidth;
			preferencesUi.sizeY->setValue( val * ratio );
		}
	}
}


void PreferencesDialog::numberOfThreadsChanged( int threads )
{
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>( "numberOfThreads", threads );
}


void PreferencesDialog::toggleMultithreading( bool toggle )
{
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "enableMultithreading", toggle );
	preferencesUi.multithreadingFrame->setVisible( toggle );

	if( !toggle ) {
		m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>( "numberOfThreads", 1 );
	} else {
		m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>( "numberOfThreads", preferencesUi.numberOfThreads->value() );

		if( preferencesUi.useAllThreads->isChecked() ) {
			preferencesUi.numberOfThreads->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "maxNumberOfThreads" ) );
		} else {
			preferencesUi.numberOfThreads->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "numberOfThreads" ) );
		}
	}
}

void PreferencesDialog::toggleUseAllThreads( bool toggle )
{
	preferencesUi.numberOfThreads->setEnabled( !toggle );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "useAllAvailableThreads", toggle );

	if( toggle ) {
		preferencesUi.numberOfThreads->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "maxNumberOfThreads" ) );
	} else {
		preferencesUi.numberOfThreads->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "numberOfThreads" ) );
	}
}


void PreferencesDialog::apply( int /*dummy*/ )
{
	saveSettings();
	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->settingsChanged();
	m_ViewerCore->updateScene();
#ifdef _OPENMP
	omp_set_num_threads( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "numberOfThreads" ) );
#endif
}


void PreferencesDialog::closeEvent( QCloseEvent * )
{
	apply();
}

void PreferencesDialog::loadSettings()
{
	preferencesUi.lutStructural->clear();
	preferencesUi.lutZmap->clear();
	const QSize size = preferencesUi.lutStructural->iconSize();
	unsigned short index = 0;
	color::Color::ColormapMapType colorMap = util::Singletons::get<color::Color, 10>().getColormapMap();
	BOOST_FOREACH( color::Color::ColormapMapType::const_reference lut, colorMap ) {
		if( lut.first != std::string( "fallback" ) ) {
			preferencesUi.lutStructural->insertItem( index++, util::Singletons::get<color::Color, 10>().getIcon( lut.first, size.width() , size.height() ), QString( lut.first.c_str() ) ) ;
			preferencesUi.lutZmap->insertItem( index++, util::Singletons::get<color::Color, 10>().getIcon( lut.first, size.width() , size.height() ), QString( lut.first.c_str() ) ) ;
		}
	}
	preferencesUi.comboInterpolation->setCurrentIndex( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "interpolationType" ) );

	if( m_ViewerCore->hasImage() ) {
		if( m_ViewerCore->getCurrentImage()->imageType == ImageHolder::z_map ) {
			preferencesUi.lutZmap->setCurrentIndex( preferencesUi.lutZmap->findText( m_ViewerCore->getCurrentImage()->lut.c_str() ) );
			preferencesUi.lutStructural->setCurrentIndex( preferencesUi.lutStructural->findText( m_ViewerCore->getOptionMap()->getPropertyAs<std::string>( "lutStructural" ).c_str() ) );
		} else if ( m_ViewerCore->getCurrentImage()->imageType == ImageHolder::structural_image ) {
			preferencesUi.lutStructural->setCurrentIndex( preferencesUi.lutStructural->findText( m_ViewerCore->getCurrentImage()->lut.c_str() ) );
			preferencesUi.lutZmap->setCurrentIndex( preferencesUi.lutZmap->findText( m_ViewerCore->getOptionMap()->getPropertyAs<std::string>( "lutZMap" ).c_str() ) );
		}
	}

	preferencesUi.checkStartUpScreen->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showStartWidget" ) );
	preferencesUi.checkCrashMessage->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showCrashMessage" ) );
	preferencesUi.checkOnlyFirst->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>("visualizeOnlyFirstVista") );
	preferencesUi.enableMultithreading->setVisible( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "ompAvailable" ) );
	preferencesUi.multithreadingFrame->setVisible( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "ompAvailable" ) );

	//screenshot
	preferencesUi.screenshotQuality->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "screenshotQuality" ) );
	preferencesUi.dpiX->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "screenshotDPIX" ) );
	preferencesUi.dpiY->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "screenshotDPIY" ) );
	preferencesUi.sizeX->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "screenshotWidth" ) );
	preferencesUi.sizeY->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "screenshotHeight" ) );
	preferencesUi.keepRatio->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "screenshotKeepAspectRatio" ) );
	preferencesUi.manualScaling->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "screenshotManualScaling" ) );
	preferencesUi.scalingFrame->setVisible( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "screenshotManualScaling" ) );

	if( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "ompAvailable" ) ) {
		preferencesUi.enableMultithreading->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "enableMultithreading" ) );
		preferencesUi.numberOfThreads->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "numberOfThreads" ) );
		preferencesUi.useAllThreads->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "useAllAvailablethreads" ) );
		preferencesUi.multithreadingFrame->setVisible( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "enableMultithreading" ) );
		preferencesUi.numberOfThreads->setEnabled( !m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "useAllAvailableThreads" ) );
		preferencesUi.numberOfThreads->setMinimum( 1 );
		preferencesUi.numberOfThreads->setMaximum( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "maxNumberOfThreads" ) );
	}

	screenshotXChanged( preferencesUi.sizeX->value() );

	preferencesUi.defaultViewWidgetComboBox->clear();
	const widget::WidgetLoader::WidgetMapType &widgetMap = util::Singletons::get<widget::WidgetLoader, 10>().getWidgetMap();
	preferencesUi.defaultViewWidgetFrame->setVisible( widgetMap.size() > 1 );
	BOOST_FOREACH( widget::WidgetLoader::WidgetMapType::const_reference w, widgetMap )
	{
		preferencesUi.defaultViewWidgetComboBox->addItem( w.first.c_str() );
	}
	preferencesUi.defaultViewWidgetComboBox->setCurrentIndex( preferencesUi.defaultViewWidgetComboBox->findText( m_ViewerCore->getOptionMap()->getPropertyAs<std::string>("defaultViewWidgetIdentifier").c_str() ) );
}

void PreferencesDialog::saveSettings()
{
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>( "interpolationType", preferencesUi.comboInterpolation->currentIndex() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "showStartWidget", preferencesUi.checkStartUpScreen->isChecked() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "showCrashMessage", preferencesUi.checkCrashMessage->isChecked() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "visualizeOnlyFirstVista", preferencesUi.checkOnlyFirst->isChecked() );
	//screenshot
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "screenshotKeepAspectRatio", preferencesUi.keepRatio->isChecked() );
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>( "screenshotQuality", preferencesUi.screenshotQuality->value() );
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>( "screenshotDPIX", preferencesUi.dpiX->value() );
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>( "screenshotDPIY", preferencesUi.dpiY->value() );
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>( "screenshotWidth", preferencesUi.sizeX->value() );
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>( "screenshotHeight", preferencesUi.sizeY->value() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "screenshotManualScaling", preferencesUi.manualScaling->isChecked() );
	m_ViewerCore->getOptionMap()->setPropertyAs<std::string>( "defaultViewWidgetIdentifier", preferencesUi.defaultViewWidgetComboBox->currentText().toStdString() );

	if( m_ViewerCore->hasImage() ) {
		if( m_ViewerCore->getCurrentImage()->imageType == ImageHolder::z_map ) {
			m_ViewerCore->getCurrentImage()->lut = preferencesUi.lutZmap->currentText().toStdString() ;
		} else if ( m_ViewerCore->getCurrentImage()->imageType == ImageHolder::structural_image ) {
			m_ViewerCore->getCurrentImage()->lut = preferencesUi.lutStructural->currentText().toStdString() ;
		}

		m_ViewerCore->getOptionMap()->setPropertyAs<std::string>( "lutZMap", preferencesUi.lutZmap->currentText().toStdString() );
		m_ViewerCore->getOptionMap()->setPropertyAs<std::string>( "lutStructural", preferencesUi.lutStructural->currentText().toStdString() );
	}
}


}
}
}