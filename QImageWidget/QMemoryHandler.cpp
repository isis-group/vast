#include "QMemoryHandler.hpp"


namespace isis {
namespace viewer {
namespace qt {
    
    
QMemoryHandler::QMemoryHandler(QViewerCore* core)
    : m_ViewerCore( core )
{

}


void QMemoryHandler::getSlice( boost::shared_ptr< ImageHolder > image, uint8_t slice, uint8_t *retPointer, PlaneOrientation orientation )
{
    uint8_t *dataPtr = static_cast<uint8_t *>( image->getImageWeakPointer( 0 ).lock().get() );
    assert( dataPtr != 0 );
    size_t planeSize = 0;
    size_t startIndex = 0;
    switch (orientation) {
	case axial:
	    planeSize = image->getImageSize()[0] * image->getImageSize()[1];
	    startIndex = planeSize * slice;
	    break;
	case sagittal:
	    planeSize = image->getImageSize()[0] * image->getImageSize()[2];
	    startIndex = image->getImageSize()[1] * slice;
	    break;
	case coronal:
	    planeSize = image->getImageSize()[1] * image->getImageSize()[2];
	    startIndex = slice;
	    break;
    } 
    
    

    
    
}

    
}}} // end namespace