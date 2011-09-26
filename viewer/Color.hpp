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
	
	Color();
	const QVector<QRgb> &getColorTable() const { return m_ColorTable; }
	void setLutType( LookUpTableType type ) { m_LutType = type; }
	void setNumberOfElements( size_t nE ) { m_NumberOfElements = nE; }
	void update();
private:
	QVector<QRgb> m_ColorTable;
	LookUpTableType m_LutType;
	size_t m_NumberOfElements;
};

}
}





#endif