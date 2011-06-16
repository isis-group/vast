#include "QPreferencesDialog.hpp"

namespace isis {
namespace viewer {
	
QPreferencesDialog::QPreferencesDialog(QWidget* parent): QDialog(parent)
{
	preferencesUi.setupUi(this);
}

	
}}