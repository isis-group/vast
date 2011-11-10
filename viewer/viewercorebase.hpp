#ifndef VIEWERCOREBASE_HPP
#define VIEWERCOREBASE_HPP

#include "datacontainer.hpp"
#include <map>
#include "pluginloader.hpp"

namespace isis
{
namespace viewer
{

class ViewerCoreBase
{

public:

	typedef std::list<boost::shared_ptr< plugin::PluginInterface > > PluginListType;
	ViewerCoreBase( );

	std::string getVersion() const;

	virtual ImageHolder::ImageListType addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );
	virtual void setImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );
	virtual boost::shared_ptr<ImageHolder> addImage( const data::Image &image, const ImageHolder::ImageType &imageType );

	void setCurrentImage( const boost::shared_ptr<ImageHolder> image ) { m_CurrentImage = image; }

	boost::shared_ptr<ImageHolder> getCurrentImage() { return m_CurrentImage; }
	size_t getCurrentTimestep() const { return m_CurrentTimestep; }
	bool setCurrentTimestep( size_t timestep ) {
		if(  m_CurrentImage->getImageSize()[3] > timestep ) {
			m_CurrentTimestep = timestep;
			return true;
		} else { return false; }
	}

	const DataContainer &getDataContainer() const { return m_DataContainer; }
	DataContainer &getDataContainer() { return m_DataContainer; }
	ImageHolder::ImageListType getImageList() const { return m_ImageList; }

	boost::shared_ptr<util::PropertyMap>  getOptionMap() { return m_OptionsMap; }

	void setCoordsTransformation( const util::fvector4 &transformation ) { m_VoxelTransformation = transformation; }
	util::fvector4 getTransformedCoords( const util::fvector4 &coords ) const;

	color::Color *getColorHandler() const { return m_ColorHandler; }

	bool hasImage() const { return getDataContainer().size(); }

private:
	//this is the container which actually holds all the images
	DataContainer m_DataContainer;
	boost::shared_ptr<ImageHolder>  m_CurrentImage;
	size_t m_CurrentTimestep;
	util::fvector4 m_VoxelTransformation;
	color::Color *m_ColorHandler;
	void setCommonViewerOptions();


protected:
	boost::shared_ptr<util::PropertyMap> m_OptionsMap;
	//additional imagelist for finding purpose
	ImageHolder::ImageListType m_ImageList;



};




}
} // end namespace



#endif