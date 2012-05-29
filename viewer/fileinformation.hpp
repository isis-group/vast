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

#ifndef VAST_FILEINFORMATION_HPP
#define VAST_FILEINFORMATION_HPP

#include <string>
#include <QSettings>

#include "imageholder.hpp"


namespace isis
{
namespace viewer
{

class FileInformation
{
public:
	FileInformation( const std::string &filename,
					 const util::istring &dialect = util::istring(),
					 const util::istring &readformat = util::istring(),
					 const std::string &widgetidentifier = std::string(),
					 const ImageHolder::ImageType &imagetype = ImageHolder::structural_image,
					 bool newensemble = true );

	std::string getFileName() const  { return m_filename; }
	std::string getCompletePath() const { return m_completePath; }
	util::istring getDialect() const { return m_dialect; }
	util::istring getReadFormat() const  { return m_readformat; }
	std::string getWidgetIdentifier() const { return m_widgetIdentifier; }
	bool isNewEnsemble() const { return m_newensemble; }
	ImageHolder::ImageType getImageType() const { return m_imagetype; }

	void setCompletePath( const std::string &cP ) { m_completePath = cP; }



private:
	FileInformation() {};
	std::string m_filename;
	std::string m_completePath;
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
}
#endif