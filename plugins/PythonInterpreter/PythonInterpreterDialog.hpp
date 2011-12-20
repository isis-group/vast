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
 * PythonInterpreterDialog.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef PYTHONINTERPRETERDIALOG_HPP
#define PYTHONINTERPRETERDIALOG_HPP

#include "ui_pythonInterpreter.h"
#include "qviewercore.hpp"
#include <QDialog>

#include "PythonBridge.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{


class PyhtonInterpreterDialog : public QDialog
{
	Q_OBJECT

public:
	PyhtonInterpreterDialog( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	virtual void run();

private:
	Ui::pythonDialog m_Interface;
	QViewerCore *m_ViewerCore;
	boost::scoped_ptr< PythonBridge > m_Bridge;
};


}
}
}


#endif