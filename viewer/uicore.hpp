#ifndef UICORE_HPP
#define UICORE_HPP

#include "qviewercore.hpp"
#include "mainwindow.hpp"
#include "qviewercore.hpp"

#include "voxelInformationWidget.hpp"
#include "imageStackWidget.hpp"

namespace isis {
namespace viewer {
namespace ui {
	
class UICore 
{
public:
	struct WidgetEnsemble
	{
		QDockWidget *dockWidget;
		QFrame *frame;
		QWidgetImplementationBase *viewWidget;
		PlaneOrientation planeOrientation;
		std::string widgetType;
		size_t ID;
	};
	typedef util::FixedVector<WidgetEnsemble, 3> RowType;
	typedef std::list< RowType > RowListType;
	typedef std::list<QWidgetImplementationBase *> ViewWidgetListType;
	typedef std::map< const QWidgetImplementationBase *, WidgetEnsemble > EnsembleMapType;
	UICore( QViewerCore *core );
	
	void showMainWindow();
	MainWindow *getMainWindow() const  { return m_MainWindow; }
	
	RowListType getRowList() const { return m_RowList; }
	
	/**
	 * Append a widget to the main widget area.
	 *\param name Name of the widget.
	 *\param widgetType Type of the widget.
	 *\param planeOrientation The planeOrientation of the widget (axial, sagittal, coronal)
	 *\return returns if the widget was successfuly appended
	 */
	
	virtual WidgetEnsemble appendWidget( const std::string &widgetType, PlaneOrientation planeOrientation = axial );
	virtual WidgetEnsemble appendWidget( const std::string &widgetType, int row, int column, PlaneOrientation planeOrientation = axial, Qt::Alignment = 0 );
	
	virtual RowType appendWidgetRow( const std::string &widgetType );
	
	virtual bool removeWidget( const QWidgetImplementationBase *widget );
	virtual ~UICore() {}
	
public Q_SLOTS:
	virtual void reloadPluginsToGUI();
	virtual void synchronize();

	
	
	
private:
	
	WidgetEnsemble createWidgetEnsemble( const std::string& widgetType, PlaneOrientation planeOrientation );
	
	QDockWidget *createDockingEnsemble( QWidget *widget );
	
	QViewerCore *m_Core;
	MainWindow *m_MainWindow;
	EnsembleMapType m_EnsembleMap;
	ViewWidgetListType m_ViewWidgetList;
	util::PropertyMap m_UICoreProperties;
	RowListType m_RowList;
	
	widget::VoxelInformationWidget *m_VoxelInformationWidget;
	widget::ImageStackWidget * m_ImageStackWidget;
	
};
	
	
	
}}}





#endif