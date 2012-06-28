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
 * ImageComponents.hpp
 *
 * Description:
 *
 *  Created on: Mar 05, 2012
 ******************************************************************/

#ifndef IMAGE_COMPONENTS_HPP
#define IMAGE_COMPONENTS_HPP

#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeTextureMapper3D.h>

#include <vtkImageData.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkImageAppendComponents.h>
#include <vtkVolume.h>

namespace isis
{
namespace viewer
{
namespace widget
{


class VTKImageComponents
{
public:
	enum VTKMapperType { Texture, CPU_FixedPointRayCast, GPU_VolumeRayCast };

	VTKImageComponents( VTKMapperType = CPU_FixedPointRayCast );

	void setVTKImageData( vtkImageData *image );
	vtkImageData *getVTKImageData() const { return imageData; }

	void mergeImage( vtkImageData *image );

	void setMapperType( VTKMapperType mapper );

	void setCropping( double *cropping );

	vtkVolume *volume;
	vtkVolumeProperty *property;

	vtkFixedPointVolumeRayCastMapper *fixedRayMapper;
	vtkGPUVolumeRayCastMapper *gpuVolumeRayMapper;
	vtkVolumeTextureMapper3D *textureMapper;

	vtkColorTransferFunction *colorFunction;
	vtkPiecewiseFunction *opacityFunction;

private:
	vtkImageData *imageData;

	double *currentCropping;
	bool croppingSet;

	vtkVolumeMapper *currentVolumeMapper;

	VTKMapperType currentMapperType;

};

}
}
} //end namespace

#endif //IMAGE_COMPONENTS_HPP