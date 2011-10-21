#ifndef ORIENTATIONCORRECTION_HPP
#define ORIENTATIONCORRECTION_HPP

#include <QDialog>
#include "QViewerCore.hpp"
#include "ui_orientationcorrection.h"

namespace isis {
namespace viewer {
namespace plugin {

class OrientatioCorrectionDialog : public QDialog
{
	Q_OBJECT
public:
	OrientatioCorrectionDialog( QWidget *parent, QViewerCore *core );
public Q_SLOTS:
	virtual void applyPressed();
	virtual void flipPressed();
	virtual void rotatePressed();
private:
	QViewerCore *m_Core;
	Ui::OrientationCorrection ui;
	boost::numeric::ublas::matrix<QTableWidgetItem*> m_MatrixItems;
	bool applyTransform ( const boost::numeric::ublas::matrix<float> &trans, bool center ) const ;
	
};



}}}

#endif