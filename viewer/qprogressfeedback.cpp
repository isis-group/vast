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
 * qprogressfeedback.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "qprogressfeedback.hpp"

namespace isis
{
namespace viewer
{

QProgressFeedback::QProgressFeedback()
	: m_ProgressBar( new QProgressBar() ),
	  m_CurrentVal( 0 )
{
	m_ProgressBar->setMaximumHeight( 20 );
	m_ProgressBar->setVisible( false );
}


void QProgressFeedback::show( size_t max, std::string header )
{
	if( !header.empty() ) {
		m_ProgressBar->setFormat( header.c_str() );
	}

	m_ProgressBar->setMaximum( max );
	m_ProgressBar->setMinimum( 0 );
	m_ProgressBar->show();
}

size_t QProgressFeedback::progress( const std::string /*message*/, size_t step )
{
	m_CurrentVal += step;
	m_ProgressBar->setValue( m_CurrentVal );
	return m_CurrentVal;
}

void QProgressFeedback::close()
{
	m_CurrentVal = 0;
	m_ProgressBar->setVisible( false );
}

size_t QProgressFeedback::getMax()
{
	return m_ProgressBar->maximum();
}

size_t QProgressFeedback::extend ( size_t by )
{
	const size_t newLen = m_ProgressBar->maximum() + by;
	m_ProgressBar->setMaximum( newLen );
	return newLen;
}


}
}