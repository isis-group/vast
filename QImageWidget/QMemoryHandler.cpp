#include "QMemoryHandler.hpp"
#include "DataStorage/io_factory.hpp"
#include "QtWidgetCommon.hpp"

namespace isis {
namespace viewer {
namespace qt {
    
    
QMemoryHandler::QMemoryHandler(QViewerCore* core)
    : m_ViewerCore( core )
{

}


void QMemoryHandler::getSlice( boost::shared_ptr< ImageHolder > image, uint8_t slice, uint8_t *retPointer, PlaneOrientation orientation, const util::ivector4 &alignedSize )
{
    uint8_t *dataPtr = static_cast<uint8_t *>( image->getImageWeakPointer( 0 ).lock().get() );
    assert( dataPtr != 0 );
    size_t planeSize = image->getImageSize()[0] * image->getImageSize()[1];
    util::ivector4 diffSize = alignedSize - image->getImageSize();
    
    
    switch (orientation) {
	case axial:
	    memcpy(retPointer, dataPtr + planeSize * slice, planeSize );
	    break;
	case coronal:
	    for ( size_t i = 0; i< image->getImageSize()[2]; i++ ) {
		memcpy(retPointer + i * image->getImageSize()[0], dataPtr + i * planeSize + image->getImageSize()[0] * slice,image->getImageSize()[0] );
	    }
	    break;
	case sagittal:
	    size_t index = 0;
	    for ( size_t s = 0; s < image->getImageSize()[data::sliceDim]; s++ ) {
		for ( size_t c = 0; c < image->getImageSize()[data::columnDim]; c++ ) {
		    
		    retPointer[index++] = dataPtr[slice + c * image->getImageSize()[data::rowDim] + s * planeSize ];
		}
		for( size_t i = 0; i < diffSize[data::columnDim]; i++) {
		    retPointer[index++] = 0;
		}
	    }
	    break;
    } 
    
    

    
    
}

    
}}} // end namespace