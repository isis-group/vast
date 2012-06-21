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
 * keycommandsdialog.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "keycommandsdialog.hpp"


isis::viewer::ui::KeyCommandsDialog::KeyCommandsDialog( QWidget *parent )
	: QDialog( parent )
{
	m_Interface.setupUi( this );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Zoom in", "+ / Mousewheel" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Zoom out", "- / Mousewheel" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Center crosshair", "Space" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Propagate zooming", "P, Z" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Ignore orientation", "I, O" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Geometrical view", "G, V" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Show preferences", "S, P" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Show labels", "S, L" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Show crosshair", "S, C" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Find local minimum", "F, M, I" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Find local maximum", "F, M, A" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Show scaling dialog", "S, S" ) );
	//  m_KeyCommands.push_back(std::make_pair<std::string, std::string>("Show image properties", "S, I, P"));
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Reset Scaling", "R, S" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Auto Scaling", "A, S" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Open image", "Ctrl+O" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Save image", "Ctrl+S" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Save image as", "Ctrl+Shift+S" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Save all images", "Ctrl+Alt+Shift+S" ) );
	m_KeyCommands.push_back( std::make_pair<std::string, std::string>( "Exit program", "Ctrl+Q" ) );

	m_Interface.tableWidget->setRowCount( m_KeyCommands.size() );
	m_Interface.tableWidget->adjustSize();
	m_Interface.tableWidget->setColumnWidth( 0, m_Interface.tableWidget->width() );
	m_Interface.tableWidget->setColumnWidth( 1, m_Interface.tableWidget->width() / 2 );
	insertIntoTable();
}

void isis::viewer::ui::KeyCommandsDialog::insertIntoTable()
{
	unsigned short index = 0;
	typedef std::list< std::pair< std::string, std::string > >::const_reference CommandsRef;
	BOOST_FOREACH( CommandsRef commands, m_KeyCommands ) {
		m_Interface.tableWidget->setItem( index, 0, new QTableWidgetItem( commands.first.c_str() ) );
		m_Interface.tableWidget->setItem( index, 1, new QTableWidgetItem( commands.second.c_str() ) );
		index++;
	}
}