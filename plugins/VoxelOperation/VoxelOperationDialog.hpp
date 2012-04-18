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
 * VoxelOperationDialog.hpp
 *
 * Description:
 *
 *  Created on: Apr 18, 2012
 *      Author: tuerke
 ******************************************************************/

#ifndef VAST_VOXELOPERATIONDIALOG_HPP
#define VAST_VOXELOPERATIONDIALOG_HPP

#include <QDialog>
#include <boost/assign.hpp>
#include "qviewercore.hpp"
#include "ui_voxelOperation.h"
#include <muParser.h>
#include "VoxelOp.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{

class VoxelOperationDialog : public QDialog
{
	Q_OBJECT

public:
	VoxelOperationDialog( QWidget *parent, QViewerCore *core );


public Q_SLOTS:
	void calculatePressed();
	void helpPressed();

private:
	Ui::voxelOperationDialog m_Interface;
	QViewerCore *m_ViewerCore;

};


}
}
}

#endif
