#ifndef QMEMORYHANDLER_HPP
#define QMEMORYHANDLER_HPP

#include "QViewerCore.hpp"

namespace isis
{
namespace viewer
{
namespace qt 
{
class QMemoryHandler
{
public:
    QMemoryHandler( QViewerCore *core );
    
    bool fillSliceChunk( data::MemChunk<InternalImageType> &sliceChunk, const boost::shared_ptr< ImageHolder > image, PlaneOrientation orientation ) const;
    
private:
    QViewerCore *m_ViewerCore;
};


}}} // end namespace


#endif