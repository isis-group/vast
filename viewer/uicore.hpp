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
 * uicore.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef UICORE_HPP
#define UICORE_HPP

#include <list>
#include "widgetensemble.hpp"
#include "widgetinterface.h"
#include "mainwindow.hpp"
#include <map>

namespace isis
{
namespace viewer
{

namespace ui
{
class VoxelInformationWidget;
class ImageStackWidget;
class SliderWidget;
}
class MainWindow;

class UICore : public QObject
{
	Q_OBJECT

public:
	friend class QViewerCore;

	enum OptionPosition { bottom, top, left, right, central11 };

	bool registerEnsembleComponent( WidgetEnsembleComponent widget );
	const WidgetEnsembleComponent::Map &getWidgets() const { return m_WidgetMap; }
	WidgetEnsembleComponent::Map &getWidgets() { return m_WidgetMap; }

	void showMainWindow();
	const MainWindow *getMainWindow() const  { return m_MainWindow; }
	MainWindow *getMainWindow() { return m_MainWindow; }

	virtual WidgetEnsemble createViewWidgetEnsemble( const std::string &widgetType, bool show = true  );
	virtual WidgetEnsemble createViewWidgetEnsemble( const std::string &widgetType, ImageHolder::Pointer image, bool show = true );
	virtual WidgetEnsemble createViewWidgetEnsemble( const std::string &widgetType, ImageHolder::List imageList, bool show = true );
	virtual WidgetEnsemble::List createViewWidgetEnsembleList( const std::string &widgetType, ImageHolder::List imageList, bool show = true );
	
	virtual void attachWidgetEnsemble( WidgetEnsemble ensemble );

	virtual bool attachImageToWidget( ImageHolder::Pointer image, widget::WidgetInterface *widget );
	virtual void attachImageToEnsemble(	ImageHolder::Pointer image, WidgetEnsemble ensemble );
	virtual void attachImageListToEnsemble( ImageHolder::List imageList, WidgetEnsemble ensemble );

	virtual ~UICore() {}

	virtual void setOptionPosition( OptionPosition pos = bottom );

	const WidgetEnsemble::List &getEnsembleList() const { return m_EnsembleList; }
	WidgetEnsemble::List &getEnsembleList() { return m_EnsembleList; }

	WidgetEnsemble getCurrentEnsemble() const;

	void setViewPlaneOrientation( PlaneOrientation orientation, bool visible );

	QImage getScreenshot();

	void removeAllWidgetEnsembles();

public Q_SLOTS:
	virtual void reloadPluginsToGUI();
	virtual void refreshUI( bool complete = true );
	void showInformationAreas( bool );


protected:
	UICore( QViewerCore *core );

private:

	virtual void removeWidgetEnsemble( WidgetEnsemble ensemble );

	WidgetEnsembleComponent createEnsembleComponent( const std::string &widgetIdentifier, PlaneOrientation planeOrientation );

	QDockWidget *createDockingEnsemble( QWidget *widget );

	QViewerCore *m_ViewerCore;
	MainWindow *m_MainWindow;
	WidgetEnsemble::List m_EnsembleList;

	ui::VoxelInformationWidget *m_VoxelInformationWidget;
	ui::ImageStackWidget *m_ImageStackWidget;
	ui::SliderWidget *m_SliderWidget;


	WidgetEnsembleComponent::Map m_WidgetMap;


};



}
}

#include "voxelInformationWidget.hpp"
#include "imageStackWidget.hpp"
#include "sliderwidget.hpp"

#endif