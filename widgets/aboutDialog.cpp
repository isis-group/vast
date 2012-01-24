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


isis::viewer::widget::AboutDialog::AboutDialog ( QWidget* parent, isis::viewer::QViewerCore* core ) 
	: QDialog ( parent ),
	m_ViewerCore( core ) 

{
	m_Interface.setupUi(this);
	QPixmap pixMap( m_ViewerCore->getOptionMap()->getPropertyAs<std::string>("vastSymbol").c_str() );
	float ratio = pixMap.height() / ( float )pixMap.width();
	m_Interface.vastSymbolLabel->setPixmap( QPixmap( 
		m_ViewerCore->getOptionMap()->getPropertyAs<std::string>("vastSymbol").c_str() ).scaled(pixMap.width() / ratio , m_Interface.vastSymbolLabel->height(), Qt::KeepAspectRatio ) );

}




