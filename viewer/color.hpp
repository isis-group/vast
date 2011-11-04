#ifndef COLOR_HPP
#define COLOR_HPP

#include "common.hpp"
#include <QColor>
#include <QVector>
#include <QIcon>
#include <QRgb>
#include <boost/regex.h>

namespace isis
{
namespace viewer
{

class ImageHolder;

namespace color
{


class Color
{
public:
	typedef QVector<QRgb> ColormapType;
	typedef std::map<std::string, ColormapType > ColormapMapType;
	enum icon_type { lower_half, upper_half, both };

	bool addColormap( const std::string &path, const boost::regex &separator
					  = boost::regex( "[[:space:]]+" ) );

	ColormapMapType getColormapMap() const { return m_ColormapMap; }
	void initStandardColormaps();

	QIcon getIcon( const std::string &lutName, size_t w, size_t h, icon_type = both ) const;

	bool hasColormap( const std::string &name ) const;
	ColormapType getFallbackColormap() const;

	static ColormapType adaptColorMapToImage( ColormapType colorMap, const boost::shared_ptr<ImageHolder> image, bool split = true );


private:
	ColormapMapType m_ColormapMap;
};

}
}
}

#include "imageholder.hpp"


#endif