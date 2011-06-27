#include "QPreferencesDialog.hpp"

namespace isis {
namespace viewer {
	
QPreferencesDialog::QPreferencesDialog(QWidget* parent, QViewerCore* core): 
	QDialog(parent),
	m_ViewerCore( core )
{
	preferencesUi.setupUi(this);
	loadSettings();
	
}


void QPreferencesDialog::closeEvent(QCloseEvent* )
{
	saveSettings();
}

void QPreferencesDialog::loadSettings()
{
	m_ViewerCore->getPreferences()->getQSettings()->beginGroup("UserProfile");
	preferencesUi.checkShowDesc->setChecked( m_ViewerCore->getPreferences()->getQSettings()->value("showDesc", false).toBool());
	preferencesUi.comboInterpolation->setCurrentIndex( m_ViewerCore->getPreferences()->getQSettings()->value("interpolation", 0).toUInt());
	preferencesUi.comboScaling->setCurrentIndex( m_ViewerCore->getPreferences()->getQSettings()->value("scaling", 0).toUInt());
	preferencesUi.checkShowLabels->setChecked( m_ViewerCore->getPreferences()->getQSettings()->value("labels", false).toBool());
	m_ViewerCore->getPreferences()->getQSettings()->endGroup();
}

void QPreferencesDialog::saveSettings()
{
	m_ViewerCore->getPreferences()->getQSettings()->beginGroup("UserProfile");
	m_ViewerCore->getPreferences()->getQSettings()->setValue("showDesc", preferencesUi.checkShowDesc->isChecked());
	m_ViewerCore->getPreferences()->getQSettings()->setValue("interpolation", preferencesUi.comboInterpolation->currentIndex());
	m_ViewerCore->getPreferences()->getQSettings()->setValue("scaling", preferencesUi.comboInterpolation->currentIndex());
	m_ViewerCore->getPreferences()->getQSettings()->setValue("labels", preferencesUi.checkShowLabels->isChecked());
	m_ViewerCore->getPreferences()->getQSettings()->endGroup();
	m_ViewerCore->getPreferences()->getQSettings()->sync();
}


	
}}