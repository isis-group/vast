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
 * aboutDialog.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/

#ifndef ABOUTDIALOG_HPP
#define ABOUTDIALOG_HPP


#include "ui_aboutDialog.h"

#include "../viewer/qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace ui
{


class AboutDialog : public QDialog
{
	Q_OBJECT
	typedef std::map<std::string, std::string> AuthorMapType;
public:
	AboutDialog( QWidget *parent,  QViewerCore *core );

public Q_SLOTS:
	virtual void showEvent( QShowEvent * );
	void onAuthorClicked( QString );
	void sendEmailClicked();

private:
	QViewerCore *m_ViewerCore;
	Ui::aboutDialog m_Interface;

	AuthorMapType m_authorMap;
};



}
}
}





#endif
