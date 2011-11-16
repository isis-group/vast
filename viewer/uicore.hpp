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

class UICore
{

public:


	enum OptionPosition { bottom, top, left, right, central11 };
	enum ViewWidgetArragment { Default, InRow, InColumn };

	enum StatusTyp { Info, Warning, Error };
	struct ViewWidget {
		QDockWidget *dockWidget;
		QFrame *frame;
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

	ViewWidgetEnsembleListType getEnsembleList() const { return m_EnsembleList; }

	void rearrangeViewWidgets();

public Q_SLOTS:
	virtual void reloadPluginsToGUI();
	virtual void refreshUI();
	void showMessage( const qt4::QMessage &message );
	void setShowWorkingLabel( const std::string &message, bool show = true );

	friend class QViewerCore;
protected:
	UICore( QViewerCore *core );

private:

	ViewWidget createViewWidget( const std::string &widgetType, PlaneOrientation planeOrientation );

	QDockWidget *createDockingEnsemble( QWidget *widget );

	QViewerCore *m_ViewerCore;
	MainWindow *m_MainWindow;
	ViewWidgetEnsembleListType m_EnsembleList;

	widget::VoxelInformationWidget *m_VoxelInformationWidget;
	QDockWidget *m_VoxelInformationDockWidget;
	widget::ImageStackWidget *m_ImageStackWidget;
	QDockWidget *m_ImageStackDockWidget;
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