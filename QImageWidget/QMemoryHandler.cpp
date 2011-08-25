#include "QMemoryHandler.hpp"


namespace isis {
namespace viewer {
namespace qt {
    
    
QMemoryHandler::QMemoryHandler(QViewerCore* core)
    : m_ViewerCore( core )
{

}


void QMemoryHandler::getSlice( boost::shared_ptr< ImageHolder > image, uint8_t slice, uint8_t *retPointer )
{
    uint8_t *dataPtr = static_cast<uint8_t *>( image->getImageWeakPointer( 0 ).lock().get() );
    assert( dataPtr != 0 );
    size_t plane = image->getImageSize()[0] * image->getImageSize()[1];
    size_t index = slice * plane;
    for ( size_t i = 0; i < plane ; i++ ) {
	retPointer[i] = dataPtr[index++];
	
    }

}

    
}}} // end namespace