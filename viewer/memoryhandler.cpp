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
 * memoryhandler.cpp
 *
 * Description:
 *
 *  Created on: Mar 16, 2012
 *      Author: tuerke
 ******************************************************************/

#include "memoryhandler.hpp"

namespace isis
{
namespace viewer
{

util::ivector4 MemoryHandler::get32BitAlignedSize ( const util::ivector4 &origSize )
{
	util::ivector4 retSize;

	for ( util::ivector4::value_type i = 0; i < 4; i++ ) {
		const int m = origSize[i] % 4;

		if( m > 0 ) {
			retSize[i] = origSize[i] + 4 - m;
		} else {
			retSize[i] = origSize[i];
		}
	}

	return retSize;
}



}
} // end namespace