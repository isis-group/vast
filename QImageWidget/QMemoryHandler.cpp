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


void QMemoryHandler::getSlice( boost::shared_ptr< ImageHolder > image, size_t slice, InternalImageType *retPointer, PlaneOrientation orientation, const util::ivector4 &alignedSize )
{
    InternalImageType *dataPtr = static_cast<InternalImageType *>( image->getImageWeakPointer( 0 ).lock().get() );
    assert( dataPtr != 0 );
    size_t planeSize = image->getImageSize()[0] * image->getImageSize()[1];
    util::ivector4 diffSize = alignedSize - image->getImageSize();
    
    switch (orientation) {
	case axial:
	    for ( size_t i = 0; i < image->getImageSize()[data::columnDim]; i++ ) {
		memcpy(retPointer + i * alignedSize[data::rowDim], dataPtr + i * image->getImageSize()[data::rowDim] + planeSize * slice, image->getImageSize()[data::rowDim]);
		for(size_t j = 0; j < diffSize[data::rowDim]; j++)
		{
		    retPointer[(i+1)*image->getImageSize()[data::rowDim] + j] = 0;
		}
	    }
	    break;
	case coronal:
	    for ( size_t i = 0; i< image->getImageSize()[2]; i++ ) {
		memcpy(retPointer + i * alignedSize[data::rowDim], dataPtr + i * planeSize + image->getImageSize()[data::rowDim] * slice,image->getImageSize()[data::rowDim] );
		for(size_t j = 0; j < diffSize[data::rowDim]; j++)
		{
		    retPointer[(i+1)*image->getImageSize()[data::rowDim] + j] = 0;
		}
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
    
QImage QMemoryHandler::getQImage(const boost::shared_ptr< ImageHolder > image, const size_t &slice, PlaneOrientation orientation ) const
{
    size_t x = 0;
    size_t y = 0;
    //qt is only capable of creating images that have 32-bit aligned size
    util::ivector4 alignedSize = image->getImageProperties().alignedSize32Bit;
    
    switch( orientation ) {
	case axial:
	    x = alignedSize[0];
	    y = alignedSize[1];
	    break;
	case sagittal:
	    x = alignedSize[1];
	    y = alignedSize[2];
	    break;
	case coronal:
	    x = alignedSize[0];
	    y = alignedSize[2];
	    break;
    }
    //we create an empty MemChunk that eventually will hold our extracted slice
    data::MemChunk<InternalImageType> tmpChunk(x,y);
    
    //allocate memory of the aligned size
    
    
    InternalImageType *dataPtr = ( InternalImageType * ) calloc( x*y , sizeof( InternalImageType ) );
    
    
    
    return QImage( (InternalImageType*)dataPtr,x,y, QImage::Format_Indexed8 );
    
}
    
    
}

    
}} // end namespace