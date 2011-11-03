#include "OrientationCorrection.hpp"
#include "boost/numeric/ublas/matrix.hpp"
#include "boost/numeric/ublas/io.hpp"
#include <math.h>
#define _USE_MATH_DEFINES

namespace isis
{
namespace viewer
{
namespace plugin
{

OrientatioCorrectionDialog::OrientatioCorrectionDialog( QWidget *parent, QViewerCore *core )
	: QDialog( parent ),
	  m_Core( core ),
	  m_MatrixItems( 3, 3 )
{
	ui.setupUi( this );
	ui.tableWidget->setInputMethodHints( Qt::ImhFormattedNumbersOnly );

	for( unsigned short i = 0; i < 3; i++ ) {
		for( unsigned short j = 0; j < 3; j++ ) {
			m_MatrixItems( i, j ) = i != j ? new QTableWidgetItem( QString( "0" ) ) : new QTableWidgetItem( QString( "1" )  ) ;
			ui.tableWidget->setItem( i, j, m_MatrixItems( i, j ) );
		}
	}

	connect( ui.pushButton, SIGNAL( pressed() ), this, SLOT( applyPressed() ) );
	connect( ui.flipButton, SIGNAL( pressed() ), this, SLOT( flipPressed() ) );
	connect( ui.rotateButton, SIGNAL( pressed() ), this, SLOT( rotatePressed() ) );
}

void OrientatioCorrectionDialog::flipPressed()
{
	boost::numeric::ublas::matrix<float> transform = boost::numeric::ublas::identity_matrix<float>( 3, 3 );

	if( ui.checkFlipZ->isChecked() ) {
		transform( 2, 2 ) = -1;
		m_Core->getCurrentImage()->addChangedAttribute( "Flip Z" );
	}

	if( ui.checkFlipY->isChecked() ) {
		transform( 1, 1 ) = -1;
		m_Core->getCurrentImage()->addChangedAttribute( "Flip Y" );
	}

	if( ui.checkFlipX->isChecked() ) {
		transform( 0, 0 ) = -1;
		m_Core->getCurrentImage()->addChangedAttribute( "Flip X" );
	}

	applyTransform( transform, ui.checkISO->isChecked() );
}
void OrientatioCorrectionDialog::applyPressed()
{
	boost::numeric::ublas::matrix<float> transform = boost::numeric::ublas::matrix<float>( 3, 3 );

	for( unsigned short i = 0; i < 3; i++ ) {
		for( unsigned short j = 0; j < 3; j++ ) {
			transform( i, j ) = m_MatrixItems( i, j )->text().toFloat();
		}
	}

	std::stringstream desc;
	desc << "Transformation matrix: " << std::endl << transform( 0, 0 ) << " " << transform( 1, 0 ) << " " << transform( 2, 0 ) << std::endl <<
		 transform( 0, 1 ) << " " << transform( 1, 1 ) << " " << transform( 2, 1 ) << std::endl <<
		 transform( 0, 2 ) << " " << transform( 1, 2 ) << " " << transform( 2, 2 ) << std::endl;
	m_Core->getCurrentImage()->addChangedAttribute( desc.str() );
	applyTransform( transform, ui.checkISO->isChecked() );
}

void OrientatioCorrectionDialog::rotatePressed()
{
	util::DefaultMsgPrint::stopBelow( warning );
	boost::numeric::ublas::matrix<float> transform = boost::numeric::ublas::zero_matrix<float>( 3, 3 );
	double normX = ( ui.rotateX->text().toDouble() / 180 ) * M_PI;
	double normY = ( ui.rotateY->text().toDouble() / 180 ) * M_PI;
	double normZ = ( ui.rotateZ->text().toDouble() / 180 ) * M_PI;
	transform( 0, 0 ) = cos( normY ) * cos( normZ );
	transform( 1, 0 ) = -cos( normX ) * sin( normZ ) + sin( normX ) * sin( normY ) * cos( normZ );
	transform( 2, 0 ) = sin( normX ) * sin( normZ ) + cos( normX ) * sin( normY ) * cos( normZ );
	transform( 0, 1 ) = cos( normY ) * sin( normZ );
	transform( 1, 1 ) = cos( normX ) * cos( normZ ) + sin( normX ) * sin( normY ) * sin( normZ );
	transform( 2, 1 ) = -sin( normX ) * cos( normZ ) + cos( normX ) * sin( normY ) * sin( normZ );
	transform( 0, 2 ) = -sin( normY );
	transform( 1, 2 ) = sin( normX ) * cos( normY );
	transform( 2, 2 ) = cos( normX ) * cos( normY );

	std::stringstream desc;

	if( ui.rotateX->text().toDouble() != 0 ) {
		desc << "X Rotation: " << ui.rotateX->text().toDouble() << std::endl;
	}

	if( ui.rotateY->text().toDouble() != 0 ) {
		desc << "Y Rotation: " << ui.rotateY->text().toDouble() << std::endl;
	}

	if( ui.rotateZ->text().toDouble() != 0 ) {
		desc << "Z Rotation: " << ui.rotateZ->text().toDouble() << std::endl;
	}

	m_Core->getCurrentImage()->addChangedAttribute( desc.str() );
	applyTransform( transform, ui.checkISO->isChecked() );

}
bool OrientatioCorrectionDialog::applyTransform( const boost::numeric::ublas::matrix< float >& trans, bool center ) const
{
	bool ret = m_Core->getCurrentImage()->getISISImage()->transformCoords( trans, center );
	m_Core->updateScene();
	return ret;

}



}
}
}