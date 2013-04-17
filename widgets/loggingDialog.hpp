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
 * loggingDialog.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef LOGGINGDIALOG_HPP
#define LOGGINGDIALOG_HPP
#include <QDialog>

#include "ui_loggingDialog.h"
#include "../viewer/qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace ui
{

class LoggingDialog : public QDialog
{
	Q_OBJECT
public:
	LoggingDialog( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	void synchronize();
	void printLog( std::list<qt4::QMessage> );

private:
	Ui::logDialog m_Interface;
	QViewerCore *m_ViewerCore;
};


}
}
}


#endif
