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

#ifndef PROPERTYTOOLIALOG_HPP
#define PROPERTYTOOLIALOG_HPP

#include <QDialog>
#include "qviewercore.hpp"
#include "ui_propertyToolDialog.h"

namespace isis
{
namespace viewer
{
namespace plugin
{
 
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
    virtual void showEvent( QShowEvent * );
    
private:
    Ui::propertyToolDialog m_Interface;
    QViewerCore *m_ViewerCore;
    void setIfHas( const std::string &name, QLabel *nameLabel, QLabel *propLabel, const boost::shared_ptr<data::Image> image );
    void buildUpTree( const util::PropertyMap &image );

};
 
}}}

#endif
