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
 * VoxelOp.hpp
 *
 * Description:
 *
 *  Created on: Apr 18, 2012
 *      Author: tuerke
 ******************************************************************/

#ifndef VAST_VOXELOP_HPP
#define VAST_VOXELOP_HPP

#include <DataStorage/image.hpp>

namespace isis
{
namespace viewer
{
namespace plugin
{
namespace _internal
{

class VoxelOp : public data::VoxelOp<double>
{
	mu::Parser parser;
	double voxBuff;
	util::FixedVector<double, 4> posBuff;
public:
	VoxelOp( std::string expr ) {
		parser.SetExpr( expr );
		parser.DefineVar( std::string( "vox" ), &voxBuff );
		parser.DefineVar( std::string( "pos_x" ), &posBuff[data::rowDim] );
		parser.DefineVar( std::string( "pos_y" ), &posBuff[data::columnDim] );
		parser.DefineVar( std::string( "pos_z" ), &posBuff[data::sliceDim] );
		parser.DefineVar( std::string( "pos_t" ), &posBuff[data::timeDim] );
	}
	bool operator()( double &vox, const isis::util::vector4<size_t>& pos ) {
		voxBuff = vox; //using parser.DefineVar every time would slow down the evaluation
		posBuff = pos;
		vox = parser.Eval();
		return true;
	}

};

}
}
}
}

#endif