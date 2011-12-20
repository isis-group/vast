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
 * PythonBridge.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef PYTHONBRIDGE_HPP
#define PYTHONBRIDGE_HPP


#include <boost/python.hpp>
#include <qviewercore.hpp>
#include "PythonStdIORedirect.hpp"

using namespace boost::python;

class PythonBridge
{
public:
	PythonBridge( isis::viewer::QViewerCore *core );

	virtual ~PythonBridge() { Py_Finalize(); }

	boost::scoped_ptr< object > main_namespace;
	boost::scoped_ptr< object > main_module;
	std::string run( const std::string &code ) const;
private:
	void initializePython();
	isis::viewer::QViewerCore *m_ViewerCore;

	void exposeEnums();
	void exposeViewerCore();
	void exposeImageHolder();

	PythonStdIoRedirect python_stdio_redirector;

};




#endif