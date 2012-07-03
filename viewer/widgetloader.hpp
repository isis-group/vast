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
 * widgetloader.hpp
 *
 * Description:
 *
 *  Created on: Feb 28, 2012
 *      Author: tuerke
 ******************************************************************/

#ifndef WIDGETLOADER_HPP
#define WIDGETLOADER_HPP

#include "widgetinterface.h"
#include <boost/filesystem.hpp>
#include <boost/regex.h>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <list>
#include <CoreUtils/singletons.hpp>

namespace isis
{
namespace viewer
{
namespace widget
{

class WidgetLoader
{
	friend class util::Singletons;
public:
	typedef boost::shared_ptr< WidgetInterface > WidgetInterfacePointer;

	typedef isis::viewer::widget::WidgetInterface* ( *loadWidget_func )() ;
	typedef QWidget* (  *loadOption_func )() ;
	typedef std::map<std::string, loadWidget_func > WidgetMapType;
	typedef std::map<std::string, loadOption_func > OptionDialogMapType;
	typedef std::map<std::string, const util::PropertyMap *> WidgetPropertyMapType;

	typedef std::list<std::string> PathsType;

	void addWidgetSearchPath( const std::string &path ) { m_WidgetSearchPaths.push_back( path ); }
	PathsType getWidgetSearchPaths() const { return m_WidgetSearchPaths; }

	WidgetMapType getWidgetMap() const { return widgetMap; }
	WidgetPropertyMapType getWidgetPropertyMap() const { return widgetPropertyMap; }
	OptionDialogMapType getOptionWidgetMap() const { return optionDialogMap; }

	static WidgetLoader &get();

protected:
	WidgetLoader();

	unsigned int findWidgets( std::list<std::string> paths );
	WidgetPropertyMapType widgetPropertyMap;
	OptionDialogMapType optionDialogMap;
	WidgetMapType widgetMap;

private:
	PathsType m_WidgetSearchPaths;
};





}
}
} //end namespace



#endif //WIDGETLOADER_HPP