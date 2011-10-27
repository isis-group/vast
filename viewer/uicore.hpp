#ifndef UICORE_HPP
#define UICORE_HPP

#include "qviewercore.hpp"
#include "mainwindow.hpp"
#include "qviewercore.hpp"

namespace isis {
namespace viewer {
namespace ui {
	
class UICore 
{
	struct WidgetEnsemble
	{
		QDockWidget *dockWidget;
		QFrame *frame;
		QWidgetImplementationBase *viewWidget;
		PlaneOrientation planeOrientation;
		std::string widgetType;
	};
	
public:
	typedef std::map<std::string, WidgetEnsemble> ViewWidgetMapType;
	UICore( QViewerCore *core );
	
	void showMainWindow();
	MainWindow *getMainWindow() const  { return m_MainWindow; }
	
	/**
	 * Append a widget to the main widget area.
	 *\param name Name of the widget.
	 *\param widgetType Type of the widget.
	 *\param planeOrientation The planeOrientation of the widget (axial, sagittal, coronal)
	 *\return returns if the widget was successfuly appended
	 */
	
	virtual bool appendWidget( const std::string &name, const std::string &widgetType, PlaneOrientation planeOrientation = axial );
	virtual bool appendWidget( const std::string &name, const std::string &widgetType, int row, int column, PlaneOrientation planeOrientation = axial );
	
	virtual bool appendWidgetRow( const std::string name, const std::string &widgetType );
	
	virtual bool removeWidget( const std::string &name );
	
	virtual void reloadPluginsToGUI();
	virtual ~UICore() {}
	
private:
	
	QDockWidget *createWidgetEnsemble( const std::string &name, const std::string& widgetType, PlaneOrientation planeOrientation );
	
	QViewerCore *m_Core;
	MainWindow *m_MainWindow;
	
	ViewWidgetMapType m_ViewWidgetMap;
	util::PropertyMap m_UICoreProperties;
};
	
	
	
}}}





#endif