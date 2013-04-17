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

	bool registerEnsembleComponent( WidgetEnsembleComponent::Pointer widget );
	const WidgetEnsembleComponent::Map &getWidgets() const { return m_WidgetMap; }
	WidgetEnsembleComponent::Map &getWidgets() { return m_WidgetMap; }

	void showMainWindow( const std::list<FileInformation> &fileList = std::list<FileInformation>() );
	const MainWindow *getMainWindow() const  { return m_MainWindow; }
	MainWindow *getMainWindow() { return m_MainWindow; }

	virtual WidgetEnsemble::Pointer createViewWidgetEnsemble( const std::string &widgetType, bool show = true  );
	virtual WidgetEnsemble::Pointer createViewWidgetEnsemble( const std::string &widgetType, ImageHolder::Pointer image, bool show = true );
	virtual WidgetEnsemble::Pointer createViewWidgetEnsemble( const std::string &widgetType, ImageHolder::Vector imageList, bool show = true );
	virtual WidgetEnsemble::Vector createViewWidgetEnsembleList( const std::string &widgetType, ImageHolder::Vector imageList, bool show = true );

	virtual void attachWidgetEnsemble( WidgetEnsemble::Pointer ensemble );

	virtual ~UICore() {}

	virtual void setOptionPosition( OptionPosition pos = bottom );

	const WidgetEnsemble::Vector &getEnsembleList() const { return m_EnsembleList; }
	WidgetEnsemble::Vector &getEnsembleList() { return m_EnsembleList; }

	WidgetEnsemble::Pointer getCurrentEnsemble() const;

	WidgetEnsemble::Pointer getEnsembleFromImage( const ImageHolder::Pointer ) const;

	void setViewPlaneOrientation( PlaneOrientation orientation, bool visible );

	QImage getScreenshot();

	///Closes all widget ensembles
	void closeAllWidgetEnsembles();
	///Closes the widget ensemble and returns its images
	ImageHolder::Vector closeWidgetEnsemble( WidgetEnsemble::Pointer ensemble );

	void openFromDropEvent( QDropEvent * );

public Q_SLOTS:
	virtual void reloadPluginsToGUI();
	virtual void refreshUI( const bool &mainwindow = true );
	virtual void refreshEnsembles();
	void showInformationAreas( bool );
	void toggleLoadingIcon( bool start, const QString &text = QString() );


protected:
	UICore( QViewerCore *core );

private:
	WidgetEnsembleComponent::Pointer createEnsembleComponent( const std::string &widgetIdentifier, PlaneOrientation planeOrientation, WidgetEnsemble::Pointer );

	QDockWidget *createDockingEnsemble( QWidget *widget );

	QViewerCore *m_ViewerCore;
	MainWindow *m_MainWindow;
	WidgetEnsemble::Vector m_EnsembleList;

	ui::VoxelInformationWidget *m_VoxelInformationWidget;
	ui::ImageStackWidget *m_ImageStackWidget;
	ui::SliderWidget *m_SliderWidget;


	WidgetEnsembleComponent::Map m_WidgetMap;


};



}
}

#include "../widgets/voxelInformationWidget.hpp"
#include "../widgets/imageStackWidget.hpp"
#include "../widgets/sliderwidget.hpp"
#include "../widgets/mainwindow.hpp"
#endif
