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
namespace widget {


VolumeHandler::VolumeHandler( )
	:m_Merger( vtkImageAppendComponents::New() ),
	m_MergedImage( vtkImageData::New() ),
	m_Importer( vtkImageImport::New() )
{
}

bool VolumeHandler::addImage ( boost::shared_ptr< ImageHolder > image, const size_t &timestep )
{
	if( std::find( m_ImageList.begin(), m_ImageList.end(), image ) == m_ImageList.end() ) {
		m_ImageList.push_back( image );
		const util::ivector4 size = image->getImageSize();
		const util::fvector4 physSize = size * image->voxelSize;
		vtkImageData *newImage = vtkImageData::New();
		newImage->SetScalarTypeToUnsignedChar();
		m_Importer->SetDataScalarTypeToUnsignedChar();

		m_Importer->SetImportVoidPointer( &image->getChunkVector().operator[](timestep).voxel<InternalImageType>(0) );
		
		newImage->SetSpacing( image->voxelSize[0], image->voxelSize[1], image->voxelSize[2] );
		newImage->SetDimensions( size[0], size[1], size[2] );
		newImage->SetOrigin( image->indexOrigin[0], image->indexOrigin[1], image->indexOrigin[2] );
		newImage->SetExtent(0, physSize[0], 0, physSize[1], 0, physSize[2] );

		m_Importer->SetWholeExtent( 0, size[0] - 1, 0, size[1] - 1, 0, size[2] - 1 );
		m_Importer->SetDataExtentToWholeExtent();
		m_Importer->Update();
		newImage = m_Importer->GetOutput();
		
		if( m_ImageList.size() > 1 ) {
			m_Merger->SetInput(0, m_MergedImage );
			m_Merger->SetInput(1, newImage);
			m_Merger->Update();
			m_MergedImage = m_Merger->GetOutput();
		} else {
			m_MergedImage = newImage;
		}		
	}
}

vtkImageData* VolumeHandler::getMergedImage()
{
	return m_MergedImage;
}


	

}}} //end namespace