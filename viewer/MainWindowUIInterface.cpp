#include "MainWindowUIInterface.hpp"


isis::viewer::MainWindowUIInterface::MainWindowUIInterface(isis::viewer::QViewerCore* core)
	: MainWindow(core),
	m_ViewerCore(core)
{
	connectSignals();

}

void isis::viewer::MainWindowUIInterface::connectSignals()
{
	connect( ui.action_Exit, SIGNAL( triggered() ), this, SLOT( exitProgram() ) );
	connect( ui.actionShow_labels, SIGNAL( toggled( bool ) ), m_ViewerCore, SLOT( setShowLabels( bool ) ) );
	connect( ui.actionAutomatic_Scaling, SIGNAL( toggled( bool ) ), m_ViewerCore, SLOT( setAutomaticScaling( bool ) ) );
	connect( actionMakeCurrent, SIGNAL( triggered( bool ) ), this, SLOT( triggeredMakeCurrentImage( bool ) ) );
	connect( actionAsZMap, SIGNAL( triggered( bool ) ), this, SLOT( triggeredMakeCurrentImageZmap( bool ) ) );
	connect( m_ViewerCore, SIGNAL( emitImagesChanged( DataContainer ) ), this, SLOT( imagesChanged( DataContainer ) ) );
	connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( physicalCoordsChanged( util::fvector4 ) ) );
	connect( m_ViewerCore, SIGNAL( emitVoxelCoordChanged(util::ivector4)), this, SLOT( voxelCoordsChanged( util::ivector4 ) ) );
	connect( ui.imageStack, SIGNAL( itemClicked( QListWidgetItem * ) ), this, SLOT( checkImageStack( QListWidgetItem * ) ) );
	connect( ui.imageStack, SIGNAL( itemDoubleClicked( QListWidgetItem * ) ), this, SLOT( doubleClickedMakeCurrentImage( QListWidgetItem * ) ) );
	connect( ui.action_Open_Image, SIGNAL( triggered() ), this, SLOT( openImageAsAnatomicalImage() ) );
	connect( ui.actionOpen_DICOM, SIGNAL( triggered() ), this, SLOT( openDICOMDir() ) );
	connect( ui.upperThreshold, SIGNAL( sliderMoved( int ) ), this, SLOT( upperThresholdChanged( int ) ) );
	connect( ui.lowerThreshold, SIGNAL( sliderMoved( int ) ), this, SLOT( lowerThresholdChanged( int ) ) );
	connect( ui.opacity, SIGNAL( sliderMoved(int)), this, SLOT( opacityChanged( int ) ) );
	connect( ui.timestepSpinBox, SIGNAL( valueChanged( int ) ), m_ViewerCore, SLOT( timestepChanged( int ) ) ) ;
	connect( ui.interpolationType, SIGNAL( currentIndexChanged( int ) ), this, SLOT( interpolationChanged( int ) ) );
	connect( ui.currentImageBox, SIGNAL( activated(int)), this, SLOT( currentImageChanged( int )));
	connect( ui.action_Controllpanel, SIGNAL( triggered(bool)), this, SLOT( showControlPanel(bool)));
	connect( ui.actionAxial_view, SIGNAL( triggered(bool)), this, SLOT( toggleAxialView(bool)) );
	connect( ui.actionCoronal_View, SIGNAL( triggered(bool)), this, SLOT(toggleCoronalView(bool)) );
	connect( ui.actionSagittal_View, SIGNAL( triggered(bool)), this, SLOT(toggleSagittalView(bool)) );
	connect( ui.actionOpenZmap, SIGNAL( triggered()), this, SLOT(openImageAsZMap()));
	
	//attach all textFields
	connect( ui.row_value, SIGNAL( textChanged(QString) ), ui.row_value_2, SLOT( setText(QString)) );
	connect( ui.column_value, SIGNAL( textChanged(QString) ), ui.column_value_2, SLOT( setText(QString)) );
	connect( ui.slice_value, SIGNAL( textChanged(QString) ), ui.slice_value_2, SLOT( setText(QString)) );
	connect( ui.x_value, SIGNAL( textChanged(QString)), ui.x_value_2, SLOT( setText(QString)) );
	connect( ui.y_value, SIGNAL( textChanged(QString)), ui.y_value_2, SLOT( setText(QString)) );
	connect( ui.z_value, SIGNAL( textChanged(QString)), ui.z_value_2, SLOT( setText(QString)) );
	connect( ui.timestepSpinBox_2, SIGNAL( valueChanged(int)), ui.timestepSpinBox, SLOT(setValue(int)));
	connect( ui.horizontalSlider, SIGNAL( valueChanged(int)), this, SLOT( lowerThresholdChanged( int ) ) );
	connect( ui.horizontalSlider_2, SIGNAL( valueChanged(int)), this, SLOT( upperThresholdChanged( int ) ) );
	connect( ui.comboBox, SIGNAL( currentIndexChanged(int)), ui.interpolationType, SLOT( setCurrentIndex(int)));
	connect( ui.row_value, SIGNAL( returnPressed()), this, SLOT(setVoxelPosition()));
	connect( ui.column_value, SIGNAL( returnPressed()), this, SLOT(setVoxelPosition()));
	connect( ui.slice_value, SIGNAL( returnPressed()), this, SLOT(setVoxelPosition()));
	connect( ui.x_value, SIGNAL( returnPressed()), this, SLOT(setPhysicalPosition()));
	connect( ui.y_value, SIGNAL( returnPressed()), this, SLOT(setPhysicalPosition()));
	connect( ui.z_value, SIGNAL( returnPressed()), this, SLOT(setPhysicalPosition()));
	connect( ui.row_value_2, SIGNAL( returnPressed()), this, SLOT(setVoxelPosition()));
	connect( ui.column_value_2, SIGNAL( returnPressed()), this, SLOT(setVoxelPosition()));
	connect( ui.slice_value_2, SIGNAL( returnPressed()), this, SLOT(setVoxelPosition()));
	connect( ui.x_value_2, SIGNAL( returnPressed()), this, SLOT(setPhysicalPosition()));
	connect( ui.y_value_2, SIGNAL( returnPressed()), this, SLOT(setPhysicalPosition()));
	connect( ui.z_value_2, SIGNAL( returnPressed()), this, SLOT(setPhysicalPosition()));
	connect( ui.imageStack, SIGNAL( customContextMenuRequested( QPoint ) ), this, SLOT( contextMenuImageStack( QPoint ) ) );
}



void isis::viewer::MainWindowUIInterface::showControlPanel(bool triggered )
{
	if(m_State == splitted ) {
		ui.dockWidget_Control_Bottom->setVisible( triggered );
	} else if (m_State == single ) {
		ui.setupDockWidget->setVisible( triggered );
	}
}

void isis::viewer::MainWindowUIInterface::toggleCoronalView(bool triggered )
{
	toggleViews( coronal, triggered);
}
void isis::viewer::MainWindowUIInterface::toggleSagittalView(bool triggered )
{
	toggleViews( sagittal, triggered );
}
void isis::viewer::MainWindowUIInterface::toggleAxialView(bool triggered )
{
	toggleViews( axial, triggered );
}


void isis::viewer::MainWindowUIInterface::toggleViews(isis::viewer::PlaneOrientation orientation, bool triggered)
{
	BOOST_FOREACH( QViewerCore::WidgetMap::const_reference widget, m_ViewerCore->getWidgets() ) 
	{
		if( static_cast< GL::QGLWidgetImplementation* >(widget.second)->getPlaneOrientation() == orientation )
		{
			if( m_State == splitted ) {
				widget.second->parentWidget()->parentWidget()->setVisible( triggered );
			} else if (m_State == single ) {
				widget.second->parentWidget()->parentWidget()->parentWidget()->setVisible( triggered );
			}
		}
	}

}

void isis::viewer::MainWindowUIInterface::openImageAsAnatomicalImage()
{
	openImageAs( ImageHolder::anatomical_image );
}


void isis::viewer::MainWindowUIInterface::openImageAsZMap()
{
	openImageAs( ImageHolder::z_map);
}
