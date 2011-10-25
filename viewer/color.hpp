#ifndef COLOR_HPP
#define COLOR_HPP

#include "common.hpp"
#include <QColor>
#include <QVector>
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
	typedef std::map<std::string, QVector<QRgb> > LUTMapType;
	
	Color();
	
	bool addLUTFromResource( const std::string &resPath, const boost::regex &separator 
		= boost::regex("[[:space:]]") );
	bool addLutFromFile( const std::string &lutPath, const boost::regex &separator 
		= boost::regex("[[:space:]]") );
	
private:
	bool addLUTFromStringList( std::list<std::string> stringList, const std::string &lutPath, const boost::regex &separator ); 
	LUTMapType m_LutMap;
	
	
	
};

//*********************************************************OLD

}
class Color {
public:
	enum LookUpTableType { standard_grey_values = 0, zmap_standard, colormap1, colormap2, colormap3, colormap4, colormap5, colormap6, colormap7, colormap8, colormap9, colormap10, colormap11, colormap12 };
	typedef QVector<QRgb> ColorMapType;

	Color();
	ColorMapType getColorTable() const { return m_ColorTable; }
	void setLutType( LookUpTableType type ) { m_LutType = type; }
	void setNumberOfElements( size_t nE ) { m_NumberOfElements = nE; }
	void setOffsetAndScaling( std::pair<double, double> offsetScaling ) { m_OffsetScaling = offsetScaling; }
	void resetOffsetAndScaling();
	void setImage( const boost::shared_ptr<ImageHolder> image ) { m_ImageHolder = image; }
	void setOmitZeros( bool omit ) { m_OmitZeros = omit; }

	void update();
private:
	ColorMapType m_ColorTable;
	LookUpTableType m_LutType;
	size_t m_NumberOfElements;
	std::pair<double, double> m_OffsetScaling;
	boost::shared_ptr<ImageHolder> m_ImageHolder;
	bool m_OmitZeros;

};


}
}

#include "imageholder.hpp"


#endif