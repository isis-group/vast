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
#include "widgetinterface.hpp"
#include "mainwindow.hpp"
#include <map>

namespace isis
{
namespace viewer
{

namespace widget
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
	enum ViewWidgetArragment { Default, InRow, InColumn };

	enum StatusTyp { Info, Warning, Error };
	struct ViewWidget {
		QDockWidget *dockWidget;
		QFrame *frame;
		QWidget *placeHolder;
		WidgetInterface *widgetImplementation;
		PlaneOrientation planeOrientation;
		std::string widgetType;
		bool operator==( const ViewWidget &other ) const { return widgetImplementation == other.widgetImplementation; }
	};
	typedef std::map<WidgetInterface *, ViewWidget > WidgetMap;
	typedef util::FixedVector<ViewWidget, 3> ViewWidgetEnsembleType;
	typedef std::list< ViewWidgetEnsembleType > ViewWidgetEnsembleListType;

	bool registerWidget( ViewWidget widget );
	const WidgetMap &getWidgets() const { return m_WidgetMap; }
	WidgetMap &getWidgets() { return m_WidgetMap; }

	void showMainWindow();
	const MainWindow *getMainWindow() const  { return m_MainWindow; }
	MainWindow *getMainWindow() { return m_MainWindow; }

	virtual ViewWidgetEnsembleType createViewWidgetEnsemble( const std::string &widgetType, bool show = true  );
	virtual ViewWidgetEnsembleType createViewWidgetEnsemble( const std::string &widgetType, boost::shared_ptr< ImageHolder > image, bool show = true );

	virtual void removeViewWidgetEnsemble( WidgetInterface *widgetImplementation );
	virtual void removeViewWidgetEnsemble( ViewWidgetEnsembleType ensemble );

	virtual ViewWidgetEnsembleType detachViewWidgetEnsemble( WidgetInterface *widgetImplementation );
	virtual ViewWidgetEnsembleType detachViewWidgetEnsemble( ViewWidgetEnsembleType ensemble );

	virtual void attachViewWidgetEnsemble( ViewWidgetEnsembleType ensemble );

	virtual ~UICore() {}

	virtual void setOptionPosition( OptionPosition pos = bottom );

	void setViewWidgetArrangement( ViewWidgetArragment arrangement ) { m_ViewWidgetArrangement = arrangement; }
	ViewWidgetArragment getViewWidgetArrangement() const { return m_ViewWidgetArrangement; }

	const ViewWidgetEnsembleListType &getEnsembleList() const { return m_EnsembleList; }
	ViewWidgetEnsembleListType &getEnsembleList() { return m_EnsembleList; }

	void setViewPlaneOrientation( PlaneOrientation orientation, bool visible );

	void rearrangeViewWidgets();

	QImage getScreenshot();

public Q_SLOTS:
	virtual void reloadPluginsToGUI();
	virtual void refreshUI();
	void showMessage( const qt4::QMessage &message );
	void showInformationAreas( bool );


protected:
	UICore( QViewerCore *core );

private:

	ViewWidget createViewWidget( const std::string &widgetType, PlaneOrientation planeOrientation );

	QDockWidget *createDockingEnsemble( QWidget *widget );

	QViewerCore *m_ViewerCore;
	MainWindow *m_MainWindow;
	ViewWidgetEnsembleListType m_EnsembleList;

	widget::VoxelInformationWidget *m_VoxelInformationWidget;
	widget::ImageStackWidget *m_ImageStackWidget;
	widget::SliderWidget *m_SliderWidget;

	ViewWidgetArragment m_ViewWidgetArrangement;

	unsigned short m_RowCount;

	WidgetMap m_WidgetMap;


};



}
}

#include "voxelInformationWidget.hpp"
#include "imageStackWidget.hpp"
#include "sliderwidget.hpp"

#endif