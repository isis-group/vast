/****************************************************************
 *
 * <Copyright information>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Author: Erik Türke, tuerke@cbs.mpg.de
 *
 * OrientationCorrection.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
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
	  m_ViewerCore( core ),
	  m_MatrixItems( 3, 3 ),
	  askedGeometricalView_(false)
{
	ui.setupUi( this );
#if QT_VERSION >= 0x040500
	ui.tableWidget->setInputMethodHints( Qt::ImhFormattedNumbersOnly );
#endif

	for( unsigned short i = 0; i < 3; i++ ) {
		for( unsigned short j = 0; j < 3; j++ ) {
			m_MatrixItems( i, j ) = i != j ? new QTableWidgetItem( QString( "0" ) ) : new QTableWidgetItem( QString( "1" )  ) ;
			ui.tableWidget->setItem( i, j, m_MatrixItems( i, j ) );
		}
	}

	connectAll();
	connect( ui.pushButton, SIGNAL( pressed() ), this, SLOT( applyPressed() ) );
	connect( ui.resetButton, SIGNAL( clicked( bool ) ), this, SLOT( resetPressed() ) );

}

void OrientatioCorrectionDialog::connectAll()
{
	connect( ui.checkFlipX, SIGNAL( clicked( bool ) ), this, SLOT( applyTransform() ) );
	connect( ui.checkFlipY, SIGNAL( clicked( bool ) ), this, SLOT( applyTransform() ) );
	connect( ui.checkFlipZ, SIGNAL( clicked( bool ) ), this, SLOT( applyTransform() ) );
	connect( ui.rotateX, SIGNAL( valueChanged( double ) ), this, SLOT( applyTransform() ) );
	connect( ui.rotateY, SIGNAL( valueChanged( double ) ), this, SLOT( applyTransform() ) );
	connect( ui.rotateZ, SIGNAL( valueChanged( double ) ), this, SLOT( applyTransform() ) );
	connect( ui.translateX, SIGNAL( valueChanged( double ) ), this, SLOT( applyTransform() ) );
	connect( ui.translateY, SIGNAL( valueChanged( double ) ), this, SLOT( applyTransform() ) );
	connect( ui.translateZ, SIGNAL( valueChanged( double ) ), this, SLOT( applyTransform() ) );
	connect( ui.imageBox, SIGNAL( currentIndexChanged( QString ) ), this, SLOT( imageChanged( QString ) ) );
}

void OrientatioCorrectionDialog::disconnectAll()
{
	disconnect( ui.checkFlipX, SIGNAL( clicked( bool ) ), this, SLOT( applyTransform() ) );
	disconnect( ui.checkFlipY, SIGNAL( clicked( bool ) ), this, SLOT( applyTransform() ) );
	disconnect( ui.checkFlipZ, SIGNAL( clicked( bool ) ), this, SLOT( applyTransform() ) );
	disconnect( ui.rotateX, SIGNAL( valueChanged( double ) ), this, SLOT( applyTransform() ) );
	disconnect( ui.rotateY, SIGNAL( valueChanged( double ) ), this, SLOT( applyTransform() ) );
	disconnect( ui.rotateZ, SIGNAL( valueChanged( double ) ), this, SLOT( applyTransform() ) );
	disconnect( ui.translateX, SIGNAL( valueChanged( double ) ), this, SLOT( applyTransform() ) );
	disconnect( ui.translateY, SIGNAL( valueChanged( double ) ), this, SLOT( applyTransform() ) );
	disconnect( ui.translateZ, SIGNAL( valueChanged( double ) ), this, SLOT( applyTransform() ) );
	disconnect( ui.imageBox, SIGNAL( currentIndexChanged( QString ) ), this, SLOT( imageChanged( QString ) ) );
}


void OrientatioCorrectionDialog::showEvent ( QShowEvent *e )
{
	if( m_ViewerCore->hasImage() ) {
		disconnectAll();
		ui.imageBox->clear();
		BOOST_FOREACH( const ImageHolder::Vector::const_reference image, m_ViewerCore->getImageVector() ) {
			ui.imageBox->addItem( image->getImageProperties().filePath.c_str() );
		}
		ui.imageBox->setCurrentIndex( ui.imageBox->findText( m_ViewerCore->getCurrentImage()->getImageProperties().filePath.c_str() ) );
		updateValues( m_ViewerCore->getCurrentImage() );
	}

	connectAll();
	//  m_ViewerCore->emitCurrentImageChanged.connect( boost::bind( &OrientatioCorrectionDialog::updateValues, this, _1 ) );

	QDialog::showEvent( e );
}

void OrientatioCorrectionDialog::closeEvent ( QCloseEvent *e )
{

	QDialog::closeEvent( e );
}

void OrientatioCorrectionDialog::imageChanged ( QString path )
{
	updateValues( m_ViewerCore->getImageMap().at( path.toStdString() ) );
}


void OrientatioCorrectionDialog::updateValues( ImageHolder::Pointer image )
{
	setValuesToZero();

	if( !image->getPropMap().hasProperty( "OrientationCorrection/origRowVec" ) ) {
		image->getPropMap().setPropertyAs<util::fvector3>( "OrientationCorrection/origRowVec",
				image->getISISImage()->getPropertyAs<util::fvector3>( "rowVec" ) );
		image->getPropMap().setPropertyAs<util::fvector3>( "OrientationCorrection/origColumnVec",
				image->getISISImage()->getPropertyAs<util::fvector3>( "columnVec" ) );
		image->getPropMap().setPropertyAs<util::fvector3>( "OrientationCorrection/origSliceVec",
				image->getISISImage()->getPropertyAs<util::fvector3>( "sliceVec" ) );
		image->getPropMap().setPropertyAs<util::fvector3>( "OrientationCorrection/origIndexOrigin",
				image->getISISImage()->getPropertyAs<util::fvector3>( "indexOrigin" ) );
	}

	if( image->getPropMap().hasProperty( "OrientationCorrection/translateX" ) ) {
		ui.translateX->setValue( image->getPropMap().getPropertyAs<float>( "OrientationCorrection/translateX" ) );
	}

	if( image->getPropMap().hasProperty( "OrientationCorrection/translateY" ) ) {
		ui.translateY->setValue( image->getPropMap().getPropertyAs<float>( "OrientationCorrection/translateY" ) );
	}

	if( image->getPropMap().hasProperty( "OrientationCorrection/translateZ" ) ) {
		ui.translateZ->setValue( image->getPropMap().getPropertyAs<float>( "OrientationCorrection/translateZ" ) );
	}

	if( image->getPropMap().hasProperty( "OrientationCorrection/rotateX" ) ) {
		ui.rotateX->setValue( image->getPropMap().getPropertyAs<float>( "OrientationCorrection/rotateX" ) );
	}

	if( image->getPropMap().hasProperty( "OrientationCorrection/rotateY" ) ) {
		ui.rotateY->setValue( image->getPropMap().getPropertyAs<float>( "OrientationCorrection/rotateY" ) );
	}

	if( image->getPropMap().hasProperty( "OrientationCorrection/rotateZ" ) ) {
		ui.rotateZ->setValue( image->getPropMap().getPropertyAs<float>( "OrientationCorrection/rotateZ" ) );
	}

	if( image->getPropMap().hasProperty( "OrientationCorrection/flippedX" ) ) {
		ui.checkFlipX->setChecked( image->getPropMap().getPropertyAs<bool>( "OrientationCorrection/flippedX" ) );
	}

	if( image->getPropMap().hasProperty( "OrientationCorrection/flippedY" ) ) {
		ui.checkFlipY->setChecked( image->getPropMap().getPropertyAs<bool>( "OrientationCorrection/flippedY" ) );
	}

	if( image->getPropMap().hasProperty( "OrientationCorrection/flippedZ" ) ) {
		ui.checkFlipZ->setChecked( image->getPropMap().getPropertyAs<bool>( "OrientationCorrection/flippedZ" ) );
	}
}

void OrientatioCorrectionDialog::applyPressed()
{
	if( m_ViewerCore->hasImage() ) {
		ImageHolder::Pointer image = m_ViewerCore->getImageMap().at( ui.imageBox->currentText().toStdString() );
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
		bool ret = image->getISISImage()->transformCoords( transform, false );

		if( ret ) {
			image->getPropMap().setPropertyAs<util::fvector3>( "originalRowVec", image->getISISImage()->getPropertyAs<util::fvector3>( "rowVec" ) );
			image->getPropMap().setPropertyAs<util::fvector3>( "originalColumnVec", image->getISISImage()->getPropertyAs<util::fvector3>( "columnVec" ) );
			image->getPropMap().setPropertyAs<util::fvector3>( "originalSliceVec", image->getISISImage()->getPropertyAs<util::fvector3>( "sliceVec" ) );
			image->addChangedAttribute( desc.str() );
		} else {
			LOG( Runtime, error ) << "Could not apply transform " << transform << " to the image " << image->getImageProperties().fileName << " !";
		}

		image->updateOrientation();
		m_ViewerCore->updateScene();
	}
}

void OrientatioCorrectionDialog::resetPressed()
{
	setValuesToZero();
	applyTransform();

}

void OrientatioCorrectionDialog::setValuesToZero()
{
	disconnectAll();
	ui.checkFlipX->setChecked( false );
	ui.checkFlipY->setChecked( false );
	ui.checkFlipZ->setChecked( false );
	ui.translateX->setValue( 0 );
	ui.translateY->setValue( 0 );
	ui.translateZ->setValue( 0 );
	ui.rotateX->setValue( 0 );
	ui.rotateY->setValue( 0 );
	ui.rotateZ->setValue( 0 );
	connectAll();
}


bool OrientatioCorrectionDialog::applyTransform( ) {
	if( m_ViewerCore->hasImage() ) {
		if( !m_ViewerCore->getUICore()->getMainWindow()->getInterface().actionGeometrical_View->isChecked() && !askedGeometricalView_) {
			askedGeometricalView_ = true;
			QMessageBox msgBox;
			msgBox.setIcon(QMessageBox::Warning);
			msgBox.setWindowTitle("\"Geometrical View\" is disabled!");
			msgBox.setText("To validate your changes on the orientation you have to turn on \"Geometrical View\".");
			msgBox.setInformativeText("Do you wish to enable it?");
			msgBox.setStandardButtons( QMessageBox::No | QMessageBox::Yes );
			msgBox.setDefaultButton(QMessageBox::Yes);
			const int answer = msgBox.exec();
			if( answer == QMessageBox::Yes ) {
				m_ViewerCore->getUICore()->getMainWindow()->toggleGeometrical(true);
			}
		}
		ImageHolder::Pointer image = m_ViewerCore->getImageMap().at( ui.imageBox->currentText().toStdString() );
		image->getISISImage()->setPropertyAs<util::fvector3>( "rowVec", image->getPropMap().getPropertyAs<util::fvector3>( "OrientationCorrection/origRowVec" ) );
		image->getISISImage()->setPropertyAs<util::fvector3>( "columnVec", image->getPropMap().getPropertyAs<util::fvector3>( "OrientationCorrection/origColumnVec" ) );
		image->getISISImage()->setPropertyAs<util::fvector3>( "sliceVec", image->getPropMap().getPropertyAs<util::fvector3>( "OrientationCorrection/origSliceVec" ) );
		image->getISISImage()->setPropertyAs<util::fvector3>( "indexOrigin", image->getPropMap().getPropertyAs<util::fvector3>( "OrientationCorrection/origIndexOrigin" ) );
		boost::numeric::ublas::matrix<float> ftransform = boost::numeric::ublas::identity_matrix<float>( 3, 3 );
		std::stringstream desc;

		//matrix from flip
		if( ui.checkFlipZ->isChecked() ) {
			ftransform( 2, 2 ) *= -1;
			image->getPropMap().setPropertyAs<float>( "OrientationCorrection/flippedZ", ui.checkFlipZ->isChecked() );
		}

		if( ui.checkFlipY->isChecked() ) {
			ftransform( 1, 1 ) *= -1;
			image->getPropMap().setPropertyAs<float>( "OrientationCorrection/flippedY", ui.checkFlipY->isChecked() );
		}

		if( ui.checkFlipX->isChecked() ) {
			ftransform( 0, 0 ) *= -1;
			image->getPropMap().setPropertyAs<float>( "OrientationCorrection/flippedX", ui.checkFlipX->isChecked() );
		}

		//matrix from rotation
		boost::numeric::ublas::matrix<float> transform = boost::numeric::ublas::zero_matrix<float>( 3, 3 );
		double normX = ( ui.rotateX->value() / 180 ) * M_PI;
		double normY = ( ui.rotateY->value() / 180 ) * M_PI;
		double normZ = ( ui.rotateZ->value() / 180 ) * M_PI;
		transform( 0, 0 ) = cos( normY ) * cos( normZ );
		transform( 1, 0 ) = -cos( normX ) * sin( normZ ) + sin( normX ) * sin( normY ) * cos( normZ );
		transform( 2, 0 ) = sin( normX ) * sin( normZ ) + cos( normX ) * sin( normY ) * cos( normZ );
		transform( 0, 1 ) = cos( normY ) * sin( normZ );
		transform( 1, 1 ) = cos( normX ) * cos( normZ ) + sin( normX ) * sin( normY ) * sin( normZ );
		transform( 2, 1 ) = -sin( normX ) * cos( normZ ) + cos( normX ) * sin( normY ) * sin( normZ );
		transform( 0, 2 ) = -sin( normY );
		transform( 1, 2 ) = sin( normX ) * cos( normY );
		transform( 2, 2 ) = cos( normX ) * cos( normY );

		if( ui.rotateX->value() != 0 ) {
			desc << "X Rotation: " << ui.rotateX->value() << std::endl;
			image->getPropMap().setPropertyAs<float>( "OrientationCorrection/rotateX", ui.rotateX->value() );
		}

		if( ui.rotateY->value() != 0 ) {
			desc << "Y Rotation: " << ui.rotateY->value() << std::endl;
			image->getPropMap().setPropertyAs<float>( "OrientationCorrection/rotateY", ui.rotateY->value() );
		}

		if( ui.rotateZ->value() != 0 ) {
			desc << "Z Rotation: " << ui.rotateZ->value() << std::endl;
			image->getPropMap().setPropertyAs<float>( "OrientationCorrection/rotateZ", ui.rotateZ->value() );
		}

		boost::numeric::ublas::matrix<float> finalMatrix = boost::numeric::ublas::prod( ftransform, transform );


		//translation
		const util::fvector3 translation( ui.translateX->text().toFloat(), ui.translateY->text().toFloat(), ui.translateZ->text().toFloat() );

		if( translation[0] != 0 ) {
			desc << "X Translation: " << translation[0] << std::endl;
			image->getPropMap().setPropertyAs<float>( "OrientationCorrection/translateX", translation[0] );
		}

		if( translation[1] != 0 ) {
			desc << "Y Translation: " << translation[1] << std::endl;
			image->getPropMap().setPropertyAs<float>( "OrientationCorrection/translateY", translation[1] );
		}

		if( translation[2] != 0 ) {
			desc << "Z Translation: " << translation[2] << std::endl;
			image->getPropMap().setPropertyAs<float>( "OrientationCorrection/translateZ", translation[2] );
		}

		const util::fvector3 oldIO = image->getISISImage()->getPropertyAs<util::fvector3>( "indexOrigin" );

		image->getISISImage()->setPropertyAs<util::fvector3>( "indexOrigin", oldIO + translation );


		bool ret = image->getISISImage()->transformCoords( finalMatrix, false );

		if( ret ) {
			image->getPropMap().setPropertyAs<util::fvector3>( "originalRowVec", image->getISISImage()->getPropertyAs<util::fvector3>( "rowVec" ) );
			image->getPropMap().setPropertyAs<util::fvector3>( "originalColumnVec", image->getISISImage()->getPropertyAs<util::fvector3>( "columnVec" ) );
			image->getPropMap().setPropertyAs<util::fvector3>( "originalSliceVec", image->getISISImage()->getPropertyAs<util::fvector3>( "sliceVec" ) );
			image->addChangedAttribute( desc.str() );
		} else {
			LOG( Runtime, error ) << "Could not apply transform " << finalMatrix << " to the image " << image->getImageProperties().fileName << " !";
		}

		image->updateOrientation();
		m_ViewerCore->emitImageContentChanged( image );
		m_ViewerCore->physicalCoordsChanged( image->getISISImage()->getPhysicalCoordsFromIndex( image->getImageProperties().voxelCoords ) );
		return ret;
	}

	return false;

}



}
}
}