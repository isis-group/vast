#ifndef QMEMORYHANDLER_HPP
#define QMEMORYHANDLER_HPP

#include "qviewercore.hpp"
#include "QOrientationHandler.hpp"

struct stat;
namespace isis
{
namespace viewer
{

class QMemoryHandler
{
public:
	QMemoryHandler( QViewerCore *core );

	template< typename TYPE>
	void fillSliceChunk( data::MemChunk<TYPE> &sliceChunk, const boost::shared_ptr< ImageHolder > image, const PlaneOrientation &orientation, const size_t &timestep = 0 ) const {
		const util::ivector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( image->getImageSize(), image, orientation );
		const util::ivector4 mappedCoords = QOrienationHandler::mapCoordsToOrientation( image->voxelCoords, image, orientation );
		const util::ivector4 mapping = QOrienationHandler::mapCoordsToOrientation( util::ivector4( 0, 1, 2, 3 ), image, orientation, true );
		const data::Chunk chunk = image->getChunkVector()[timestep];
		#pragma omp parallel for

		for ( int32_t y = 0; y < mappedSize[1]; y++ ) {
			for ( int32_t x = 0; x < mappedSize[0]; x++ ) {
				const util::ivector4 coords( x, y, mappedCoords[2] );
				static_cast<data::Chunk &>( sliceChunk ).voxel<TYPE>( coords[0], coords[1] ) = chunk.voxel<TYPE>( coords[mapping[0]], coords[mapping[1]], coords[mapping[2]] );
			}
		}
	}

private:
	QViewerCore *m_ViewerCore;
};



}
} // end namespace


#endif