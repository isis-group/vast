#ifndef IMAGEHOLDER_HPP
#define IMAGEHOLDER_HPP

#include <boost/foreach.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <vector>
#include <CoreUtils/propmap.hpp>
#include <DataStorage/image.hpp>
#include "common.hpp"
#include "color.hpp"
#include "widgetinterface.hpp"

namespace isis
{
namespace viewer
{
class WidgetInterface;
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
	boost::numeric::ublas::matrix<double> getNormalizedImageOrientation( bool transposed = false ) const;
	boost::numeric::ublas::matrix<double> getImageOrientation( bool transposed = false ) const;
	void addChangedAttribute( const std::string &attribute );
	bool removeChangedAttribute( const std::string &attribute );

	boost::weak_ptr<void>
	getImageWeakPointer( size_t timestep = 0 ) const {
		return getImageVector()[timestep]->getRawAddress();
	}

	util::slist getFileNames() const { return m_Filenames; }

	bool operator<( const ImageHolder &ref ) const { return m_ID < ref.getID(); }

	void checkVoxelCoords();

	void addWidget( WidgetInterface *widget ) { m_WidgetList.push_back( widget ); }
	void removeWidget( WidgetInterface *widget ) { m_WidgetList.erase( std::find( m_WidgetList.begin(), m_WidgetList.end(), widget ) ) ; }
	std::list< WidgetInterface * > getWidgetList() { return m_WidgetList; }
	
	void updateOrientation();
	
	/**offset, scaling**/
	std::pair<double, double> getOptimalScaling() const;
	
	util::ivector4 voxelCoords;
	util::fvector4 physicalCoords;
	util::fvector4 voxelSize;
	bool isVisible;
	bool isRGB;
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
	std::pair<util::ValueReference, util::ValueReference> minMax;
	std::pair<util::ValueReference, util::ValueReference> internMinMax;
	std::pair<double, double> optimalScalingOffset;
	boost::numeric::ublas::matrix<double> orientation;
	boost::numeric::ublas::matrix<double> latchedOrientation;
	unsigned short majorTypeID;
	
private:

	util::FixedVector<size_t, 4> m_ImageSize;
	util::PropertyMap m_PropMap;

	boost::shared_ptr<data::Image> m_Image;
	util::slist m_Filenames;
	size_t m_ID;
	std::pair<double, double> m_OptimalScalingPair;

	std::vector< ImagePointerType > m_ImageVector;
	std::vector< data::Chunk > m_ChunkVector;

	std::list<WidgetInterface *> m_WidgetList;
	
	template<typename TYPE>
	void copyImageToVector( const data::Image &image ) {
		data::ValuePtr<TYPE> imagePtr( ( TYPE * ) calloc( image.getVolume(), sizeof( TYPE ) ), image.getVolume() );
		LOG( Debug, verbose_info ) << "Needed memory: " << image.getVolume() * sizeof( TYPE ) / ( 1024.0 * 1024.0 ) << " mb.";
		data::TypedImage<TYPE> typedefImage ( image );
		static_cast<data::Image&>( typedefImage).copyToMem<TYPE>( &imagePtr[0], image.getVolume() );
		LOG( Debug, verbose_info ) << "Copied image to continuous memory space.";
		internMinMax = imagePtr.getMinMax();
		//splice the image in its volumes -> we get a vector of t volumes
		if( m_ImageSize[3] > 1 ) { //splicing is only necessary if we got more than 1 timestep
			m_ImageVector = imagePtr.splice( m_ImageSize[0] * m_ImageSize[1] * m_ImageSize[2] );
		} else {
			m_ImageVector.push_back( imagePtr );
		}

	
	}
	

};

}
} //end namespace

#endif