#include "QMemoryHandler.hpp"
#include "QOrientationHandler.hpp"

namespace isis
{
namespace viewer
{



QMemoryHandler::QMemoryHandler( QViewerCore *core )
	: m_ViewerCore( core )
{

}


bool QMemoryHandler::fillSliceChunk( data::MemChunk< InternalImageType > &sliceChunk, const boost::shared_ptr< ImageHolder > image, const PlaneOrientation &orientation, const size_t &timestep ) const
{
	const util::ivector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( image->getImageSize(), image, orientation );
	const util::ivector4 mappedCoords = QOrienationHandler::mapCoordsToOrientation( image->voxelCoords, image, orientation );
	const util::ivector4 mapping = QOrienationHandler::mapCoordsToOrientation( util::ivector4( 0, 1, 2, 3 ), image, orientation, true );
	data::Chunk chunk = image->getChunkVector()[timestep];
#pragma omp parallel for
	for ( uint16_t y = 0; y < mappedSize[1]; y++ ) {
		for ( uint16_t x = 0; x < mappedSize[0]; x++ ) {
			const util::ivector4 coords(x,y,mappedCoords[2] );
			sliceChunk.voxel<InternalImageType>( coords[0], coords[1] ) =
				chunk.voxel<InternalImageType>( coords[mapping[0]], coords[mapping[1]], coords[mapping[2]] );
		}
	}
}


}
} // end namespace