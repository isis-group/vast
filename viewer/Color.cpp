#include "Color.hpp"



namespace isis
{
namespace viewer
{

std::vector< util::fvector4 > Color::getColorGradientRGB( const Color::LookUpTableType &lutType, const size_t &numberOfEntries )
{

	std::vector< util::fvector4 > retRGBGradient(numberOfEntries);
	QColor rgbColor;
	switch(lutType) {
		case Color::zmap_standard: {
			for( size_t i = 0; i < (numberOfEntries / 2); i++) {
				if( i < (numberOfEntries / 4) ) {
					retRGBGradient[(numberOfEntries-1)-((numberOfEntries / 2)-i-1)] = util::fvector4(255, i*4, i*2);
					retRGBGradient[(numberOfEntries-1)-(i+(numberOfEntries / 2))] = util::fvector4(i*2, i*4, 255);
				} else {
					retRGBGradient[(numberOfEntries-1)-((numberOfEntries / 2)-i-1)] = util::fvector4(255, 255, i*2);
					retRGBGradient[(numberOfEntries-1)-(i+(numberOfEntries / 2))] = util::fvector4(i*2, 255, 255);
				}	
			}
			return retRGBGradient;
			break;
		}
	}
}


}
} // end namespace