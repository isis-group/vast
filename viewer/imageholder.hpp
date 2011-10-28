#ifndef IMAGEHOLDER_HPP
#define IMAGEHOLDER_HPP

#include <boost/foreach.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <vector>
#include <CoreUtils/propmap.hpp>
#include <DataStorage/image.hpp>
#include "common.hpp"
#include "color.hpp"

namespace isis
{
namespace viewer
{

/**
 * Class that holds one image in a vector of data::ValuePtr's
 * It ensures the data is hold in continuous memory and only consists of one type.
 * Furthermore this class handles the meta information of the image
 */

class ImageHolder
{


public:
	typedef std::list<boost::shared_ptr< ImageHolder > > ImageListType;
	typedef data::_internal::ValuePtrBase::Reference ImagePointerType;

	enum ImageType { anatomical_image, z_map };

	//here we store only properties which can not be stored as isis::PropertyValue
	struct ImageProperties {
		ImageType imageType;
		InterpolationType interpolationType;
		std::pair<double, double> zmapThreshold;

	};
	ImageHolder( );

	bool setImage( const data::Image &image, const ImageType &imageType, const std::string &filename = "" );

	size_t getID() const { return m_ID; }
	void setID( size_t id ) { m_ID = id; }

	std::vector< ImagePointerType > getImageVector() const { return m_ImageVector; }
	std::vector< data::Chunk > getChunkVector() const { return m_ChunkVector; }
	util::PropertyMap &getPropMap() { return m_PropMap; }
	const util::PropertyMap &getPropMap() const { return m_PropMap; }
	const util::FixedVector<size_t, 4> &getImageSize() const { return m_ImageSize; }
	boost::shared_ptr< data::Image >getISISImage() const { return m_Image; }
	boost::numeric::ublas::matrix<float> getNormalizedImageOrientation( bool transposed = false ) const;
	boost::numeric::ublas::matrix<float> getImageOrientation( bool transposed = false ) const;
	std::pair<util::ValueReference, util::ValueReference> getMinMax() const { return m_MinMax; }
	std::pair<util::ValueReference, util::ValueReference> getInternMinMax() const { return m_InternMinMax; }
	void addChangedAttribute( const std::string &attribute );

	/**offset, scaling**/
	std::pair<double, double> getOptimalScalingPair() const { return m_OptimalScalingPair;  }
	boost::weak_ptr<void>
	getImageWeakPointer( size_t timestep = 0 ) const {
		return getImageVector()[timestep]->getRawAddress();
	}

	util::slist getFileNames() const { return m_Filenames; }

	bool operator<( const ImageHolder &ref ) const { return m_ID < ref.getID(); }

	const ImageProperties &getImageProperties() const { return m_ImageProperties; }
	ImageProperties &getImageProperties() { return m_ImageProperties; }

	/**offset, scaling**/
	template<typename TYPE>
	std::pair<double, double> getOptimalScalingToForType( const std::pair<double, double> &cutAway ) const {
		size_t volume = getImageSize()[0] * getImageSize()[1] * getImageSize()[2];
		TYPE maxTypeValue = std::numeric_limits<TYPE>::max();
		TYPE minTypeValue = std::numeric_limits<TYPE>::min();
		TYPE minImage = getInternMinMax().first->as<TYPE>();
		TYPE maxImage = getInternMinMax().second->as<TYPE>();
		TYPE extent = maxImage - minImage;
		//      double histogram[extent];
		double *histogram = ( double * ) calloc( extent + 1, sizeof( double ) );
		size_t stepSize = 2;
		size_t numberOfVoxels = volume / stepSize;
		TYPE *dataPtr = static_cast<TYPE *>( getImageVector().front()->getRawAddress().get() );

		//create the histogram
		for( size_t i = 0; i < volume; i += stepSize ) {
			histogram[dataPtr[i]]++;
		}

		//normalize histogram
		for( TYPE i = 0; i < extent; i++ ) {
			histogram[i] /= numberOfVoxels;
		}

		TYPE upperBorder = extent - 1;
		TYPE lowerBorder = 0;
		double sum = 0;

		while( sum < cutAway.second ) {
			sum += histogram[upperBorder--];

		}

		sum = 0;

		while ( sum < cutAway.first ) {
			sum += histogram[lowerBorder++];
		}

		std::pair<double, double> retPair;
		retPair.first = lowerBorder;
		retPair.second = ( float )maxTypeValue / float( upperBorder - lowerBorder );
		delete[] histogram;
		return retPair;
	}

	void setImageType( ImageType imageType ) { m_ImageProperties.imageType = imageType; }

private:


	size_t m_NumberOfTimeSteps;
	util::FixedVector<size_t, 4> m_ImageSize;
	util::PropertyMap m_PropMap;
	std::pair<util::ValueReference, util::ValueReference> m_MinMax;
	std::pair<util::ValueReference, util::ValueReference> m_InternMinMax;
	boost::shared_ptr<data::Image> m_Image;
	util::slist m_Filenames;
	size_t m_ID;
	std::pair<double, double> m_OptimalScalingPair;
	std::pair<double, double> m_CutAwayPair;
	ImageProperties m_ImageProperties;

	std::vector< ImagePointerType > m_ImageVector;
	std::vector< data::Chunk > m_ChunkVector;

	bool filterRelevantMetaInformation();

};

}
} //end namespace

#endif