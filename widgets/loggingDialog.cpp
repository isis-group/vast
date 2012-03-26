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
 * loggingDialog.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "loggingDialog.hpp"


isis::viewer::ui::LoggingDialog::LoggingDialog( QWidget *parent, isis::viewer::QViewerCore *core )
	: QDialog( parent ),
	  m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	m_Interface.errorCheck->setCheckState( Qt::Checked );
	m_Interface.warningCheck->setCheckState( Qt::Unchecked );
	m_Interface.infoCheck->setCheckState( Qt::Unchecked );
	m_Interface.verboseCheck->setCheckState( Qt::Unchecked );
	m_Interface.noticeCheck->setCheckState( Qt::Unchecked );
	m_Interface.checkRuntime->setChecked( true );
	m_Interface.checkDev->setChecked( false );
	connect( m_Interface.verboseCheck, SIGNAL( clicked() ), this, SLOT( synchronize() ) );
	connect( m_Interface.infoCheck, SIGNAL( clicked() ), this, SLOT( synchronize() ) );
	connect( m_Interface.warningCheck, SIGNAL( clicked() ), this, SLOT( synchronize() ) );
	connect( m_Interface.errorCheck, SIGNAL( clicked() ), this, SLOT( synchronize() ) );
	connect( m_Interface.noticeCheck, SIGNAL( clicked() ), this, SLOT( synchronize() ) );
	connect( m_Interface.checkDev, SIGNAL( clicked() ), this, SLOT( synchronize() ) );
	connect( m_Interface.checkRuntime, SIGNAL( clicked() ), this, SLOT( synchronize() ) );
	QPalette pal;
	pal.setBrush( QPalette::Base, Qt::red );
	m_Interface.errorCheck->setPalette( pal );
	pal.setBrush( QPalette::Base, QColor( 218, 165, 32 ) );
	m_Interface.warningCheck->setPalette( pal );
	pal.setBrush( QPalette::Base, Qt::darkBlue );
	m_Interface.infoCheck->setPalette( pal );
	pal.setBrush( QPalette::Base, QColor( 34, 139, 34 ) );
	m_Interface.noticeCheck->setPalette( pal );
}

void isis::viewer::ui::LoggingDialog::printLog ( std::list< isis::qt4::QMessage > messageList )
{
	BOOST_FOREACH( std::list< qt4::QMessage>::const_reference message, messageList ) {
		QListWidgetItem *item = new QListWidgetItem();
		std::stringstream logStream;
		logStream << message.m_module << "(" << message.time_str << ") -> " << message.message ;
		item->setText( logStream.str().c_str() );

		switch ( message.m_level ) {
		case error:

			if( m_Interface.errorCheck->checkState() == Qt::Checked ) {
				item->setTextColor( Qt::red );
				m_Interface.logList->addItem( item );
			}

			break;
		case warning:

			if( m_Interface.warningCheck->checkState() == Qt::Checked ) {
				item->setTextColor( QColor( 218, 165, 32 ) );
				m_Interface.logList->addItem( item );
			}

			break;
		case info:

			if( m_Interface.infoCheck->checkState() == Qt::Checked ) {
				item->setTextColor( Qt::darkBlue );
				m_Interface.logList->addItem( item );
			}

			break;
		case verbose_info:

			if( m_Interface.verboseCheck->checkState() == Qt::Checked ) {
				item->setTextColor( Qt::black );
				m_Interface.logList->addItem( item );
			}

			break;
		case notice:

			if( m_Interface.noticeCheck->checkState() == Qt::Checked ) {
				item->setTextColor( QColor( 34, 139, 34 ) );
				m_Interface.logList->addItem( item );
			}

			break;
		}

	}
}

void isis::viewer::ui::LoggingDialog::synchronize()
{
	m_Interface.logList->clear();

	if( m_Interface.checkDev->isChecked() ) {
		printLog( m_ViewerCore->getMessageLogDev() );
	}

	if( m_Interface.checkRuntime->isChecked() ) {
		printLog( m_ViewerCore->getMessageLog() );
	}


}
