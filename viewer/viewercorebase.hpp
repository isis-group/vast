#ifndef VIEWERCOREBASE_HPP
#define VIEWERCOREBASE_HPP

#include "datacontainer.hpp"
#include <map>
#include "pluginloader.hpp"


#ifdef _OPENMP
#include <omp.h>
#endif /*_OPENMP*/

namespace isis
{
namespace viewer
{

class ViewerCoreBase
{

public:

	enum Mode { standard, zmap };
	
	typedef std::list<boost::shared_ptr< plugin::PluginInterface > > PluginListType;
	ViewerCoreBase( );

	std::string getVersion() const;

	virtual ImageHolder::ImageListType addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );
	virtual void setImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );
	virtual boost::shared_ptr<ImageHolder> addImage( const data::Image &image, const ImageHolder::ImageType &imageType );

	void setCurrentImage( const boost::shared_ptr<ImageHolder> image ) { m_CurrentImage = image; }

	boost::shared_ptr<ImageHolder> getCurrentImage();

	boost::shared_ptr<ImageHolder> getCurrentAnatomicalRefernce() const { return m_CurrentAnatomicalReference; }
	
	const DataContainer &getDataContainer() const { return m_DataContainer; }
	DataContainer &getDataContainer() { return m_DataContainer; }
	ImageHolder::ImageListType getImageList() const { return m_ImageList; }

	boost::shared_ptr<util::PropertyMap>  getOptionMap() { return m_OptionsMap; }

	bool hasImage() const { return getDataContainer().size(); }
	
	void setMode( Mode mode ) { m_Mode = mode; }
	Mode getMode() const { return m_Mode; }
	

private:
	//this is the container which actually holds all the images
	DataContainer m_DataContainer;
	boost::shared_ptr<ImageHolder>  m_CurrentImage;
	void setCommonViewerOptions();


protected:
	boost::shared_ptr<util::PropertyMap> m_OptionsMap;
	//additional imagelist for finding purpose
	ImageHolder::ImageListType m_ImageList;
	
	boost::shared_ptr<ImageHolder> m_CurrentAnatomicalReference;
	Mode m_Mode;



};




}
} // end namespace



#endif