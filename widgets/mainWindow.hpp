// /*
//  * MainWindow.cpp
//  *
//  *  Created on: Nov 3, 2010
//  *      Author: tuerke
//  */
//
// #ifndef MAINWINDOW_CPP_
// #define MAINWINDOW_CPP_
//
// #include "mainWindowBase.hpp"
// #include "qviewercore.hpp"
//
// #include "QImageWidgetImplementation.hpp"
//
// #include <CoreUtils/type.hpp>
//
// namespace isis
// {
// namespace viewer
// {
//
// class MainWindow : public MainWindowBase
// {
//
//  Q_OBJECT
// protected:
//  enum State { single, splitted } m_State;
// public:
//  MainWindow( QViewerCore *core, WidgetType wType = type_gl );
//
//  State getState() const { return m_State; }
//  void setWidgetType( WidgetType type ) { m_WidgetType = type; };
//
// private:
//
//  QViewerCore *m_ViewerCore;
//  void setInitialState();
//
// public Q_SLOTS:
//  void closeEvent( QCloseEvent * );
//  void loadSettings();
//  void saveSettings();
//  void physicalCoordsChanged( util::fvector4 );
//  void voxelCoordsChanged( util::ivector4 );
//  void exitProgram();
//  void imagesChanged( DataContainer );
//  void checkImageStack( QListWidgetItem *item );
//  void openImageAs( ImageHolder::ImageType type );
//  void openDICOMDir();
//  void contextMenuImageStack( QPoint );
//
//  void triggeredMakeCurrentImageZmap( bool );
//
//  void interpolationChanged( int );
//  void currentImageChanged( int );
//  void updateInterfaceValues();
//  void setVoxelPosition();
//  void setPhysicalPosition();
//
//  void callPlugin( QString );
//
//  void reloadPluginsToGUI();
//
//  void upperThresholdChanged( int );
//  void lowerThresholdChanged( int );
//  void opacityChanged( int );
//
//  void assembleViewInRows( );
//
//
// protected:
//  WidgetType m_WidgetType;
//
//  QWidgetImplementationBase *m_AxialWidget;
//  QWidgetImplementationBase *m_CoronalWidget;
//  QWidgetImplementationBase *m_SagittalWidget;
//  QWidgetImplementationBase *m_MasterWidget;
//
//  QAction *actionMakeCurrent;
//  QAction *actionAsZMap;
//
//  QToolBar *m_Toolbar;
//
//  QWidgetImplementationBase *createView( QDockWidget *widget, PlaneOrientation orientation, unsigned short index );
//
//  template<typename TYPE> void displayIntensity( util::ivector4 coords ) {
//      util::Value<TYPE> vIntensity ( m_ViewerCore->getCurrentImage()->getISISImage()->voxel<TYPE>( coords[0], coords[1], coords[2], coords[3] ) );
//      double intensity = roundNumber<double>( vIntensity, 2 );
//      ui.intensity_value->display( intensity );
//      ui.intensity_value_2->display( intensity );
//      m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<double>( "currentIntensity", intensity );
//
//  }
// };
//
//
// }
// } //end namespaces
//
// #endif /* MAINWINDOW_CPP_ */
