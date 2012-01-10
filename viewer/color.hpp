/****************************************************************
 *
 * <Copyright information>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Author: Erik Türke, tuerke@cbs.mpg.de
 *
 * color.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
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

	QIcon getIcon( const std::string &lutName, size_t w, size_t h, icon_type = both, bool flipped = false ) const;

	bool hasColormap( const std::string &name ) const;
	ColormapType getFallbackColormap() const;

	void adaptColorMapToImage( ImageHolder *image, bool split = true );



private:
	ColormapMapType m_ColormapMap;
};

}
}
}

#include "imageholder.hpp"


#endif