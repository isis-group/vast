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
 * helpdialog.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "helpdialog.hpp"

#include <QMessageBox>
#include <boost/filesystem.hpp>

isis::viewer::ui::HelpDialog::HelpDialog( QWidget *parent )
	: QDialog( parent )
{
	m_Interface.setupUi( this );


}

void isis::viewer::ui::HelpDialog::showEvent ( QShowEvent *e )
{
	if( boost::filesystem::is_directory( boost::filesystem::path( std::string( VAST_DOC_PATH ) ) ) ) {
		std::string indexFile = std::string ( VAST_DOC_PATH ) + std::string( "/index.html" );
		m_Interface.helpView->load( QUrl( indexFile.c_str() ) );
	} else {
		m_Interface.helpView->load( QUrl( "http://isis-group.github.io/vast/doc/index.html" ) );
	}

	QDialog::showEvent( e );
}
