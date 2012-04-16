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
 * helpDialog.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef HELPDIALOG_HPP
#define HELPDIALOG_HPP

#include "ui_helpDialog.h"

namespace isis
{
namespace viewer
{
namespace ui
{


class HelpDialog : public QDialog
{
	Q_OBJECT
public:
	HelpDialog( QWidget *parent );
public Q_SLOTS:
	void showEvent( QShowEvent * );

private:
	Ui::helpDialog m_Interface;
};


}
}
}


#endif