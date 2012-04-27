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

VTKImageComponents::VTKImageComponents(bool ray)
	: volume( vtkVolume::New() ),
	  property( vtkVolumeProperty::New() ),
	  rayMapper( vtkFixedPointVolumeRayCastMapper::New() ),
	  textureMapper( vtkVolumeTextureMapper3D::New() ),
	  colorFunction( vtkColorTransferFunction::New() ),
	  opacityFunction( vtkPiecewiseFunction::New() ),
	  imageData( vtkImageData::New() ),
	  rayMapping( ray )
{
	if( ray ) {
		volume->SetMapper( rayMapper );
	} else {
		volume->SetMapper( textureMapper );
	}
	volume->SetProperty( property );
	property->SetColor( colorFunction );
	property->SetScalarOpacity( opacityFunction );

}


void VTKImageComponents::setVTKImageData ( vtkImageData *image )
{
	imageData = image;
	if( rayMapping ) {
		rayMapper->SetInput( image );
	} else {
		textureMapper->SetInput( image );
	}
}

void VTKImageComponents::setCropping ( double* cropping )
{
	if( rayMapping ) {
		rayMapper->CroppingOn();
		rayMapper->SetCroppingRegionFlagsToFence();
		rayMapper->SetCroppingRegionPlanes( cropping );
	} else {
		textureMapper->CroppingOn();
		textureMapper->SetCroppingRegionFlagsToFence();
		textureMapper->SetCroppingRegionPlanes( cropping );
		
	}
}


}
}
} // end namespace