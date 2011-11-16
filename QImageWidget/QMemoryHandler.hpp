#ifndef QMEMORYHANDLER_HPP
#define QMEMORYHANDLER_HPP

#include "qviewercore.hpp"

namespace isis
{
namespace viewer
{

class QMemoryHandler
{
public:
	QMemoryHandler( QViewerCore *core );

	bool fillSliceChunk( data::MemChunk<InternalImageType> &sliceChunk, const boost::shared_ptr< ImageHolder > image, PlaneOrientation orientation, size_t timestep = 0 ) const;

private:
	QViewerCore *m_ViewerCore;
};



}
} // end namespace


#endif