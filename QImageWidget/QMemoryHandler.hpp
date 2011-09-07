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
    void getSlice( boost::shared_ptr< ImageHolder> image, size_t slice, InternalImageType *retPointer, PlaneOrientation orientation, const util::ivector4 &alignedSize );
    
    QImage getQImage( const boost::shared_ptr< ImageHolder > image, const size_t &slice, PlaneOrientation orientation ) const;
private:
    QViewerCore *m_ViewerCore;
};


}}} // end namespace


#endif