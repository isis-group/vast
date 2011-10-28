// #include "mainWindowUIInterface.hpp"
// #include "DataStorage/io_factory.hpp"
//
// namespace isis
// {
// namespace viewer
// {
//
// MainWindowUIInterface::MainWindowUIInterface( isis::viewer::QViewerCore *core, WidgetType wType )
//  : MainWindow( core, wType ),
//    m_ViewerCore( core )
// {
//  connectSignals();
// }
//
// void MainWindowUIInterface::connectSignals()
// {
//  connect( ui.action_Exit, SIGNAL( triggered() ), this, SLOT( exitProgram() ) );
//  connect( ui.actionShow_labels, SIGNAL( toggled( bool ) ), m_ViewerCore, SLOT( setShowLabels( bool ) ) );
//  connect( ui.actionAutomatic_Scaling, SIGNAL( toggled( bool ) ), m_ViewerCore, SLOT( setAutomaticScaling( bool ) ) );
//  connect( actionMakeCurrent, SIGNAL( triggered( bool ) ), this, SLOT( triggeredMakeCurrentImage( bool ) ) );
//  connect( actionAsZMap, SIGNAL( triggered( bool ) ), this, SLOT( triggeredMakeCurrentImageZmap( bool ) ) );
//  connect( m_ViewerCore, SIGNAL( emitImagesChanged( DataContainer ) ), this, SLOT( imagesChanged( DataContainer ) ) );
//  connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( physicalCoordsChanged( util::fvector4 ) ) );
//  connect( m_ViewerCore, SIGNAL( emitVoxelCoordChanged( util::ivector4 ) ), this, SLOT( voxelCoordsChanged( util::ivector4 ) ) );
//  connect( ui.imageStack, SIGNAL( itemClicked( QListWidgetItem * ) ), this, SLOT( checkImageStack( QListWidgetItem * ) ) );
//  connect( ui.imageStack, SIGNAL( itemDoubleClicked( QListWidgetItem * ) ), this, SLOT( doubleClickedMakeCurrentImage( QListWidgetItem * ) ) );
//  connect( ui.action_Open_Image, SIGNAL( triggered() ), this, SLOT( openImageAsAnatomicalImage() ) );
//  connect( ui.action_save_Image, SIGNAL( triggered() ), this, SLOT( saveImage() ) );
//  connect( ui.actionSave_Image_As, SIGNAL( triggered() ), this, SLOT( saveImageAs() ) );
//  connect( ui.actionOpen_DICOM, SIGNAL( triggered() ), this, SLOT( openDICOMDir() ) );
//  connect( ui.upperThreshold, SIGNAL( sliderMoved( int ) ), this, SLOT( upperThresholdChanged( int ) ) );
//  connect( ui.lowerThreshold, SIGNAL( sliderMoved( int ) ), this, SLOT( lowerThresholdChanged( int ) ) );
//  connect( ui.opacity, SIGNAL( sliderMoved( int ) ), this, SLOT( opacityChanged( int ) ) );
//  connect( ui.timestepSpinBox, SIGNAL( valueChanged( int ) ), m_ViewerCore, SLOT( timestepChanged( int ) ) ) ;
//  connect( ui.currentImageBox, SIGNAL( activated( int ) ), this, SLOT( currentImageChanged( int ) ) );
//  connect( ui.action_Controllpanel, SIGNAL( triggered( bool ) ), this, SLOT( showControlPanel( bool ) ) );
//  connect( ui.actionAxial_view, SIGNAL( triggered( bool ) ), this, SLOT( toggleAxialView( bool ) ) );
//  connect( ui.actionCoronal_View, SIGNAL( triggered( bool ) ), this, SLOT( toggleCoronalView( bool ) ) );
//  connect( ui.actionSagittal_View, SIGNAL( triggered( bool ) ), this, SLOT( toggleSagittalView( bool ) ) );
//  connect( ui.actionOpenZmap, SIGNAL( triggered() ), this, SLOT( openImageAsZMap() ) );
//  connect( ui.action_Preferences, SIGNAL( triggered() ), this, SLOT( openPreferences() ) );
//  connect( ui.action_Pluginsinformation, SIGNAL( triggered() ), this, SLOT( showPluginInformation() ) );
//
//  //attach all textFields
//  connect( ui.row_value, SIGNAL( textChanged( QString ) ), ui.row_value_2, SLOT( setText( QString ) ) );
//  connect( ui.column_value, SIGNAL( textChanged( QString ) ), ui.column_value_2, SLOT( setText( QString ) ) );
//  connect( ui.slice_value, SIGNAL( textChanged( QString ) ), ui.slice_value_2, SLOT( setText( QString ) ) );
//  connect( ui.x_value, SIGNAL( textChanged( QString ) ), ui.x_value_2, SLOT( setText( QString ) ) );
//  connect( ui.y_value, SIGNAL( textChanged( QString ) ), ui.y_value_2, SLOT( setText( QString ) ) );
//  connect( ui.z_value, SIGNAL( textChanged( QString ) ), ui.z_value_2, SLOT( setText( QString ) ) );
//  connect( ui.timestepSpinBox_2, SIGNAL( valueChanged( int ) ), ui.timestepSpinBox, SLOT( setValue( int ) ) );
//  connect( ui.horizontalSlider, SIGNAL( valueChanged( int ) ), this, SLOT( lowerThresholdChanged( int ) ) );
//  connect( ui.horizontalSlider_2, SIGNAL( valueChanged( int ) ), this, SLOT( upperThresholdChanged( int ) ) );
//  connect( ui.row_value, SIGNAL( returnPressed() ), this, SLOT( setVoxelPosition() ) );
//  connect( ui.column_value, SIGNAL( returnPressed() ), this, SLOT( setVoxelPosition() ) );
//  connect( ui.slice_value, SIGNAL( returnPressed() ), this, SLOT( setVoxelPosition() ) );
//  connect( ui.x_value, SIGNAL( returnPressed() ), this, SLOT( setPhysicalPosition() ) );
//  connect( ui.y_value, SIGNAL( returnPressed() ), this, SLOT( setPhysicalPosition() ) );
//  connect( ui.z_value, SIGNAL( returnPressed() ), this, SLOT( setPhysicalPosition() ) );
//  connect( ui.row_value_2, SIGNAL( returnPressed() ), this, SLOT( setVoxelPosition() ) );
//  connect( ui.column_value_2, SIGNAL( returnPressed() ), this, SLOT( setVoxelPosition() ) );
//  connect( ui.slice_value_2, SIGNAL( returnPressed() ), this, SLOT( setVoxelPosition() ) );
//  connect( ui.x_value_2, SIGNAL( returnPressed() ), this, SLOT( setPhysicalPosition() ) );
//  connect( ui.y_value_2, SIGNAL( returnPressed() ), this, SLOT( setPhysicalPosition() ) );
//  connect( ui.z_value_2, SIGNAL( returnPressed() ), this, SLOT( setPhysicalPosition() ) );
//  connect( ui.imageStack, SIGNAL( customContextMenuRequested( QPoint ) ), this, SLOT( contextMenuImageStack( QPoint ) ) );
// }
//
//
//
// void MainWindowUIInterface::showControlPanel( bool triggered )
// {
//  if( m_State == splitted ) {
//      ui.dockWidget_Control_Bottom->setVisible( triggered );
//  } else if ( m_State == single ) {
//      ui.setupDockWidget->setVisible( triggered );
//  }
// }
//
// void MainWindowUIInterface::toggleCoronalView( bool triggered )
// {
//  toggleViews( coronal, triggered );
// }
// void MainWindowUIInterface::toggleSagittalView( bool triggered )
// {
//  toggleViews( sagittal, triggered );
// }
// void MainWindowUIInterface::toggleAxialView( bool triggered )
// {
//  toggleViews( axial, triggered );
// }
//
//
// void MainWindowUIInterface::toggleViews( isis::viewer::PlaneOrientation orientation, bool triggered )
// {
//  BOOST_FOREACH( QViewerCore::WidgetList::const_reference widget, m_ViewerCore->getWidgets() ) {
//      if( static_cast< QWidgetImplementationBase * >( widget )->getPlaneOrientation() == orientation ) {
//          if( m_State == splitted ) {
//              widget->getParent()->parentWidget()->setVisible( triggered );
//          } else if ( m_State == single ) {
//              widget->getParent()->parentWidget()->parentWidget()->setVisible( triggered );
//          }
//      }
//  }
//
// }
//
// void MainWindowUIInterface::openImageAsAnatomicalImage()
// {
//  openImageAs( ImageHolder::anatomical_image );
// }
//
//
// void MainWindowUIInterface::openImageAsZMap()
// {
//  openImageAs( ImageHolder::z_map );
// }
//
// void MainWindowUIInterface::openPreferences()
// {
//  m_PreferencesDialog->show();
// }
//
//
// void MainWindowUIInterface::doubleClickedMakeCurrentImage( QListWidgetItem * )
// {
//  triggeredMakeCurrentImage( true );
// }
//
//
// void MainWindowUIInterface::triggeredMakeCurrentImage( bool triggered )
// {
//  m_ViewerCore->setCurrentImage( m_ViewerCore->getDataContainer().at( ui.imageStack->currentItem()->text().toStdString() ) );
//  imagesChanged( m_ViewerCore->getDataContainer() );
//  updateInterfaceValues();
// }
//
// void MainWindowUIInterface::saveImage()
// {
//  if( !m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ).size() ) {
//      QMessageBox msgBox;
//      msgBox.setText( "The image that is currently selected has no changes! Won´t save anything." );
//      msgBox.exec();
//  } else {
//      QMessageBox msgBox;
//      msgBox.setIcon( QMessageBox::Information );
//      std::stringstream text;
//      text << "This will overwrite" << m_ViewerCore->getCurrentImage()->getFileNames().front() << " !";
//      msgBox.setText( text.str().c_str() );
//      msgBox.setInformativeText( "Do you want to proceed?" );
//      std::stringstream detailedText;
//      detailedText << "Changed attributes: " << std::endl;
//      BOOST_FOREACH( util::slist::const_reference attrChanged, m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<util::slist>( "changedAttributes" ) ) {
//          detailedText << " >> " << attrChanged << std::endl;
//      }
//      msgBox.setDetailedText( detailedText.str().c_str() );
//      msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
//      msgBox.setDefaultButton( QMessageBox::No );
//
//      switch ( msgBox.exec() ) {
//      case QMessageBox::No:
//          return;
//          break;
//      case QMessageBox::Yes:
//          isis::data::IOFactory::write( *m_ViewerCore->getCurrentImage()->getISISImage(), m_ViewerCore->getCurrentImage()->getFileNames().front(), "", "" );
//          break;
//      }
//  }
// }
//
// void MainWindowUIInterface::saveImageAs()
// {
//  std::stringstream fileFormats;
//  fileFormats << "Image files (" << getFileFormatsAsString( std::string( "*." ) ) << ")";
//  QString filename = QFileDialog::getSaveFileName( this,
//                     tr( "Save Image As..." ),
//                     m_CurrentPath,
//                     tr( fileFormats.str().c_str() ) );
//
//  if( filename.size() ) {
//      isis::data::IOFactory::write( *m_ViewerCore->getCurrentImage()->getISISImage(), filename.toStdString(), "", "" );
//  }
//
// }
//
// void MainWindowUIInterface::showPluginInformation()
// {
//
// }
//
//
// }
// }
