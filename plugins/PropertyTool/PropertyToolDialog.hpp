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
 * PropertyToolDialog.hpp
 *
 * Description:
 *
 *  Created on: Jan 05, 2012
 *      Author: tuerke
 ******************************************************************/

#ifndef PROPERTYTOOLDIALOG_HPP
#define PROPERTYTOOLDIALOG_HPP

#include <QDialog>
#include <boost/assign.hpp>
#include "qviewercore.hpp"
#include "ui_propertyToolDialog.h"

namespace isis
{
namespace viewer
{
namespace plugin
{

namespace _internal
{


class FillChunkListThread : public QThread
{
	QComboBox *chunkList;
	std::vector<data::Chunk> chunks;
public:
    FillChunkListThread ( QObject *parent, QComboBox *chList )
		: QThread( parent ), chunkList( chList ) {}
	void setChunks( const std::vector<data::Chunk> &c ) { chunks = c; }
	void run() {
		for ( unsigned short i = 0; i < chunks.size() - 1; i++ ) {
			std::stringstream entry;
			entry << "Chunk " << i;
			chunkList->addItem( entry.str().c_str() );
		}
	}
};
	
template<typename TYPE> struct fromString {
	TYPE operator()( const std::string &string, bool &ok ) {
		ok = true;
		return util::Value<std::string>( string ).as<TYPE>();
	}
};


template<typename TYPE> struct fromString<util::vector4<TYPE> > {
	util::vector4<TYPE> operator()( const std::string &string, bool &ok ) {
		util::vector4<TYPE> ret;
		const std::list<std::string> elems = util::stringToList<std::string>( string, boost::regex( "[^\\.\\-0-9]" ) );

		if( elems.size() == 4 ) {
			ok = true;
			unsigned short index = 0;

			for( std::list<std::string>::const_iterator iter = elems.begin(); iter != elems.end(); iter++, index++ ) {
				boost::regex e( "^\\-?[[:digit:]]+\\.?[[:digit:]]*" );

				if( !iter->length() || !boost::regex_match( iter->c_str(),  e ) ) {
					ok = false;
				}

				ret[index] = util::Value<std::string>( *iter ).as<TYPE>();
			}
		} else {
			ok = false;
		}

		return ret;

	}
};


}

class TreePropMap : public util::PropertyMap
{
public:
	void fillTreeWidget( QTreeWidget *treeWidget );
	TreePropMap( const PropertyMap &propMap );
private:
	QTreeWidget *m_TreeWidget;
	void walkTree( QTreeWidgetItem *item, const TreePropMap &propMap, bool topLevel );
};



class PropertyToolDialog : public QDialog
{
	Q_OBJECT

public:
	PropertyToolDialog( QWidget *parent, QViewerCore *core );


public Q_SLOTS:
	void updateProperties();
	void selectionChanged( int );
	void onPropertyTreeClicked();
	void editRequested();
	QString getItemName( QTreeWidgetItem *item );
	virtual void showEvent( QShowEvent * );

private:
	Ui::propertyToolDialog m_Interface;
	QViewerCore *m_ViewerCore;
	_internal::FillChunkListThread *m_fillChunkListThread;

	void setIfHas( const std::string &name, QLabel *nameLabel, QLabel *propLabel, const boost::shared_ptr<data::Image> image );

	void buildUpTree( const util::PropertyMap &image );

	template<typename TYPE>
	TYPE fromString( const std::string &string, bool &ok )  {
		return _internal::fromString<TYPE>()( string, ok );
	}

	QString genericPrintPropertyValue( const std::string &name ) {
		return QString( static_cast<util::PropertyMap &>( *m_ViewerCore->getCurrentImage()->getISISImage() ).propertyValue( name.c_str() ).toString().c_str() );

	}
	template<typename TYPE>
	void checkAndSet( util::PropertyMap &map, const util::PropertyMap::PropPath &path, const QString &name ) {
		bool ok;
		TYPE value = fromString<TYPE>( name.toStdString(), ok );

		if( ok ) {
			map.setPropertyAs<TYPE>( path, value );
		} else {
			QMessageBox msgBox;
			msgBox.setText( "Could not parse input!" );
			msgBox.exec();
		}

	}


};


}
}
}

#endif
