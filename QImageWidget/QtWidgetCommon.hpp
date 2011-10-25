#ifndef QTWIDGETCOMMON_HPP
#define QTWIDGETCOMMON_HPP

#include "CoreUtils/vector.hpp"
#include "imageholder.hpp"

namespace isis
{
namespace viewer
{
namespace qt
{

typedef std::vector<boost::shared_ptr<ImageHolder> > ImageVectorType;
typedef  util::FixedVector<float, 6 > ViewPortType;



}
}
}//end namespace



#endif