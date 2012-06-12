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
 * PythonBridge.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "PythonBridge.hpp"

PythonBridge::PythonBridge( isis::viewer::QViewerCore *core )
	: m_ViewerCore( core )
{
	initializePython();

	exposeViewerCore();
	exposeImageHolder();
	exposeEnums();
}

void PythonBridge::exposeEnums()
{
	try {
		( *main_namespace )["image_type"] = enum_<isis::viewer::ImageHolder::ImageType>( "image_type" )
											.value( "STATISTICAL", isis::viewer::ImageHolder::statistical_image )
											.value( "STRUCTURAL", isis::viewer::ImageHolder::structural_image );
	} catch ( error_already_set ) {
		PyErr_Print();
	}
}


void PythonBridge::exposeViewerCore()
{
	( *main_namespace )["Core"] = class_<isis::viewer::QViewerCore, boost::noncopyable >( "Core" )
								  .def( "getVersion", &isis::viewer::QViewerCore::getVersion )
								  .staticmethod( "getVersion" )
								  .def( "addImage", &isis::viewer::ViewerCoreBase::addImage )
								  .def( "updateScene", &isis::viewer::QViewerCore::updateScene )
								  ;

}

void PythonBridge::exposeImageHolder()
{
	( *main_namespace )["ImageHolder"] = class_< isis::viewer::ImageHolder > ( "ImageHolder" );
}



void PythonBridge::initializePython()
{
	Py_Initialize();
	main_module.reset( new object(  handle<> ( borrowed( PyImport_AddModule( "__main__" ) ) ) ) );
	main_namespace.reset( new object ( main_module->attr( "__dict__" ) ) );
	handle<> ignored( ( PyRun_String( "from isis import util; from isis import data", Py_file_input, main_namespace->ptr(), main_namespace->ptr() ) ) );

	//redirect stdio
	( *main_namespace )["PythonStdIoRedirect"] = class_<PythonStdIoRedirect>( "PythonStdIoRedirect", init<>() )
			.def( "write", &PythonStdIoRedirect::Write );

	boost::python::import( "sys" ).attr( "stderr" ) = python_stdio_redirector;
	boost::python::import( "sys" ).attr( "stdout" ) = python_stdio_redirector;
}



std::string PythonBridge::run( const std::string &code ) const
{
	if( m_ViewerCore->hasImage() ) {
		( *main_namespace )["ci"] = m_ViewerCore->getCurrentImage()->getISISImage().get();
	}


	try {
		handle<> ignored( ( PyRun_String( code.c_str(), Py_file_input, main_namespace->ptr(), main_namespace->ptr() ) ) );
	} catch( error_already_set ) {
		PyErr_Print();
	}

	return python_stdio_redirector.GetOutput();
}
