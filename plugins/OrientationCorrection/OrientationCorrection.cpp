#include "OrientationCorrection.hpp"
#include "boost/numeric/ublas/matrix.hpp"
#include "boost/numeric/ublas/io.hpp"
#include <math.h>
#define _USE_MATH_DEFINES

namespace isis {
namespace viewer {
namespace plugin {

OrientatioCorrectionDialog::OrientatioCorrectionDialog(QWidget* parent, QViewerCore* core)
	: QDialog(parent),
	m_Core(core),
	m_MatrixItems(3,3)
{
	ui.setupUi(this);
	ui.tableWidget->setInputMethodHints( Qt::ImhFormattedNumbersOnly );
	for( unsigned short i = 0; i< 3; i++) {
		for( unsigned short j = 0; j< 3; j++) {
			m_MatrixItems(i,j) = i != j ? new QTableWidgetItem( QString("0") ) : new QTableWidgetItem( QString("1")  ) ;
			ui.tableWidget->setItem(i,j, m_MatrixItems(i,j) );
		}
	}
	connect( ui.pushButton, SIGNAL( pressed()), this, SLOT( applyPressed()));
	connect( ui.flipButton, SIGNAL( pressed()), this, SLOT( flipPressed()));
	connect( ui.rotateButton, SIGNAL( pressed()), this, SLOT( rotatePressed()) );
}

void OrientatioCorrectionDialog::flipPressed()
{
	boost::numeric::ublas::matrix<float> transform = boost::numeric::ublas::identity_matrix<float>(3,3);
	transform(2,2) = ui.checkFlipAxial->isChecked() ? -1 : 1;
	transform(0,0) = ui.checkFlipSagittal->isChecked() ? -1 : 1;
	transform(1,1) = ui.checkFlipCoronal->isChecked() ? -1 : 1;
	applyTransform( transform, ui.checkISO->isChecked() );
}
void OrientatioCorrectionDialog::applyPressed()
{
	boost::numeric::ublas::matrix<float> transform = boost::numeric::ublas::matrix<float>(3,3);
	for( unsigned short i = 0; i< 3; i++) {
		for( unsigned short j = 0; j< 3; j++) {
			transform(i,j) = m_MatrixItems(i,j)->text().toFloat();
		}
	}	
	applyTransform( transform, ui.checkISO->isChecked() );
}

void OrientatioCorrectionDialog::rotatePressed()
{
	util::DefaultMsgPrint::stopBelow(warning);
	boost::numeric::ublas::matrix<float> transformX = boost::numeric::ublas::zero_matrix<float>(3,3);
	boost::numeric::ublas::matrix<float> transformY = boost::numeric::ublas::zero_matrix<float>(3,3);
	boost::numeric::ublas::matrix<float> transformZ = boost::numeric::ublas::zero_matrix<float>(3,3);
	double normX = (ui.rotateX->text().toDouble() / 180) * M_PI;
	double normY = (ui.rotateY->text().toDouble() / 180) * M_PI;
	double normZ = (ui.rotateZ->text().toDouble() / 180) * M_PI;
	transformX(0,0) = 1;
	transformX(1,1) = cos( normX );
	transformX(2,2) = cos( normX );
	transformX(2,1) = -sin( normX );
	transformX(1,2) = sin( normX );
	std::cout << transformX << std::endl;
	applyTransform( transformX, ui.checkISO->isChecked() );
}
bool OrientatioCorrectionDialog::applyTransform(const boost::numeric::ublas::matrix< float >& trans, bool center) const
{
	bool ret = m_Core->getCurrentImage()->getISISImage()->transformCoords(trans, center );
	m_Core->updateScene();
	return ret;

}



}}}