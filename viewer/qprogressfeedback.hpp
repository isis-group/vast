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
 * qprogressfeedback.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef QPROGRESSFEEDBACK_HPP
#define QPROGRESSFEEDBACK_HPP

#include <CoreUtils/progressfeedback.hpp>
#include <QProgressBar>

namespace isis
{
namespace viewer
{

class QProgressFeedback : public util::ProgressFeedback
{
public:
	virtual void show( size_t max, std::string header = "" );
	virtual size_t progress( const std::string message = "", size_t step = 1 );
	virtual void close();
	virtual size_t getMax();
	virtual size_t extend( size_t by );

	QProgressFeedback();

	QProgressBar *getProgressBar() const { return m_ProgressBar; }
private:
	QProgressBar *m_ProgressBar;
	size_t m_CurrentVal;
};


}
}



#endif