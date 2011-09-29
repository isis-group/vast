#ifndef COLOR_HPP
#define COLOR_HPP

#include "common.hpp"
#include <QColor>
#include <QVector>
#include <QRgb>

namespace isis
{
namespace viewer
{

class ImageHolder;

class Color
{

public:
	enum LookUpTableType { standard_grey_values, zmap_standard };
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


#include "ImageHolder.hpp"


#endif