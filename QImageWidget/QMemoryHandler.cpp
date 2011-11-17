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


bool QMemoryHandler::fillSliceChunk( data::MemChunk< InternalImageType > &sliceChunk, const boost::shared_ptr< ImageHolder > image, PlaneOrientation orientation, size_t timestep ) const
{
	util::ivector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( image->getImageSize(), image, orientation );
	util::ivector4 mappedCoords = QOrienationHandler::mapCoordsToOrientation( image->voxelCoords, image, orientation );
	util::ivector4 mapping = QOrienationHandler::mapCoordsToOrientation( util::ivector4( 0, 1, 2, 3 ), image, orientation, true );
	data::Chunk chunk = image->getChunkVector()[timestep];
	util::ivector4 coords;
	coords[2] = mappedCoords[2];
	for ( coords[1] = 0; coords[1] < mappedSize[1]; coords[1]++ ) {
		for ( coords[0] = 0; coords[0] < mappedSize[0]; coords[0]++ ) {
			sliceChunk.voxel<InternalImageType>( coords[0], coords[1] ) =
				chunk.voxel<InternalImageType>( coords[mapping[0]], coords[mapping[1]], coords[mapping[2]] );
		}
	}
}


}
} // end namespace