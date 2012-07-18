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
	boost::shared_ptr<data::Image> image;
	Ui::propertyToolDialog *interface;
public:
	FillChunkListThread ( QObject *parent, Ui::propertyToolDialog *pD )
		: QThread( parent ), interface( pD ) {}
	void setISISImage( boost::shared_ptr<data::Image> i ) { image = i; }
	void run() {
		const std::vector<data::Chunk> chunks = image->copyChunksToVector( false );
		interface->L_numberOfChunks->setVisible( chunks.size() > 1 );
		interface->numberOfChunks->setVisible( chunks.size() > 1 );
		interface->numberOfChunks->setText( QString::number( chunks.size() ) );

		for ( unsigned short i = 0; i < chunks.size() - 1; i++ ) {
			std::stringstream entry;
			entry << "Chunk " << i;
			interface->selection->addItem( entry.str().c_str() );
		}
	}
};

template<typename TYPE> struct fromString {
	TYPE operator()( const std::string &string, bool &ok ) {
		ok = true;
		return util::Value<std::string>( string ).as<TYPE>();
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
	virtual void closeEvent( QCloseEvent * );

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
