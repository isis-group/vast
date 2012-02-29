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
 *  Created on: Jan 16, 2012
 *      Author: tuerke
 ******************************************************************/

#include "fileinformation.hpp"


isis::viewer::_internal::FileInformation::FileInformation ( const std::string& filename, const std::string& dialect, const std::string &widgetidentifier, const std::string& readformat, const isis::viewer::ImageHolder::ImageType& imagetype, bool newEnsemble )
	: m_filename( filename ),
	m_dialect( dialect ),
	m_readformat( readformat ),
	m_widgetIdentifier( widgetidentifier ),
	m_imagetype( imagetype ),
	m_newensemble( newEnsemble )
{}

void isis::viewer::_internal::FileInformationMap::writeFileInformationMap ( QSettings* settings, const std::string& section )
{
	QStringList fileNames;
	QStringList dialects;
	QStringList widgetidentifierList;
	QStringList readFormats;
	QList<QVariant> imageTypes;
    BOOST_FOREACH( const_reference elem, *this ) {
		fileNames.push_back( elem.second.getFileName().c_str() );
		widgetidentifierList.push_back( elem.second.getWidgetIdentifier().c_str() );
		dialects.push_back( elem.second.getDialect().c_str() );
		readFormats.push_back( elem.second.getReadFormat().c_str() );
		imageTypes.push_back( QVariant( static_cast<uint>( elem.second.getImageType() ) ) );
	}
	settings->beginGroup(section.c_str());
	settings->setValue( "fileNames", fileNames );
	settings->setValue( "readFormats", readFormats );
	settings->setValue( "dialects", dialects );
	settings->setValue( "widgetidentifierList", widgetidentifierList );
	settings->setValue( "imageTypes", imageTypes );
	settings->endGroup();
	settings->sync();
}

void isis::viewer::_internal::FileInformationMap::readFileInfortmationMap ( QSettings* settings, const std::string& section )
{
	settings->beginGroup( section.c_str() );
	QStringList fileNames = settings->value( "fileNames" ).toStringList();
	QStringList dialects = settings->value( "dialects" ).toStringList();
	QStringList readFormats = settings->value( "readFormats" ).toStringList();
	QStringList widgetidentifierList = settings->value( "widgetidentifierList").toStringList();
	QList<QVariant> imageTypes = settings->value( "imageTypes" ).toList();
	
    QStringList::iterator fileNameIter = fileNames.begin();
	QStringList::iterator dialectsIter = dialects.begin();
	QStringList::iterator readFormatsIter = readFormats.begin();
	QStringList::iterator widgetIdentifierIter = widgetidentifierList.begin();
	QList<QVariant>::iterator imageTypesIter = imageTypes.begin();
	while( fileNameIter != fileNames.end() ) {
		insertSave(  _internal::FileInformation( (*fileNameIter).toStdString(), (*dialectsIter).toStdString(), (*widgetIdentifierIter).toStdString(), (*readFormatsIter).toStdString(), static_cast<ImageHolder::ImageType>( (*imageTypesIter).toUInt() ), true ) );
		fileNameIter++;
		readFormatsIter++;
		widgetIdentifierIter++;
		dialectsIter++;
		imageTypesIter++;
	}
	settings->endGroup();
}

isis::viewer::_internal::FileInformationMap::FileInformationMap()
	:m_limit(10)
{

}

void isis::viewer::_internal::FileInformationMap::insertSave ( const isis::viewer::_internal::FileInformation& fileInfo )
{
	insert( std::make_pair<std::string, FileInformation>( fileInfo.getFileName(), fileInfo) );
	m_lookup.push_back( fileInfo.getFileName() );
	if( size() > m_limit ) {
		const std::string front = m_lookup.front();
		m_lookup.pop_front();
		erase( find( front ) );
	}
}



