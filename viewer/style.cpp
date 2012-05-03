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
 * style.cpp
 *
 * Description:
 *
 *  Created on: Apr 27, 2012
 *      Author: tuerke
 ******************************************************************/

#include "style.hpp"
#include "common.hpp"
#include <QFile>

namespace isis
{
namespace viewer
{
namespace style
{

Style::Style()
{
	addStyleSheet( std::string( ":/style/stylesheets/default" ), std::string( "default" ) );
	addStyleSheet( std::string( ":/style/stylesheets/fancy" ), std::string( "fancy" ) );
}

bool Style::addStyleSheet ( const std::string &path, const std::string &name )
{
	QFile lutFile( path.c_str() );
	lutFile.open( QIODevice::ReadOnly );

	if( !lutFile.isReadable() ) {
		LOG( Dev, warning ) << "The QResource \"" << path << "\" is not readable!";
		return false;
	}

	QString data( lutFile.readAll() );
	lutFile.close();
	m_StyleSheetMap[name] = data;
	return true;
}


QString Style::getStyleSheet ( const std::string &name )
{
	if( m_StyleSheetMap.find( name ) != m_StyleSheetMap.end() ) {
		return m_StyleSheetMap[name];
	} else {
		LOG( Dev, warning ) << "Trying to get stylesheet " << name << ". But there is no such stylesheet";
		return QString();
	}
}



}
}
}