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
 * ImageComponents.cpp
 *
 * Description:
 *
 *  Created on: Mar 05, 2012
 ******************************************************************/

#include "ImageComponents.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{

VTKImageComponents::VTKImageComponents( VTKImageComponents::VTKMapperType mapper )
	: volume( vtkVolume::New() ),
	  property( vtkVolumeProperty::New() ),
	  colorFunction( vtkColorTransferFunction::New() ),
	  opacityFunction( vtkPiecewiseFunction::New() ),
	  imageData( vtkImageData::New() ),
	  currentCropping( new double[6] ),
	  croppingSet( false ),
	  currentMapperType( mapper )
{
	switch( currentMapperType ) {
	case Texture:
		textureMapper = vtkVolumeTextureMapper3D::New();
		currentVolumeMapper = textureMapper;
		volume->SetMapper( textureMapper );
		break;
	case GPU_VolumeRayCast:
		gpuVolumeRayMapper = vtkGPUVolumeRayCastMapper::New();
		currentVolumeMapper = gpuVolumeRayMapper;
		volume->SetMapper( gpuVolumeRayMapper );
		break;
	case CPU_FixedPointRayCast:
		fixedRayMapper = vtkFixedPointVolumeRayCastMapper::New();
		currentVolumeMapper = fixedRayMapper;
		volume->SetMapper( fixedRayMapper );
		break;
	}


	volume->SetProperty( property );
	property->SetColor( 0, colorFunction );
	property->SetScalarOpacity( 0, opacityFunction );
}

void VTKImageComponents::mergeImage ( vtkImageData *image )
{
	vtkImageAppendComponents *merger = vtkImageAppendComponents::New();
	merger->AddInput( 0, imageData );
	merger->AddInput( 0, image );
	merger->Update();
	setVTKImageData( image );
	//  property->SetScalarOpacity(1, opacityFunction );
	//  property->SetColor(1, colorFunction );
	//  volume->SetProperty( property );
}


void VTKImageComponents::setMapperType ( VTKImageComponents::VTKMapperType mapper )
{
	currentMapperType = mapper;

	switch( mapper ) {
	case Texture:
		textureMapper = vtkVolumeTextureMapper3D::New();
		currentVolumeMapper = textureMapper;
		volume->SetMapper( textureMapper );
		break;
	case GPU_VolumeRayCast:
		gpuVolumeRayMapper = vtkGPUVolumeRayCastMapper::New();
		currentVolumeMapper = gpuVolumeRayMapper;
		volume->SetMapper( gpuVolumeRayMapper );
		break;
	case CPU_FixedPointRayCast:
		fixedRayMapper = vtkFixedPointVolumeRayCastMapper::New();
		currentVolumeMapper = fixedRayMapper;
		volume->SetMapper( fixedRayMapper );
		break;
	}

	setVTKImageData( imageData );

	if( croppingSet ) {
		setCropping( currentCropping );
	}

}


void VTKImageComponents::setVTKImageData ( vtkImageData *image )
{
	imageData = image;
	currentVolumeMapper->SetInput( image );

}
void VTKImageComponents::setCropping ( double *cropping )
{
	croppingSet = true;
	currentVolumeMapper->CroppingOn();
	currentVolumeMapper->SetCroppingRegionFlagsToFence();
	currentVolumeMapper->SetCroppingRegionPlanes( cropping );

	for( unsigned short i = 0; i < 6; i++ ) {
		currentCropping[i] = cropping[i];
	}
}


}
}
} // end namespace