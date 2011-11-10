#ifndef NATIVEIMAGEOPS_HPP
#define NATIVEIMAGEOPS_HPP

#include "common.hpp"

#include <boost/concept_check.hpp>
#include <numeric>
#include "qprogressfeedback.hpp"
#include "imageholder.hpp"
#include <DataStorage/image.hpp>

namespace isis
{
namespace viewer
{
namespace operation
{

class NativeImageOps
{
	static boost::shared_ptr< QProgressFeedback > m_ProgressFeedback;
	static QViewerCore *m_ViewerCore;
public:

	static util::ivector4 getGlobalMin( const boost::shared_ptr<ImageHolder> image, const util::ivector4 &startPos, const unsigned short &radius );
	static util::ivector4 getGlobalMax( const boost::shared_ptr<ImageHolder> image, const util::ivector4 &startPos, const unsigned short &radius );

	static void setProgressFeedBack( boost::shared_ptr< QProgressFeedback > progressFeedback );

private:

	template<typename TYPE>
	static util::ivector4 internGetMin( const boost::shared_ptr<ImageHolder> image, const util::ivector4 &startPos, const unsigned short &radius ) {
		const util::ivector4 size = image->getImageSize();
		util::ivector4 start;
		util::ivector4 end;

		if( radius ) {
			for( size_t i = 0; i < 3; i++ ) {
				start[i] = ( startPos[i] - radius ) < 0 ? 0 : startPos[i] - radius;
				end[i] = ( startPos[i] + radius ) > size[i] ? size[i] : startPos[i] + radius;
			}
		} else {
			start = util::ivector4( 0, 0, 0, 0 );
			end = image->getImageSize();
		}

		util::ivector4 currentPos;
		TYPE currentValue;
		data::TypedImage<TYPE> typedImage = *image->getISISImage();
		TYPE currentMin = std::numeric_limits<TYPE>::max();

		for( unsigned short z = start[2]; z < end[2]; z++ ) {
			for( unsigned short y = start[1]; y < end[1]; y++ ) {
				for( unsigned short x = start[0]; x < end[0]; x++ ) {
					currentValue = static_cast<data::Image &>( typedImage ).voxel<TYPE>( x, y, z ); //TODO wtf??

					if( currentValue < currentMin ) {
						currentPos = util::ivector4( x, y, z );
						currentMin = currentValue;
					}
				}
			}
		}

		return currentPos;
	}

	template<typename TYPE>
	static util::ivector4 internGetMax( const boost::shared_ptr<ImageHolder> image, const util::ivector4 &startPos, const unsigned short &radius ) {
		const util::ivector4 size = image->getImageSize();
		util::ivector4 start;
		util::ivector4 end;

		if( radius ) {
			for( size_t i = 0; i < 3; i++ ) {
				start[i] = ( startPos[i] - radius ) < 0 ? 0 : startPos[i] - radius;
				end[i] = ( startPos[i] + radius ) > size[i] ? size[i] : startPos[i] + radius;
			}
		} else {
			start = util::ivector4( 0, 0, 0, 0 );
			end = image->getImageSize();
		}

		util::ivector4 currentPos;
		TYPE currentValue;
		data::TypedImage<TYPE> typedImage = *image->getISISImage();
		TYPE currentMax = std::numeric_limits<TYPE>::min();

		for( unsigned short z = start[2]; z < end[2]; z++ ) {
			for( unsigned short y = start[1]; y < end[1]; y++ ) {
				for( unsigned short x = start[0]; x < end[0]; x++ ) {
					currentValue = static_cast<data::Image &>( typedImage ).voxel<TYPE>( x, y, z ); //TODO wtf??

					if( currentValue > currentMax ) {
						currentPos = util::ivector4( x, y, z );
						currentMax = currentValue;
					}
				}
			}
		}

		return currentPos;
	}
};



}
}
}


#endif