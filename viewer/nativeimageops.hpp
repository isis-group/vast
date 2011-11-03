#ifndef NATIVEIMAGEOPS_HPP
#define NATIVEIMAGEOPS_HPP

#include "common.hpp"

#include <boost/concept_check.hpp>
#include <numeric>
#include "qprogressfeedback.hpp"
#include "imageholder.hpp"

namespace isis {
namespace viewer {
namespace operation {
	
class NativeImageOps
{
	static boost::shared_ptr< QProgressFeedback > m_ProgressFeedback;
	static QViewerCore *m_ViewerCore;
public:
	
	static util::ivector4 getGlobalMin( const boost::shared_ptr<ImageHolder> image );
	static util::ivector4 getGlobalMax( const boost::shared_ptr<ImageHolder> image );
	
	static void setProgressFeedBack( boost::shared_ptr< QProgressFeedback > progressFeedback );
	
private:
	
	template<typename TYPE>
	static util::ivector4 internGetMin( const boost::shared_ptr<ImageHolder> image ) {		
		struct FindMinOp : public data::VoxelOp<TYPE>
		{
			TYPE currentMin;
			util::ivector4 currentPos;
			FindMinOp() { currentMin = std::numeric_limits<TYPE>::max() ; }
			bool operator() ( TYPE &vox, const util::FixedVector<size_t, 4> &pos ) {
				if ( vox < currentMin ) {
					currentMin = vox;
					currentPos = pos;
				}
				return true;
			}
		};
		FindMinOp findMin;
		image->getISISImage()->foreachVoxel( findMin );
		return findMin.currentPos;
	}
	
	template<typename TYPE>
	static util::ivector4 internGetMax( const boost::shared_ptr<ImageHolder> image ) {
		struct FindMaxOp : public data::VoxelOp<TYPE>
		{
			TYPE currentMax;
			util::ivector4 currentPos;
			FindMaxOp() { currentMax = std::numeric_limits<TYPE>::min() ;}
			bool operator() ( TYPE &vox, const util::FixedVector<size_t, 4> &pos ) {
				if ( vox > currentMax ) {
					currentMax = vox;
					currentPos = pos;
				}
				return true;
			}
		};
		FindMaxOp findMax;
		image->getISISImage()->foreachVoxel( findMax );
		return findMax.currentPos;
	}
};
	
	
	
}}}


#endif