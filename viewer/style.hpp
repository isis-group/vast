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
 * Author: Erik Tuerke, tuerke@cbs.mpg.de
 *
 * style.hpp
 *
 * Description:
 *
 *  Created on: Apr 27, 2012
 *      Author: tuerke
 ******************************************************************/
#ifndef VAST_STYLE_HPP
#define VAST_STYLE_HPP

#include <QString>
#include <map>

namespace isis
{
namespace viewer
{
namespace style
{


class Style
{

public:
	Style();

	bool addStyleSheet( const std::string &path, const std::string &name );
	QString getStyleSheet( const std::string &name );

private:
	std::map< std::string, QString > m_StyleSheetMap;
};


}
}
}




#endif