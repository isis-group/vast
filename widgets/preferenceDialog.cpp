#include "preferenceDialog.hpp"
#include <qviewercore.hpp>

namespace isis
{
namespace viewer
{
namespace widget
{

PreferencesDialog::PreferencesDialog( QWidget *parent, QViewerCore *core ):
	QDialog( parent ),
	m_ViewerCore( core )
{
	preferencesUi.setupUi( this );
	connect( preferencesUi.comboBox, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	connect( preferencesUi.comboInterpolation, SIGNAL( activated( int ) ), this, SLOT( apply( int ) ) );
	connect( preferencesUi.enableMultithreading, SIGNAL( clicked( bool ) ), this, SLOT( toggleMultithreading( bool ) ) );
	connect( preferencesUi.useAllThreads, SIGNAL( clicked( bool ) ), this, SLOT( toggleUseAllThreads( bool ) ) );
	connect( preferencesUi.numberOfThreads, SIGNAL( valueChanged( int ) ), this, SLOT( numberOfThreadsChanged( int ) ) );

	QSize size( QSize( preferencesUi.comboBox->size().width() - 70, preferencesUi.comboBox->height() - 10 ) );
	preferencesUi.comboBox->setIconSize( size );

}

void PreferencesDialog::numberOfThreadsChanged( int threads )
{
	m_ViewerCore->getOptionMap()->setPropertyAs<uint8_t>( "numberOfThreads", threads );
}


void PreferencesDialog::toggleMultithreading( bool toggle )
{
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "enableMultithreading", toggle );
	preferencesUi.multithreadingFrame->setVisible( toggle );

	if( !toggle ) {
		m_ViewerCore->getOptionMap()->setPropertyAs<uint8_t>( "numberOfThreads", 1 );
	} else {
		m_ViewerCore->getOptionMap()->setPropertyAs<uint8_t>( "numberOfThreads", preferencesUi.numberOfThreads->value() );

		if( preferencesUi.useAllThreads->isChecked() ) {
			preferencesUi.numberOfThreads->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint8_t>( "maxNumberOfThreads" ) );
		} else {
			preferencesUi.numberOfThreads->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint8_t>( "numberOfThreads" ) );
		}
	}
}

void PreferencesDialog::toggleUseAllThreads( bool toggle )
{
	preferencesUi.numberOfThreads->setEnabled( !toggle );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "useAllAvailableThreads", toggle );

	if( toggle ) {
		preferencesUi.numberOfThreads->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint8_t>( "maxNumberOfThreads" ) );
	} else {
		preferencesUi.numberOfThreads->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint8_t>( "numberOfThreads" ) );
	}
}


void PreferencesDialog::apply( int /*dummy*/ )
{
	saveSettings();
	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->settingsChanged();
	m_ViewerCore->updateScene();
#ifdef _OPENMP
	omp_set_num_threads( m_ViewerCore->getOptionMap()->getPropertyAs<uint8_t>( "numberOfThreads" ) );
#endif
}


void PreferencesDialog::closeEvent( QCloseEvent * )
{
	apply();
}

void PreferencesDialog::loadSettings()
{
	preferencesUi.comboBox->clear();
	QSize size = preferencesUi.comboBox->iconSize();
	unsigned short index = 0;
	color::Color::ColormapMapType colorMap = util::Singletons::get<color::Color, 10>().getColormapMap();
	BOOST_FOREACH( color::Color::ColormapMapType::const_reference lut, colorMap ) {
		if( lut.first != std::string( "fallback" ) ) {
			preferencesUi.comboBox->insertItem( index++, util::Singletons::get<color::Color, 10>().getIcon( lut.first, size.width() , size.height() ), QString( lut.first.c_str() ) ) ;
		}
	}
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	preferencesUi.comboInterpolation->setCurrentIndex( m_ViewerCore->getSettings()->value( "interpolationType", 0 ).toUInt() );

	if( m_ViewerCore->hasImage() ) {
		preferencesUi.comboBox->setCurrentIndex( preferencesUi.comboBox->findText( m_ViewerCore->getCurrentImage()->lut.c_str() ) );
	}

	m_ViewerCore->getSettings()->endGroup();
	preferencesUi.checkLoadingScreen->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showLoadingWidget" ) );
	preferencesUi.checkStartUpScreen->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showStartWidget" ) );
	preferencesUi.enableMultithreading->setVisible( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "ompAvailable" ) );
	preferencesUi.multithreadingFrame->setVisible( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "ompAvailable" ) );
	
	//screenshot
	preferencesUi.screenshotQuality->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint8_t>("screenshotQuality") );
	preferencesUi.dpiX->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("screenshotDPIX") );
	preferencesUi.dpiY->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("screenshotDPIY") );
	preferencesUi.sizeX->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("screenshotWidth" ) );
	preferencesUi.sizeY->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("screenshotHeight" ) );
	preferencesUi.keepRatio->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>("screenshotKeepAspectRatio") );
	
	

	if( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "ompAvailable" ) ) {
		preferencesUi.enableMultithreading->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "enableMultithreading" ) );
		preferencesUi.numberOfThreads->setValue( m_ViewerCore->getOptionMap()->getPropertyAs<uint8_t>( "numberOfThreads" ) );
		preferencesUi.useAllThreads->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "useAllAvailablethreads" ) );
		preferencesUi.multithreadingFrame->setVisible( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "enableMultithreading" ) );
		preferencesUi.numberOfThreads->setEnabled( !m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "useAllAvailableThreads" ) );
		preferencesUi.numberOfThreads->setMinimum( 1 );
		preferencesUi.numberOfThreads->setMaximum( m_ViewerCore->getOptionMap()->getPropertyAs<uint8_t>( "maxNumberOfThreads" ) );
	}

}

void PreferencesDialog::saveSettings()
{
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	m_ViewerCore->getSettings()->setValue( "interpolationType", preferencesUi.comboInterpolation->currentIndex() );
	m_ViewerCore->getSettings()->endGroup();
	m_ViewerCore->getSettings()->sync();
	
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "showStartWidget", preferencesUi.checkStartUpScreen->isChecked() );
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>( "showLoadingWidget", preferencesUi.checkLoadingScreen->isChecked() );
	//screenshot
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>("screenshotKeepAspectRatio", preferencesUi.keepRatio->isChecked() );
	m_ViewerCore->getOptionMap()->setPropertyAs<uint8_t>("screenshotQuality", preferencesUi.screenshotQuality->value() );
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>("screenshotDPIX", preferencesUi.dpiX->value() );
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>("screenshotDPIY", preferencesUi.dpiY->value() );
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>("screenshotWidth", preferencesUi.sizeX->value() );
	m_ViewerCore->getOptionMap()->setPropertyAs<uint16_t>("screenshotHeight", preferencesUi.sizeY->value() );

	if( m_ViewerCore->hasImage() ) {
		m_ViewerCore->getCurrentImage()->lut = preferencesUi.comboBox->currentText().toStdString() ;
		if( m_ViewerCore->getCurrentImage()->imageType == ImageHolder::z_map ) {
			m_ViewerCore->getOptionMap()->setPropertyAs<std::string>("lutZMap", preferencesUi.comboBox->currentText().toStdString() );
		} else {
			m_ViewerCore->getOptionMap()->setPropertyAs<std::string>("lutAna", preferencesUi.comboBox->currentText().toStdString() );
		}
	}
}


}
}
}