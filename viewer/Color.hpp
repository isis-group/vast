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

	static QVector<QRgb> getColorTable( const LookUpTableType &lutType, size_t numberOfElems = 256 );
};

}
}





#endif