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
#include "qviewercore.hpp"
#include "ui_propertyToolDialog.h"

namespace isis
{
namespace viewer
{
namespace plugin
{
 
namespace _internal {
 
template<typename VTYPE>
QString vectorToString( const VTYPE &vector ) {
    std::stringstream vStream;
    for( unsigned short i = 0; i < 4; i++ ) {
         vStream << vector[i] << " ";
    }
    return QString( vStream.str().c_str() );
}

template<typename TYPE> struct printPropertyValue{
QString operator()( const std::string &name, const util::PropertyMap &propMap ) {
    return QString::number( propMap.getPropertyAs<TYPE>(name.c_str()) );
}
};

template<typename TYPE> struct printPropertyValue<util::vector4<TYPE> >{
QString operator()( const std::string &name, const util::PropertyMap &propMap ) {
  util::dvector4 vec=propMap.getPropertyAs<util::dvector4>(name.c_str());
  return util::listToString(vec.begin(),vec.end()," ","","").c_str() ;
}
};

 
}
 
class TreePropMap : public util::PropertyMap 
{
public:
    void fillTreeWidget( QTreeWidget *treeWidget );
    TreePropMap( const PropertyMap& propMap ) { static_cast<util::PropertyMap&>( *this ) = propMap; }
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
	QString getItemName( QTreeWidgetItem* item );
    virtual void showEvent( QShowEvent * );
    
private:
    Ui::propertyToolDialog m_Interface;
    QViewerCore *m_ViewerCore;
    void setIfHas( const std::string &name, QLabel *nameLabel, QLabel *propLabel, const boost::shared_ptr<data::Image> image );
    void buildUpTree( const util::PropertyMap &image );
    
    template<typename TYPE> 
    QString printPropertyValue( const std::string &name )  {
        return _internal::printPropertyValue<TYPE>()(name, static_cast<util::PropertyMap&>( *m_ViewerCore->getCurrentImage()->getISISImage() ) );
    }

	QString genericPrintPropertyValue( const std::string &name ) {
		return QString( static_cast<util::PropertyMap&>( *m_ViewerCore->getCurrentImage()->getISISImage() ).propertyValue(name.c_str()).toString().c_str() );
		
	}




};

 
}}}

#endif
