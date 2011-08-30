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
    
    //debug
    void getSlice( boost::shared_ptr< ImageHolder> image, uint8_t slice, uint8_t *retPointer, PlaneOrientation orientation, const util::ivector4 &alignedSize );
    
private:
    QViewerCore *m_ViewerCore;
};


}}} // end namespace


#endif