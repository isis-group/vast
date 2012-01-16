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
#ifndef PYTHONSTDIOREDIRECT_HPP
#define PYTHONSTDIOREDIRECT_HPP


#include <iostream>
#include <boost/circular_buffer.hpp>

class PythonStdIoRedirect
{
public:
	typedef boost::circular_buffer<std::string> ContainerType;
	void Write( std::string const &str );
	static std::string GetOutput();

private:
	static ContainerType m_outputs; // must be static, otherwise output is missing
};



#endif