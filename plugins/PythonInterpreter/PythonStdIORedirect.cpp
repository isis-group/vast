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
 * PythonStdIORedirect.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "PythonStdIORedirect.hpp"

#include <sstream>

PythonStdIoRedirect::ContainerType PythonStdIoRedirect::m_outputs;

void PythonStdIoRedirect::Write( const std::string &str )
{
	if ( m_outputs.capacity() < 100 ) {
		m_outputs.resize( 100 );
	}

	m_outputs.push_back( str );
}

std::string PythonStdIoRedirect::GetOutput()
{
	std::string ret;
	std::stringstream ss;

	for( boost::circular_buffer<std::string>::const_iterator it = m_outputs.begin();
		 it != m_outputs.end();
		 it++ ) {
		ss << *it;
	}

	m_outputs.clear();
	ret =  ss.str();
	return ret;
}
