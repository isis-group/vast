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
 * VolumeHanderl.cpp
 *
 * Description:
 *
 *  Created on: Feb 28, 2012
 ******************************************************************/
#include "VolumeHandler.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{


VolumeHandler::VolumeHandler( )
{
}

vtkImageData *VolumeHandler::getVTKImageData( const ImageHolder::Pointer image, const geometrical::BoundingBoxType &bb, const size_t &timestep )
{
	const util::ivector4 size = image->getImageSize();
	vtkImageImport *importer = vtkImageImport::New();
	vtkTransform *transform = vtkTransform::New();
	vtkImageReslice *reslicer = vtkImageReslice::New();
	vtkMatrix4x4 *orientationMatrix = vtkMatrix4x4::New();
	transform->Identity();
	importer->SetDataScalarTypeToUnsignedChar();
	importer->SetImportVoidPointer( &image->getVolumeVector().operator[]( timestep ).voxel<InternalImageType>( 0 ) );
	importer->SetWholeExtent( 0, size[0] - 1, 0, size[1] - 1, 0, size[2] - 1 );
	importer->SetDataExtentToWholeExtent();

	//transform the image with orientation matrix
	const util::fvector3 mio = image->getImageProperties().orientation.transpose().dot( image->getImageProperties().indexOrigin );
	orientationMatrix->SetElement( 3, 3, 1 );

	for( uint8_t i = 0; i < 3; i++ ) {
		for ( uint8_t j = 0; j < 3; j++ ) {
			orientationMatrix->SetElement( i, j, image->getImageProperties().orientation.elem( i, j ) / image->getImageProperties().voxelSize[i] );
		}

		orientationMatrix->SetElement( i, 3, -1 * mio[i] / image->getImageProperties().voxelSize[i] );
	}

	transform->SetMatrix( orientationMatrix );
	reslicer->SetInput( importer->GetOutput() );
	reslicer->SetOutputExtent( bb[0].first, bb[0].second, bb[1].first, bb[1].second, bb[2].first, bb[2].second );
	reslicer->SetInterpolationModeToNearestNeighbor();
	reslicer->SetResliceTransform( transform );
	reslicer->AutoCropOutputOn();
	reslicer->Update();
	return reslicer->GetOutput();
}


}
}
} //end namespace