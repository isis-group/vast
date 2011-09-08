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


class Color
{

public:
	enum LookUpTableType { standard_grey_values, zmap_standard };
	static std::vector< util::fvector4 > getColorGradientRGB( const LookUpTableType &lutType, const size_t &numberOfEntries = 256 );
	
	static QVector<QRgb> getColorTable( const LookUpTableType &lutType );
};

}
}





#endif