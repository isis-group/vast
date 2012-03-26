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
 * settings.hpp
 *
 * Description:
 *
 *  Created on: Mar 14, 2012
 *      Author: tuerke
 ******************************************************************/

#ifndef VAST_DEFAULT_SETTINGS_HPP
#define VAST_DEFAULT_SETTINGS_HPP

#include <CoreUtils/propmap.hpp>
#include <QSettings>
#include "fileinformation.hpp"

namespace isis
{
namespace viewer
{

class Settings : public util::PropertyMap
{
public:
	Settings();
	void initializeWithDefaultSettings();
	void save();
	void load();

	QSettings *getQSettings() { return m_QSettings; }
	const QSettings *getQSettings() const { return m_QSettings; }

	FileInformationMap &getFavoriteFiles() { return m_FavFiles; }
	FileInformationMap &getRecentFiles() { return m_RecentFiles; }

private:
	QSettings *m_QSettings;
	FileInformationMap m_RecentFiles;
	FileInformationMap m_FavFiles;
};



}
}



#endif //VAST_DEFAULT_SETTINGS_HPP