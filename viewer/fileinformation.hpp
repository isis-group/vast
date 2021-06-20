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
 * fileinformation.cpp
 *
 * Description:
 *
 *  Created on: Jan 26, 2012
 *      Author: tuerke
 ******************************************************************/

#pragma once

#include <string>
#include <filesystem>
#include <QSettings>

#include "imageholder.hpp"

namespace isis::viewer
{

class FileInformation : public std::filesystem::path
{
public:
	FileInformation()=delete;
	explicit FileInformation( const std::string &filename,
					 const util::istring &dialect = util::istring(),
					 const util::istring &readformat = util::istring(),
					 const std::string &widgetidentifier = std::string(),
					 const ImageHolder::ImageType &imagetype = ImageHolder::structural_image,
					 bool newensemble = true );

	[[nodiscard]] util::istring getDialect() const { return m_dialect; }
	[[nodiscard]] util::istring getReadFormat() const  { return m_readformat; }
	[[nodiscard]] std::string getWidgetIdentifier() const { return m_widgetIdentifier; }
	[[nodiscard]] bool isNewEnsemble() const { return m_newensemble; }
	[[nodiscard]] ImageHolder::ImageType getImageType() const { return m_imagetype; }

private:
	util::istring m_dialect;
	util::istring m_readformat;
	std::string m_widgetIdentifier;
	ImageHolder::ImageType m_imagetype;
	bool m_newensemble;

};

class FileInformationMap : public std::map<std::string, FileInformation>
{
public:
	FileInformationMap();
	void insertSave( const FileInformation &fileInfo );
	void writeFileInformationMap( QSettings *settings, const std::string &section );
	void readFileInfortmationMap( QSettings *settings, const std::string &section );
	void setLimit( size_t limit ) { m_limit = limit; }

private:
	size_t m_limit;
	std::list<std::string> m_lookup;
};

}
