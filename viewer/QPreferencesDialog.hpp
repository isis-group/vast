#ifndef QPREFERNCESDIALOG_HPP
#define QPREFERNCESDIALOG_HPP

#include "ui_preferences.h"

namespace isis {
namespace viewer {
	
class QPreferencesDialog : public QDialog
{
	Q_OBJECT;
	
public:
	QPreferencesDialog(QWidget* parent = 0 );
protected:
	Ui::preferencesDialog preferencesUi;
};

	
}
}



#endif