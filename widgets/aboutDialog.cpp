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
 * aboutDialog.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/

#include "aboutDialog.hpp"


isis::viewer::ui::AboutDialog::AboutDialog ( QWidget *parent, isis::viewer::QViewerCore *core )	: QDialog ( parent ), m_ViewerCore( core )
{
	m_Interface.setupUi( this );

	connect( m_Interface.authorsList, SIGNAL( currentTextChanged( QString ) ), this, SLOT( onAuthorClicked( QString ) ) );
	connect( m_Interface.sendMailButton, SIGNAL( pressed() ), this, SLOT( sendEmailClicked() ) );
	m_Interface.sendMailButton->setIcon( QIcon( ":/common/icon_email.gif" ) );

	m_Interface.sendMailButton->setEnabled( false );

	QPixmap pixMap( m_ViewerCore->getSettings()->getPropertyAs<std::string>( "vastSymbol" ).c_str() );
	const float ratio = pixMap.height() / ( float )pixMap.width();
	m_Interface.vastSymbolLabel->setPixmap( QPixmap(
			m_ViewerCore->getSettings()->getPropertyAs<std::string>( "vastSymbol" ).c_str() ).scaled( 200 / ratio , m_Interface.vastSymbolLabel->height(), Qt::KeepAspectRatio ) );
	QPalette pal;
	pal.setColor( QPalette::Text, Qt::blue );
	m_Interface.contactEdit->setPalette( pal );

	m_authorMap["Erik Tuerke"] = std::string( "tuerke@cbs.mpg.de" );
	m_authorMap["Enrico Reimer"] = std::string( "reimer@cbs.mpg.de" );

}

void isis::viewer::ui::AboutDialog::showEvent( QShowEvent * )
{
	m_Interface.labelCopyright->setText( m_ViewerCore->getSettings()->getPropertyAs<std::string>( "copyright" ).c_str() );
	m_Interface.labelVersion->setText( m_ViewerCore->getVersion().c_str() );
	m_Interface.authorsList->clear();
	BOOST_FOREACH( AboutDialog::AuthorMapType::const_reference author, m_authorMap ) {
		m_Interface.authorsList->addItem( author.first.c_str() );
	}
	m_Interface.labelISISVersion->setText( util::Application::getCoreVersion().c_str() );
	m_Interface.authorsList->setCurrentRow( 0 );
}

void isis::viewer::ui::AboutDialog::onAuthorClicked ( QString author )
{
	m_Interface.contactEdit->setText( m_authorMap.at( author.toStdString() ).c_str() );
	m_Interface.sendMailButton->setEnabled( true );
}

void isis::viewer::ui::AboutDialog::sendEmailClicked()
{
	QDesktopServices::openUrl( QUrl( QString( "mailto:" ) + m_Interface.contactEdit->text() ) );
}

