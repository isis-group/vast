#ifndef UICORE_HPP
#define UICORE_HPP

#include "qviewercore.hpp"
#include "mainwindow.hpp"
#include "qviewercore.hpp"

#include "voxelInformationWidget.hpp"
#include "imageStackWidget.hpp"

namespace isis {
namespace viewer {

class MainWindow;
	
class UICore 
{

public:
	typedef std::list<QWidgetImplementationBase * > WidgetList;
	
	enum OptionPosition { bottom, top, left, right, central11 };
	enum ViewWidgetArragment { Default, InRow, InColumn };
	struct ViewWidget
	{
		QDockWidget *dockWidget;
		QFrame *frame;
		QWidgetImplementationBase *widgetImplementation;
		PlaneOrientation planeOrientation;
		std::string widgetType;
	};
	
	typedef util::FixedVector<ViewWidget, 3> ViewWidgetEnsembleType;
	typedef std::list< ViewWidgetEnsembleType > ViewWidgetEnsembleListType;
	
	bool registerWidget( QWidgetImplementationBase *widget );
	const WidgetList &getWidgets() const { return m_WidgetList; }
	WidgetList &getWidgets() { return m_WidgetList; }
	
	void showMainWindow();
	const MainWindow *getMainWindow() const  { return m_MainWindow; }
	MainWindow *getMainWindow() { return m_MainWindow; }

	virtual ViewWidgetEnsembleType createViewWidgetEnsemble( const std::string &widgetType, bool show = true  );
	virtual ViewWidgetEnsembleType createViewWidgetEnsemble( const std::string &widgetType, boost::shared_ptr< ImageHolder > image, bool show = true );
	
	virtual ViewWidgetEnsembleType removeViewWidgetEnsemble( QWidgetImplementationBase* widgetImplementation );
	virtual ViewWidgetEnsembleType removeViewWidgetEnsemble( ViewWidgetEnsembleType ensemble );
	
	virtual ~UICore() {}
	
	virtual void setOptionPosition( OptionPosition pos = bottom );
	
	void setViewWidgetArrangement( ViewWidgetArragment arrangement ) { m_ViewWidgetArrangement = arrangement; }
	ViewWidgetArragment getViewWidgetArrangement() const { return m_ViewWidgetArrangement; }
	
public Q_SLOTS:
	virtual void reloadPluginsToGUI();
	virtual void synchronize();
	
	friend class QViewerCore;
protected:
	UICore( QViewerCore *core );
	
private:
	
	ViewWidget createViewWidget( const std::string& widgetType, PlaneOrientation planeOrientation );
	
	QDockWidget *createDockingEnsemble( QWidget *widget );
	
	QViewerCore *m_Core;
	MainWindow *m_MainWindow;
	ViewWidgetEnsembleListType m_EnsembleList;
	util::PropertyMap m_UICoreProperties;
	
	widget::VoxelInformationWidget *m_VoxelInformationWidget;
	widget::ImageStackWidget * m_ImageStackWidget;
	ViewWidgetArragment m_ViewWidgetArrangement;
	
	WidgetList m_WidgetList;
	
};
	
	
	
}}





#endif