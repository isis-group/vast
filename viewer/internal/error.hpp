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
 * error.hpp
 *
 * Description:
 *
 *  Created on: Jan 26, 2012
 *      Author: tuerke
 ******************************************************************/

#ifndef VAST_ERROR_HPP
#define VAST_ERROR_HPP

#include <iostream>
#include <fstream>
#include "common.hpp"
#include <Adapter/qtapplication.hpp>

namespace isis
{
namespace viewer
{
namespace error
{

void sigsegv ( int exit_code )
{
#ifndef WIN32

	std::ofstream logFile ( getCrashLogFilePath().c_str(), std::ofstream::binary );
	const qt4::QMessageList &messageList = util::Singletons::get<qt4::QMessageList, 10>();

	for( qt4::QMessageList::const_iterator iter = messageList.begin(); iter != messageList.end(); iter++ ) {
		logFile << iter->m_module << "(" << iter->time_str << ") [" << iter->m_file << ":" << iter->m_line << "] " << iter->message << std::endl;
	}

	logFile.close();
	std::cout << "Seems like vast crashed :-( . Logfile was written to " << getCrashLogFilePath() << std::endl;

#else
#warning implement me!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#endif
	exit( exit_code );
}
}
}
}
#endif