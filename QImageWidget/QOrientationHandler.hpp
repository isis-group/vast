#ifndef QORIENTATIONHANDLER_HPP
#define QORIENTATIONHANDLER_HPP

#include <QTransform>
#include "ImageHolder.hpp"
#include "common.hpp"
#include "QtWidgetCommon.hpp"

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/concept_check.hpp>
#include <math.h>

namespace isis
{
namespace viewer
{
namespace qt
{

class QOrienationHandler
{

public:
	static util::fvector4 mapCoordsToOrientation( const util::fvector4 &coords, const boost::shared_ptr<ImageHolder> image, PlaneOrientation orientation, bool back = false, bool absolute = true );
	static util::ivector4 getMappedCoords( const boost::shared_ptr<ImageHolder> image, const util::ivector4 &coords );

	static ViewPortType getViewPort( util::PropertyMap &properties, const boost::shared_ptr< ImageHolder > image, const size_t &w, const size_t &h, PlaneOrientation orientation );
	static QTransform getTransform( const ViewPortType &viewPort, const boost::shared_ptr< ImageHolder > image, const size_t &w, const size_t &h, PlaneOrientation orientation );

	static util::ivector4 convertWindow2VoxelCoords( const ViewPortType &viewPort, const util::PropertyMap &properties, const boost::shared_ptr< ImageHolder > image, const size_t &x, const size_t &y, const size_t &slice, PlaneOrientation orientation );
	static std::pair<size_t, size_t> convertVoxel2WindowCoords( const ViewPortType &viewPort, const util::PropertyMap &properties, const boost::shared_ptr< ImageHolder > image, PlaneOrientation orientation );
};


}
}
}// end namespace


#endif