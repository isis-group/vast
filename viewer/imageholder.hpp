#ifndef IMAGEHOLDER_HPP
#define IMAGEHOLDER_HPP

#include <boost/foreach.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <vector>
#include <CoreUtils/propmap.hpp>
#include <DataStorage/image.hpp>
#include "common.hpp"
#include "color.hpp"
#include "widgetImplementationBase.hpp"

namespace isis
{
namespace viewer
{
class QWidgetImplementationBase;
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


	void addWidget( QWidgetImplementationBase *widget ) { m_WidgetList.push_back( widget ); }
	void removeWidget( QWidgetImplementationBase *widget ) { m_WidgetList.erase( std::find( m_WidgetList.begin(), m_WidgetList.end(), widget ) ) ; }
	std::list< QWidgetImplementationBase * > getWidgetList() { return m_WidgetList; }

	bool isInsideImage( const util::ivector4 &voxelCoords ) const;
	bool isInsideImage( const util::fvector4 &physicalCoords ) const;

	/**offset, scaling**/
	template<typename TYPE>
	std::pair<double, double> getOptimalScalingToForType( const std::pair<double, double> &cutAway ) const {
		const size_t volume = getImageSize()[0] * getImageSize()[1] * getImageSize()[2];
		const TYPE maxTypeValue = std::numeric_limits<TYPE>::max();
		const TYPE minTypeValue = std::numeric_limits<TYPE>::min();
		const TYPE minImage = getInternMinMax().first->as<TYPE>();
		const TYPE maxImage = getInternMinMax().second->as<TYPE>();
		const TYPE extent = maxImage - minImage;
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

	util::ivector4 voxelCoords;
	util::fvector4 physicalCoords;
	bool isVisible;
	float opacity;
	util::ivector4 alignedSize32;
	double offset;
	double scaling;
	double extent;
	double lowerThreshold;
	double upperThreshold;
	std::string lut;
	ImageType imageType;
	InterpolationType interpolationType;

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

	std::vector< ImagePointerType > m_ImageVector;
	std::vector< data::Chunk > m_ChunkVector;

	bool filterRelevantMetaInformation();

	std::list<QWidgetImplementationBase *> m_WidgetList;
	

};

}
} //end namespace

#endif