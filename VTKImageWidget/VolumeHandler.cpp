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
 * VolumeHanderl.cpp
 *
 * Description:
 *
 *  Created on: Feb 28, 2012
 ******************************************************************/
#include "VolumeHandler.hpp"


namespace isis {
namespace viewer {
namespace vtk {


VolumeHandler::VolumeHandler( )
	:m_Merger( vtkImageAppendComponents::New() ),
	m_MergedImage( vtkImageData::New() )
{
}

bool VolumeHandler::addImage ( boost::shared_ptr< ImageHolder > image, const size_t &timestep )
{
	if( std::find( m_ImageList.begin(), m_ImageList.end(), image ) == m_ImageList.end() ) {
		m_ImageList.push_back( image );
		const util::ivector4 size = image->getImageSize();
		vtkImageData *newImage = vtkImageData::New();
		newImage->SetScalarTypeToUnsignedChar();

		newImage->SetSpacing( image->voxelSize[0], image->voxelSize[1], image->voxelSize[2] );
		newImage->SetDimensions( size[0], size[1], size[2] );
		newImage->SetOrigin( image->indexOrigin[0], image->indexOrigin[1], image->indexOrigin[2] );

		m_Merger->AddInput(m_MergedImage);
		for( int64_t slice = 0; slice < size[2]; slice++ ) {
			for( int64_t column = 0; column < size[2]; column++ ) {
				for( int64_t row = 0; row < size[2]; row++ ) {
					newImage->SetScalarComponentFromFloat( row, column, slice, 0, image->getChunkVector().operator[](timestep).voxel<InternalImageType>(row, column, slice ) );
				}
			}
		}
		m_Merger->AddInput(newImage);
		m_Merger->Update();
		m_MergedImage = newImage;
	}
}

vtkImageData* VolumeHandler::getMergedImage()
{
	return m_MergedImage;
}


	

}}} //end namespace