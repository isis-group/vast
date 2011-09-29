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
	loadSettings();

}


void QPreferencesDialog::closeEvent( QCloseEvent * )
{
	saveSettings();
}

void QPreferencesDialog::loadSettings()
{
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	preferencesUi.checkShowDesc->setChecked( m_ViewerCore->getSettings()->value( "showDesc", false ).toBool() );
	preferencesUi.comboInterpolation->setCurrentIndex( m_ViewerCore->getSettings()->value( "interpolation", 0 ).toUInt() );
	preferencesUi.comboScaling->setCurrentIndex( m_ViewerCore->getSettings()->value( "scaling", 0 ).toUInt() );
	preferencesUi.checkShowLabels->setChecked( m_ViewerCore->getSettings()->value( "labels", false ).toBool() );
	preferencesUi.checkPropagateZooming->setChecked( m_ViewerCore->getSettings()->value( "propagateZooming", false ).toBool() );
	m_ViewerCore->getSettings()->endGroup();
}

void QPreferencesDialog::saveSettings()
{
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	m_ViewerCore->getSettings()->setValue( "showDesc", preferencesUi.checkShowDesc->isChecked() );
	m_ViewerCore->getSettings()->setValue( "interpolation", preferencesUi.comboInterpolation->currentIndex() );
	m_ViewerCore->getSettings()->setValue( "scaling", preferencesUi.comboScaling->currentIndex() );
	m_ViewerCore->getSettings()->setValue( "labels", preferencesUi.checkShowLabels->isChecked() );
	m_ViewerCore->getSettings()->setValue( "propagateZooming", preferencesUi.checkPropagateZooming->isChecked() );
	m_ViewerCore->getSettings()->endGroup();
	m_ViewerCore->getSettings()->sync();
}



}
}