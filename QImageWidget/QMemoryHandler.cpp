#include "QMemoryHandler.hpp"
#include "DataStorage/io_factory.hpp"
#include "QtWidgetCommon.hpp"
#include "QOrientationHandler.hpp"

namespace isis {
namespace viewer {
namespace qt {
    
    
QMemoryHandler::QMemoryHandler(QViewerCore* core)
    : m_ViewerCore( core )
{

}


bool QMemoryHandler::fillSliceChunk(data::MemChunk< InternalImageType > &sliceChunk, const boost::shared_ptr< ImageHolder > image, const size_t slice, PlaneOrientation orientation) const
{
    util::ivector4 mappedSize = QOrienationHandler::mapCoordsToOrientation( image->getImageSize(), image, orientation );
    
    data::Chunk chunk = image->getChunkVector()[0];
     for ( size_t y = 0; y < mappedSize[1]; y++ ) {
	for ( size_t x = 0; x < mappedSize[0]; x++ ) {
	    sliceChunk.voxel<InternalImageType>(x,y) =
		chunk.voxel<InternalImageType>( x, y, slice );
	}
    }
}
 
    
}

    
}} // end namespace