#include "QPreferencesDialog.hpp"

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
	QSize size( QSize( preferencesUi.comboBox->size().width() - 70, preferencesUi.comboBox->height() ) );
	preferencesUi.comboBox->setIconSize( size );
	preferencesUi.comboBox->insertItem( 0, QIcon( ":/lut/colormap9.png" ), QString( "standard grey values" ) );
	preferencesUi.comboBox->insertItem( 1, QIcon( ":/lut/lut_standard.png" ), QString( "standard zmap" ) );
	preferencesUi.comboBox->insertItem( 2, QIcon( ":/lut/colormap1.png" ), QString( "LUT 1" ) );
	preferencesUi.comboBox->insertItem( 3, QIcon( ":/lut/colormap2.png" ), QString( "LUT 2" ) );
	preferencesUi.comboBox->insertItem( 4, QIcon( ":/lut/colormap3.png" ), QString( "LUT 3" ) );
	preferencesUi.comboBox->insertItem( 5, QIcon( ":/lut/colormap4.png" ), QString( "LUT 4" ) );
	preferencesUi.comboBox->insertItem( 6, QIcon( ":/lut/colormap5.png" ), QString( "LUT 5" ) );
	preferencesUi.comboBox->insertItem( 7, QIcon( ":/lut/colormap6.png" ), QString( "LUT 6" ) );
	preferencesUi.comboBox->insertItem( 8, QIcon( ":/lut/colormap7.png" ), QString( "LUT 7" ) );
	preferencesUi.comboBox->insertItem( 9, QIcon( ":/lut/colormap8.png" ), QString( "LUT 8" ) );
	preferencesUi.comboBox->insertItem( 10, QIcon( ":/lut/colormap9.png" ), QString( "LUT 9" ) );
	preferencesUi.comboBox->insertItem( 11, QIcon( ":/lut/colormap10.png" ), QString( "LUT 10" ) );
	preferencesUi.comboBox->insertItem( 12, QIcon( ":/lut/colormap11.png" ), QString( "LUT 11" ) );


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