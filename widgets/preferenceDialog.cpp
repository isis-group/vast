#include "preferenceDialog.hpp"
#include "color.hpp"

namespace isis
{
namespace viewer
{

QPreferencesDialog::QPreferencesDialog( QWidget *parent, QViewerCore *core ):
	QDialog( parent ),
	m_ViewerCore( core )
{
	preferencesUi.setupUi( this );
	connect( preferencesUi.applyButton, SIGNAL( pressed() ), this, SLOT( apply() ) ) ;
	QSize size( QSize( preferencesUi.comboBox->size().width() - 70, preferencesUi.comboBox->height() -10 ) );
	preferencesUi.comboBox->setIconSize( size );
	unsigned short index = 0;
	BOOST_FOREACH( color::Color::LUTMapType::const_reference lut, m_ViewerCore->getColorHandler()->getLUTMap() ) {
			preferencesUi.comboBox->insertItem(index++, m_ViewerCore->getColorHandler()->getIcon( lut.first, size.width() ,size.height() ), QString( lut.first.c_str() ) ) ;
	}

	loadSettings();
}

void QPreferencesDialog::apply()
{
	saveSettings();
	m_ViewerCore->settingsChanged();
	m_ViewerCore->updateScene();
}


void QPreferencesDialog::closeEvent( QCloseEvent * )
{
	saveSettings();
	m_ViewerCore->settingsChanged();
	m_ViewerCore->updateScene();
}

void QPreferencesDialog::loadSettings()
{
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	preferencesUi.checkShowDesc->setChecked( m_ViewerCore->getSettings()->value( "showDesc", false ).toBool() );
	preferencesUi.comboInterpolation->setCurrentIndex( m_ViewerCore->getSettings()->value( "interpolationType", 0 ).toUInt() );
	preferencesUi.comboScaling->setCurrentIndex( m_ViewerCore->getSettings()->value( "scaling", 0 ).toUInt() );
	preferencesUi.checkShowLabels->setChecked( m_ViewerCore->getSettings()->value( "labels", false ).toBool() );
	preferencesUi.checkPropagateZooming->setChecked( m_ViewerCore->getSettings()->value( "propagateZooming", false ).toBool() );
	preferencesUi.comboBox->setCurrentIndex( m_ViewerCore->getSettings()->value( "lut", 0 ).toUInt() );
	m_ViewerCore->getSettings()->endGroup();
}

void QPreferencesDialog::saveSettings()
{
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	m_ViewerCore->getSettings()->setValue( "showDesc", preferencesUi.checkShowDesc->isChecked() );
	m_ViewerCore->getSettings()->setValue( "interpolationType", preferencesUi.comboInterpolation->currentIndex() );
	m_ViewerCore->getSettings()->setValue( "lut", preferencesUi.comboBox->currentIndex() );
	m_ViewerCore->getSettings()->setValue( "scaling", preferencesUi.comboScaling->currentIndex() );
	m_ViewerCore->getSettings()->setValue( "labels", preferencesUi.checkShowLabels->isChecked() );
	m_ViewerCore->getSettings()->setValue( "propagateZooming", preferencesUi.checkPropagateZooming->isChecked() );
	m_ViewerCore->getSettings()->endGroup();
	m_ViewerCore->getSettings()->sync();
}



}
}