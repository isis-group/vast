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

vtkImageData *VolumeHandler::getVTKImageData( const ImageHolder::Pointer image, const size_t &timestep )
{
	using namespace boost::numeric::ublas;
	const util::ivector4 size = image->getImageSize();
	vtkImageData *newImage = vtkImageData::New();
	vtkImageImport *importer = vtkImageImport::New();
	vtkTransform *transform = vtkTransform::New();
	vtkImageReslice *reslicer = vtkImageReslice::New();
	vtkMatrix4x4 *orientationMatrix = vtkMatrix4x4::New();
	transform->Identity();
	newImage->SetScalarTypeToUnsignedChar();
	importer->SetDataScalarTypeToUnsignedChar();
	importer->SetImportVoidPointer( &image->getChunkVector().operator[]( timestep ).voxel<InternalImageType>( 0 ) );
	importer->SetWholeExtent( 0, size[0] - 1, 0, size[1] - 1, 0, size[2] - 1 );
	importer->SetDataExtentToWholeExtent();
	importer->Update();
	newImage = importer->GetOutput();

	//transform the image with orientation matrix
	const util::fvector4 mappedSize = image->getImageProperties().orientation.dot( size );
	const util::fvector4 mappedSpacing = image->getImageProperties().orientation.dot( image->getImageProperties().voxelSize );
	orientationMatrix->SetElement( 3, 3, 1 );

	for( uint8_t i = 0; i < 4; i++ ) {
		for ( uint8_t j = 0; j < 4; j++ ) {
			orientationMatrix->SetElement( i, j, image->getImageProperties().orientation.elem( j, i ) / fabs( mappedSpacing[j] ) );
		}
	}

	transform->SetMatrix( orientationMatrix );

	reslicer->SetInput( newImage );
	util::fvector4 start;
	util::fvector4 end;

	for( uint8_t i = 0; i < 4; i++ ) {
		if( mappedSize[i] > 0 ) {
			start[i] = -image->getImageProperties().indexOrigin[i];
			end[i] = mappedSize[i] * fabs( mappedSpacing[i] ) - image->getImageProperties().indexOrigin[i];
		} else {
			start[i] = mappedSize[i] * fabs( mappedSpacing[i] ) - image->getImageProperties().indexOrigin[i];
			end[i] = -image->getImageProperties().indexOrigin[i];
		}
	}

	reslicer->SetOutputExtent( start[0], end[0], start[1], end[1], start[2] , end[2]  );
	reslicer->SetOutputOrigin( image->getImageProperties().indexOrigin[0], image->getImageProperties().indexOrigin[1], image->getImageProperties().indexOrigin[2] );
	reslicer->SetOutputSpacing( 1, 1, 1 );
	reslicer->SetInterpolationModeToNearestNeighbor();
	reslicer->SetResliceTransform( transform );
	reslicer->Update();
	return reslicer->GetOutput();
}




}
}
} //end namespace