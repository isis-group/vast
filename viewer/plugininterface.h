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
 * plugininterface.h
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#ifdef __cplusplus
#include <iostream>
#include <QObject>
#include <QIcon>
#include <QKeySequence>

namespace isis
{
namespace viewer
{

class QViewerCore;

namespace plugin
{


/// Base class for all plugins that are aiming to do any operation on the images
class PluginInterface
{
public:
	PluginInterface() {};
	void setViewerCore( isis::viewer::QViewerCore *core ) { viewerCore = core; }
	void setParentWidget( QWidget *p ) { parentWidget = p; }

	/**
	 * Calls the plugin. This function should execute the plugin
	 * (e.g. showing a dialog or directly executing an operation)
	 */
	virtual bool call() = 0;
	/**
	 * This function returns the name of the plugin.
	 * A name for instance can look like "category/subcategory/pluginname
	 * and automaticall will be in the respective submenu (plugins/category/subcategory).
	 */
	virtual std::string getName() = 0;

	///returns a description of the plugin
	virtual std::string getDescription() = 0;

	///returns the tooltip of the plugin
	virtual std::string getTooltip() = 0;

	///returns the shortcut that can be used to call the plugin
	virtual QKeySequence getShortcut() { return QKeySequence(); }

	///returns if the plugin uses a gui
	virtual bool isGUI() = 0;

	///returns the string pointing to the toolbarIcon. If this string is empty the plugin will not be placed in the toolbar.
	virtual QIcon *getToolbarIcon() { return new QIcon(); }
	virtual ~PluginInterface() {}

	std::string plugin_file;
protected:
	QViewerCore *viewerCore;
	QWidget *parentWidget;


};


}
}
}

#else
typedef struct PluginInterface PluginInterface;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__STDC__) || defined(__cplusplus)
#ifdef WIN32
	extern __declspec( dllexport ) isis::viewer::plugin::PluginInterface *loadPlugin();
#else
	extern isis::viewer::plugin::PluginInterface *loadPlugin();
#endif
#else
#ifdef WIN32
	extern __declspec( dllexport ) PluginInterface *loadPlugin();
#else
	extern PluginInterface *loadPlugin();
#endif
#endif

#ifdef __cplusplus
}
#endif



#endif