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
 * RegistrationDialog.hpp
 *
 * Description:
 *
 *  Created on: Apr 17, 2012
 *      Author: tuerke
 ******************************************************************/

#ifndef VAST_ITKREGISTRATIONDIALOG_HPP
#define VAST_ITKREGISTRATIONDIALOG_HPP

#include "ui_itkRegistrationDialog.h"
#include "qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{

class RegistrationDialog : public QDialog
{
	Q_OBJECT
public:
	RegistrationDialog( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	void showEvent( QShowEvent * );
	void startRegistration();

private:
	QViewerCore *m_ViewerCore;
	Ui::RegistrationDialog m_Interface;

};

}
}
}

#endif