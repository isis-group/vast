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
 * Author: Erik Tuerke, tuerke@cbs.mpg.de
 *
 * RegistrationDialog.cpp
 *
 * Description:
 *
 *  Created on: Apr 17, 2012
 *      Author: tuerke
 ******************************************************************/
#include "RegistrationDialog.hpp"


#include "itkAdapter.hpp"
#include "itkCastImageFilter.h"
#include "itkEllipseSpatialObject.h"
#include "itkImage.h"
#include "itkImageRegistrationMethod.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSpatialObjectToImageFilter.h"
#include "itkVersorRigid3DTransform.h"

namespace isis
{
namespace viewer
{
namespace plugin
{

RegistrationDialog::RegistrationDialog ( QWidget *parent, QViewerCore *core )
	: QDialog ( parent ),
	  m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	connect( m_Interface.goButton, SIGNAL( pressed() ), this, SLOT( startRegistration() ) );

}

void RegistrationDialog::showEvent ( QShowEvent * )
{
	m_Interface.fixedImageCombo->clear();
	m_Interface.movingImageCombo->clear();
	BOOST_FOREACH( const ImageHolder::Vector::const_reference image, m_ViewerCore->getImageVector() ) {
		m_Interface.fixedImageCombo->addItem( image->getImageProperties().filePath.c_str() );
		m_Interface.movingImageCombo->addItem( image->getImageProperties().filePath.c_str() );
	}
}

void RegistrationDialog::startRegistration()
{
	if( m_Interface.fixedImageCombo->currentText() == m_Interface.movingImageCombo->currentText() ) {
		QMessageBox msgBox;
		msgBox.setIcon( QMessageBox::Question );
		msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
		msgBox.setText( "The fixed and moving images are the same. Do you really want to proceed?" );
	}

	typedef itk::Image< uint8_t, 3>  ImageType;
	typedef itk::VersorRigid3DTransform< double > TransformType;
	typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;
	typedef itk::MattesMutualInformationImageToImageMetric<ImageType, ImageType> MetricType;
	typedef itk::LinearInterpolateImageFunction< ImageType, double >    InterpolatorType;
	typedef itk::ImageRegistrationMethod< ImageType, ImageType > RegistrationType;
	typedef RegistrationType::ParametersType ParametersType;

	isis::adapter::itkAdapter *fixedAdapter = new isis::adapter::itkAdapter;
	isis::adapter::itkAdapter *movingAdapter = new isis::adapter::itkAdapter;

	MetricType::Pointer         metric        = MetricType::New();
	TransformType::Pointer      transform     = TransformType::New();
	OptimizerType::Pointer      optimizer     = OptimizerType::New();
	InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
	RegistrationType::Pointer   registration  = RegistrationType::New();

	registration->SetMetric(        metric        );
	registration->SetOptimizer(     optimizer     );
	registration->SetTransform(     transform     );
	registration->SetInterpolator(  interpolator  );

	ImageType::Pointer fixedImage = fixedAdapter->makeItkImageObject<ImageType>( *m_ViewerCore->getImageMap().at( m_Interface.fixedImageCombo->currentText().toStdString() )->getISISImage().get() ) ;
	ImageType::Pointer movingImage = movingAdapter->makeItkImageObject<ImageType>( *m_ViewerCore->getImageMap().at( m_Interface.movingImageCombo->currentText().toStdString() )->getISISImage().get() ) ;

	registration->SetFixedImage( fixedImage );
	registration->SetMovingImage( movingImage );

	registration->SetFixedImageRegion( fixedImage->GetLargestPossibleRegion() );

	ParametersType initialParameters( transform->GetNumberOfParameters() );

	initialParameters.Fill( 0.0 );

	registration->SetInitialTransformParameters( initialParameters );

	optimizer->SetMaximumStepLength( 0.1 );
	optimizer->SetMinimumStepLength( 0.0001 );
	optimizer->SetRelaxationFactor( 0.9 );
	optimizer->SetGradientMagnitudeTolerance( 0.00001 );
	optimizer->SetMinimize( true );
	optimizer->SetNumberOfIterations( 200 );
	OptimizerType::ScalesType scale( transform->GetNumberOfParameters() );
	scale[0] = 1.0;
	scale[0] = 1.0;
	scale[0] = 1.0;
	scale[0] = 1.0 / 100.0;
	scale[0] = 1.0 / 100.0;
	scale[0] = 1.0 / 100.0;
	optimizer->SetScales( scale );

	metric->SetNumberOfFixedImageSamples( fixedImage->GetLargestPossibleRegion().GetNumberOfPixels() * 0.01 );
	metric->SetNumberOfHistogramBins( 50 );

	registration->Update();

	typedef itk::ResampleImageFilter <
	ImageType,
	ImageType >    ResampleFilterType;
	ResampleFilterType::Pointer resampler = ResampleFilterType::New();
	resampler->SetInput( movingImage );
	resampler->SetTransform( registration->GetOutput()->Get() );
	resampler->SetSize( fixedImage->GetLargestPossibleRegion().GetSize() );
	resampler->SetOutputOrigin(  fixedImage->GetOrigin() );
	resampler->SetOutputSpacing( fixedImage->GetSpacing() );
	resampler->SetOutputDirection( fixedImage->GetDirection() );
	resampler->SetDefaultPixelValue( 0 );
	resampler->Update();
	data::Image outImage = fixedAdapter->makeIsisImageObject<ImageType>( resampler->GetOutput() ).front();
	m_ViewerCore->getUICore()->createViewWidgetEnsemble( m_ViewerCore->getSettings()->getPropertyAs<std::string>( "defaultViewWidgetIdentifier" ), m_ViewerCore->addImage( outImage, ImageHolder::structural_image ) );
	m_ViewerCore->getUICore()->refreshUI();
}


}
}
}