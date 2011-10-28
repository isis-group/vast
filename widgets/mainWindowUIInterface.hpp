// #ifndef MAINWINDOWUIINTERFACE_HPP
// #define MAINWINDOWUIINTERFACE_HPP
//
// #include "mainWindow.hpp"
//
// namespace isis
// {
// namespace viewer
// {
//
//
// class MainWindowUIInterface : public MainWindow
// {
//  Q_OBJECT;
// public:
//  MainWindowUIInterface( QViewerCore *core, WidgetType wType = type_gl );
//
// protected:
//  void connectSignals();
//
//  QViewerCore *m_ViewerCore;
//
//
// public Q_SLOTS:
//  void showControlPanel( bool );
//  void toggleAxialView( bool );
//  void toggleCoronalView( bool );
//  void toggleSagittalView( bool );
//  void openImageAsZMap();
//  void openImageAsAnatomicalImage();
//  void openPreferences();
//  void saveImage();
//  void saveImageAs();
//  void showPluginInformation();
//
//  void triggeredMakeCurrentImage( bool );
//  void doubleClickedMakeCurrentImage( QListWidgetItem * );
//
//
//  void toggleViews( PlaneOrientation orientation, bool toggled );
//
// };
//
//
//
// }
// }
//
// #endif
