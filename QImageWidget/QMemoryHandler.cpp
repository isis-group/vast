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
//     switch (orientation) {
// 	case axial:
// 	    std::cout << "axial" << std::endl;
// 	    break;
// 	case sagittal:
// 	    std::cout << "sagittal" << std::endl;
// 	    break;
// 	case coronal:
// 	    std::cout << "coronal" << std::endl;
// 	    break;
//     }
//     std::cout << mappedSize << std::endl;
    util::ivector4 t = QOrienationHandler::mapCoordsToOrientation( util::ivector4(0,1,2,3), image, orientation, true );
//     std::cout << t << std::endl;
    data::Chunk chunk = image->getChunkVector()[0];
     for ( size_t y = 0; y < mappedSize[1]; y++ ) {
	for ( size_t x = 0; x < mappedSize[0]; x++ ) {
	    util::ivector4 coords (x,y,slice);
	    sliceChunk.voxel<InternalImageType>(x,y) = 
		chunk.voxel<InternalImageType>( coords[t[0]], coords[t[1]], coords[t[2]] );
	}
    }
}
 
    
}

    
}} // end namespace