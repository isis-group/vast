#include "preferenceDialog.hpp"
#include "color.hpp"

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
	QSize size( QSize( preferencesUi.comboBox->size().width() - 70, preferencesUi.comboBox->height() - 10 ) );
	preferencesUi.comboBox->setIconSize( size );


}

void PreferencesDialog::apply( int dummy )
{
	saveSettings();
	m_ViewerCore->getUI()->refreshUI();
	m_ViewerCore->settingsChanged();
	m_ViewerCore->updateScene();
}


void PreferencesDialog::closeEvent( QCloseEvent * )
{
	saveSettings();
	m_ViewerCore->settingsChanged();
	m_ViewerCore->updateScene();
}

void PreferencesDialog::loadSettings()
{
	preferencesUi.comboBox->clear();
	QSize size = preferencesUi.comboBox->iconSize();
	unsigned short index = 0;
	BOOST_FOREACH( color::Color::ColormapMapType::const_reference lut, m_ViewerCore->getColorHandler()->getColormapMap() ) {
		if( lut.first != std::string( "fallback" ) ) {
			preferencesUi.comboBox->insertItem( index++, m_ViewerCore->getColorHandler()->getIcon( lut.first, size.width() , size.height() ), QString( lut.first.c_str() ) ) ;
		}
	}
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	preferencesUi.checkShowDesc->setChecked( m_ViewerCore->getSettings()->value( "showDesc", false ).toBool() );
	preferencesUi.comboInterpolation->setCurrentIndex( m_ViewerCore->getSettings()->value( "interpolationType", 0 ).toUInt() );
	preferencesUi.comboScaling->setCurrentIndex( m_ViewerCore->getSettings()->value( "scaling", 0 ).toUInt() );
	preferencesUi.checkShowLabels->setChecked( m_ViewerCore->getSettings()->value( "showLabels", false ).toBool() );
	preferencesUi.checkPropagateZooming->setChecked( m_ViewerCore->getSettings()->value( "propagateZooming", false ).toBool() );
	preferencesUi.comboBox->setCurrentIndex( preferencesUi.comboBox->findText( m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<std::string>( "lut" ).c_str() ) );
	m_ViewerCore->getSettings()->endGroup();
}

void PreferencesDialog::saveSettings()
{
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	m_ViewerCore->getSettings()->setValue( "showDesc", preferencesUi.checkShowDesc->isChecked() );
	m_ViewerCore->getSettings()->setValue( "interpolationType", preferencesUi.comboInterpolation->currentIndex() );
	m_ViewerCore->getSettings()->setValue( "lut", preferencesUi.comboBox->currentText() );
	m_ViewerCore->getSettings()->setValue( "scaling", preferencesUi.comboScaling->currentIndex() );
	m_ViewerCore->getSettings()->setValue( "showLabels", preferencesUi.checkShowLabels->isChecked() );
	m_ViewerCore->getSettings()->setValue( "propagateZooming", preferencesUi.checkPropagateZooming->isChecked() );
	m_ViewerCore->getSettings()->endGroup();
	m_ViewerCore->getSettings()->sync();
	m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<std::string>( "lut", preferencesUi.comboBox->currentText().toStdString() ) ;
}


}
}
}