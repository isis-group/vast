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
 * Author: Erik Tuerke, tuerke@cbs.mpg.de, Samuel Eckermann, Hans-Christian Heinz
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
#include <vtkImageData.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolume.h>

namespace isis {
namespace viewer {
namespace widget {


class VTKImageComponents {
public:
	VTKImageComponents();


	void setVTKImageData( vtkImageData *image );
	vtkImageData* getVTKImageData() const { return imageData; }

	vtkVolume *volume;
	vtkVolumeProperty *property;
	vtkFixedPointVolumeRayCastMapper *mapper;
	vtkColorTransferFunction *colorFunction;
	vtkPiecewiseFunction *opacityFunction;
private:
	vtkImageData *imageData;
};

}}} //end namespace

#endif //IMAGE_COMPONENTS_HPP