/*
 * MainWindow.cpp
 *
 *  Created on: Nov 3, 2010
 *      Author: tuerke
 */

#include "MainWindow.hpp"
#include <common.hpp>
#include <DataStorage/io_factory.hpp>


namespace isis
{
namespace viewer
{

MainWindow::MainWindow( QViewerCore *core, WidgetType wType )
	: m_ViewerCore( core ),
	  m_WidgetType( wType )
{
	m_PlottingDialog->setViewerCore( m_ViewerCore );
	m_PreferencesDialog = new QPreferencesDialog( this, m_ViewerCore );
	m_State = single;
	actionMakeCurrent = new QAction( "Make current", this );
	actionAsZMap = new QAction( "Show as zmap", this );
	actionAsZMap->setCheckable( true );

	m_Toolbar = new QToolBar( this );

	switch( m_WidgetType ) {
	case type_gl:
		m_MasterWidget = new GL::QGLWidgetImplementation( core, 0, axial );
		break;
	case type_qt:
		m_MasterWidget = new qt::QImageWidgetImplementation( core, 0, axial );
		break;
	}

	m_AxialWidget =  m_MasterWidget->createSharedWidget( ui.axialWidget, axial );
	m_ViewerCore->registerWidget( "axialView", m_AxialWidget );

	m_CoronalWidget = m_MasterWidget->createSharedWidget( ui.coronalWidget, coronal );
	m_ViewerCore->registerWidget( "coronalView", m_CoronalWidget );

	m_SagittalWidget = m_MasterWidget->createSharedWidget( ui.sagittalWidget, sagittal );
	m_ViewerCore->registerWidget( "sagittalView", m_SagittalWidget );

	setInitialState();
	loadSettings();
}

void MainWindow::closeEvent( QCloseEvent *event )
{
	saveSettings();
}

void MainWindow::saveSettings()
{
	//saving the preferences to the profile file
	m_ViewerCore->getSettings()->beginGroup( "MainWindow" );
	m_ViewerCore->getSettings()->setValue( "size", size() );
	m_ViewerCore->getSettings()->setValue( "maximized", isMaximized() );
	m_ViewerCore->getSettings()->setValue( "pos", pos() );
	m_ViewerCore->getSettings()->endGroup();
	m_ViewerCore->getSettings()->sync();
}


void MainWindow::loadSettings()
{
	m_ViewerCore->getSettings()->beginGroup( "MainWindow" );
	resize( m_ViewerCore->getSettings()->value( "size", QSize( 900, 900 ) ).toSize() );
	move( m_ViewerCore->getSettings()->value( "pos", QPoint( 0, 0 ) ).toPoint() );

	if( m_ViewerCore->getSettings()->value( "maximized", false ).toBool() ) {
		showMaximized();
	}

	m_ViewerCore->getSettings()->endGroup();
	m_ViewerCore->getSettings()->beginGroup( "UserProfile" );
	ui.interpolationType->setCurrentIndex( m_ViewerCore->getSettings()->value( "interpolation", 0 ).toUInt() );
	ui.actionAutomatic_Scaling->setChecked( m_ViewerCore->getSettings()->value( "scaling", 0 ).toBool() );
	ui.actionShow_labels->setChecked( m_ViewerCore->getSettings()->value( "labels", 0 ).toBool() );
	m_ViewerCore->getOption()->propagateZooming = m_ViewerCore->getSettings()->value( "propagateZooming", false ).toBool();
	m_ViewerCore->getSettings()->endGroup();


}



void isis::viewer::MainWindow::setInitialState()
{
	ui.actionShow_labels->setCheckable( true );
	ui.actionShow_labels->setChecked( false );
	ui.imageStack->setContextMenuPolicy( Qt::CustomContextMenu );
	ui.dockWidget_Control_Bottom->setVisible( false );
	ui.actionAxial_view->setChecked( true );
	ui.actionCoronal_View->setChecked( true );
	ui.actionSagittal_View->setChecked( true );
	ui.action_Controllpanel->setChecked( true );
	ui.action_Plotting->setEnabled( false );

	//toolbar stuff
	m_Toolbar->setOrientation( Qt::Horizontal );
	m_Toolbar->setMinimumHeight( 20 );
	m_Toolbar->setMaximumHeight( 30 );
	this->addToolBar( Qt::TopToolBarArea, m_Toolbar );
	m_Toolbar->addAction( ui.action_Open_Image );
	m_Toolbar->addAction( ui.actionOpenZmap );
	m_Toolbar->addAction( ui.actionOpen_DICOM );
	m_Toolbar->addSeparator();
	m_Toolbar->addAction( ui.action_Preferences );
	m_Toolbar->addAction( ui.actionShow_labels );
	m_Toolbar->addAction( ui.actionAutomatic_Scaling );
	m_Toolbar->addSeparator();
	m_Toolbar->addAction( ui.actionAxial_view );
	m_Toolbar->addAction( ui.actionSagittal_View );
	m_Toolbar->addAction( ui.actionCoronal_View );
	m_Toolbar->addAction( ui.action_Controllpanel );
	m_Toolbar->addSeparator();
	m_Toolbar->addAction( ui.action_Plotting );
	m_Toolbar->addSeparator();
	m_Toolbar->addAction( ui.action_Exit );
	m_ViewerCore->setCoordsTransformation( util::fvector4( -1, -1, 1, 1 ) );
}

void MainWindow::setVoxelPosition()
{
	if( m_State == splitted ) {

		m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getImage()->getPhysicalCoordsFromIndex(
				util::ivector4( ui.row_value->text().toInt(),
								ui.column_value->text().toInt(),
								ui.slice_value->text().toInt() ) ) ) ;
	} else if ( m_State == single ) {
		m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getImage()->getPhysicalCoordsFromIndex(
				util::ivector4( ui.row_value_2->text().toInt(),
								ui.column_value_2->text().toInt(),
								ui.slice_value_2->text().toInt() ) ) ) ;
	}

}

void MainWindow::setPhysicalPosition()
{
	if( m_State == splitted ) {
		m_ViewerCore->physicalCoordsChanged( util::fvector4( ui.x_value->text().toFloat(),
											 ui.y_value->text().toFloat(),
											 ui.z_value->text().toFloat() ) );
	} else if ( m_State == single ) {
		m_ViewerCore->physicalCoordsChanged( util::fvector4( ui.x_value_2->text().toFloat(),
											 ui.y_value_2->text().toFloat(),
											 ui.z_value_2->text().toFloat() ) );
	}
}


void MainWindow::contextMenuImageStack( QPoint position )
{
	QList<QAction *> actions;

	if( ui.imageStack->indexAt( position ).isValid() ) {
		actions.append( actionMakeCurrent );
		actions.append( actionAsZMap );
	}

	QMenu::exec( actions, ui.imageStack->mapToGlobal( position ) );

}


void MainWindow::currentImageChanged( int index )
{
	m_ViewerCore->setCurrentImage( m_ViewerCore->getDataContainer().getImageByID( index ) );
	imagesChanged( m_ViewerCore->getDataContainer() );
	updateInterfaceValues();

	if( m_ViewerCore->getCurrentImage()->getImageSize()[3] > 1 ) {
		ui.action_Plotting->setEnabled( true );
	} else {
		ui.action_Plotting->setEnabled( false );
	}
}


void MainWindow::updateInterfaceValues()
{
	if( m_ViewerCore->getCurrentImage()->getImageProperties().imageType == ImageHolder::z_map ) {
		ui.lowerThreshold->setSliderPosition( 1000.0 / m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() *
											  ( m_ViewerCore->getCurrentImage()->getImageProperties().zmapThreshold.first  ) );
		ui.upperThreshold->setSliderPosition( 1000.0 / m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>() *
											  ( m_ViewerCore->getCurrentImage()->getImageProperties().zmapThreshold.second ) );
	} else {
		double range = m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>() - m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>();
		ui.lowerThreshold->setSliderPosition( 1000.0 / range *
											  ( m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<float>("lowerThreshold") - m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() ) );
		ui.upperThreshold->setSliderPosition( 1000.0 / range *
											  ( m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<float>("lowerThreshold") - m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() ) );
	}

	m_ViewerCore->updateScene();
}

void MainWindow::triggeredMakeCurrentImageZmap( bool triggered )
{
	if( triggered ) {
		m_ViewerCore->getDataContainer()[ui.imageStack->currentItem()->text().toStdString()]->setImageType( ImageHolder::z_map );
	} else {
		m_ViewerCore->getDataContainer()[ui.imageStack->currentItem()->text().toStdString()]->setImageType( ImageHolder::anatomical_image );
	}

	m_ViewerCore->updateScene();
	imagesChanged( m_ViewerCore->getDataContainer() );
}


void MainWindow::voxelCoordsChanged( util::ivector4 coords )
{
	physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getImage()->getPhysicalCoordsFromIndex( coords ) );
}


void MainWindow::physicalCoordsChanged( util::fvector4 coords )
{
	util::ivector4 voxelCoords = m_ViewerCore->getCurrentImage()->getImage()->getIndexFromPhysicalCoords( coords );
	util::fvector4 transformedCoords = m_ViewerCore->getTransformedCoords( coords );

	ui.row_value->setText( QString::number( voxelCoords[0] ) );
	ui.column_value->setText( QString::number( voxelCoords[1] ) );
	ui.slice_value->setText( QString::number( voxelCoords[2] ) );
	ui.x_value->setText( QString::number( transformedCoords[0] ) );
	ui.y_value->setText( QString::number( transformedCoords[1] ) );
	ui.z_value->setText( QString::number( transformedCoords[2] ) );
	bool isOutside = false;

	for( size_t i = 0; i < 4; i++ ) {
		if( voxelCoords[i] < 0 || voxelCoords[i] > static_cast<int32_t>( m_ViewerCore->getCurrentImage()->getImageSize()[i] -  1 ) )
			return;
	}

	data::Chunk ch = m_ViewerCore->getCurrentImage()->getImage()->getChunk( voxelCoords[0], voxelCoords[1], voxelCoords[2], voxelCoords[3] );

	switch( ch.getTypeID() ) {
	case data::ValuePtr<int8_t>::staticID:
		displayIntensity<int8_t>( voxelCoords );
		break;
	case data::ValuePtr<uint8_t>::staticID:
		displayIntensity<uint8_t>( voxelCoords );
		break;
	case data::ValuePtr<int16_t>::staticID:
		displayIntensity<int16_t>( voxelCoords );
		break;
	case data::ValuePtr<uint16_t>::staticID:
		displayIntensity<uint16_t>( voxelCoords );
		break;
	case data::ValuePtr<int32_t>::staticID:
		displayIntensity<int32_t>( voxelCoords );
		break;
	case data::ValuePtr<uint32_t>::staticID:
		displayIntensity<uint32_t>( voxelCoords );
		break;
	case data::ValuePtr<float>::staticID:
		displayIntensity<float>( voxelCoords );
		break;
	case data::ValuePtr<double>::staticID:
		displayIntensity<double>( voxelCoords );
		break;
	}

}

void MainWindow::imagesChanged( DataContainer images )
{
	ui.imageStack->clear();
	BOOST_FOREACH( DataContainer::const_reference imageRef, images ) {
		QListWidgetItem *item = new QListWidgetItem;
		QString sD = imageRef.second->getPropMap().getPropertyAs<std::string>( "sequenceDescription" ).c_str();
		item->setText( QString( imageRef.second->getFileNames().front().c_str() ) );
		item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable );

		if( imageRef.second->getPropMap().getPropertyAs<bool>( "isVisible" ) ) {
			item->setCheckState( Qt::Checked );
		} else {
			item->setCheckState( Qt::Unchecked );
		}

		if( m_ViewerCore->getCurrentImage().get() == imageRef.second.get() ) {
			item->setIcon( QIcon( ":/common/currentImage.gif" ) );
		}

		ui.imageStack->addItem( item );
	}
	double min = roundNumber<double>( m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>(), 2 );
	double max = roundNumber<double>( m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>(), 2 );
	ui.minValueLabel->setText( QString::number( min ) );
	ui.maxValueLabel->setText( QString::number( max ) );

	if( m_ViewerCore->getCurrentImage()->getImage()->getSizeAsVector()[3] > 1 ) {
		ui.timestepSpinBox->setEnabled( true );
		ui.timestepSpinBox->setMaximum( m_ViewerCore->getCurrentImage()->getImageSize()[3] - 1 );
		ui.timestepSpinBox_2->setEnabled( true );
		ui.timestepSpinBox_2->setMaximum( m_ViewerCore->getCurrentImage()->getImageSize()[3] - 1 );
		ui.timestepSpinBox->setValue( m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<size_t>( "currentTimestep" ) );
		ui.timestepSpinBox_2->setValue( m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<size_t>( "currentTimestep" ) );

	} else {
		ui.timestepSpinBox->setEnabled( false );
		ui.timestepSpinBox_2->setEnabled( false );
	}

	if( m_ViewerCore->getCurrentImage()->getImageProperties().imageType == ImageHolder::anatomical_image ) {
		ui.upperThreshold->setVisible( false );
		ui.lowerThreshold->setVisible( false );
		ui.maxLabel->setVisible( false );
		ui.minLabel->setVisible( false );
		ui.maxValueLabel->setVisible( false );
		ui.minValueLabel->setVisible( false );
		ui.opacity->setVisible( true );
		ui.labelOpacity->setVisible( true );
		ui.frame_4->setVisible( false );
		ui.opacity->setValue( ( ui.opacity->maximum() - ui.opacity->minimum() ) * m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<float>( "opacity" ) );
	} else if ( m_ViewerCore->getCurrentImage()->getImageProperties().imageType == ImageHolder::z_map ) {
		ui.upperThreshold->setVisible( true );
		ui.lowerThreshold->setVisible( true );
		ui.maxLabel->setVisible( true );
		ui.minLabel->setVisible( true );
		ui.maxValueLabel->setVisible( true );
		ui.minValueLabel->setVisible( true );
		ui.opacity->setVisible( false );
		ui.labelOpacity->setVisible( false );
	}

	/*if( m_ViewerCore->widgetsAreIntitialized() ) {

	}*/
	m_ViewerCore->updateScene();

	if( m_ViewerCore->getCurrentImage()->getImageSize()[3] > 1 ) {
		ui.action_Plotting->setEnabled( true );
	} else {
		ui.action_Plotting->setEnabled( false );
	}
}

void MainWindow::openImageAs( ImageHolder::ImageType type )
{
	std::string title;

	switch ( type ) {
	case ImageHolder::anatomical_image:
		title = "Open anatomical images";
		break;
	case ImageHolder::z_map:
		title = "Open images as zmaps";
		break;
	}

	std::stringstream fileFormats;
	fileFormats << "Image files (" << getFileFormatsAsString( std::string( "*." ) ) << ")";
	QStringList filenames = QFileDialog::getOpenFileNames( this,
							tr( title.c_str() ),
							m_CurrentPath,
							tr( fileFormats.str().c_str() ) );


	if( !filenames.empty() ) {
		QDir dir;
		m_CurrentPath = dir.absoluteFilePath( filenames.front() );
		bool isFirstImage = m_ViewerCore->getDataContainer().size() == 0;
		std::list<data::Image> imgList;
		util::slist pathList;
		BOOST_FOREACH( QStringList::const_reference filename, filenames ) {
			std::list<data::Image> tempImgList = data::IOFactory::load( filename.toStdString() , "", "" );
			pathList.push_back( filename.toStdString() );
			BOOST_FOREACH( std::list<data::Image>::const_reference image, tempImgList ) {
				imgList.push_back( image );
			}
		}
		m_ViewerCore->addImageList( imgList, type, true );
		m_ViewerCore->updateScene( isFirstImage );
	}
}

void MainWindow::handImagesToPlotter()
{
	//hand all images to plotting instance
	QPlottingDialog::ImageList tmpList;
	BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
		tmpList.push_back( image.second );
	}
	m_PlottingDialog->setImageHolderList( tmpList );
}


void MainWindow::openDICOMDir()
{
	QString dir = QFileDialog::getExistingDirectory( this, tr( "Open directory" ), m_CurrentPath );

	if( dir.size() ) {
		m_CurrentPath = dir;
		bool isFirstImage = m_ViewerCore->getDataContainer().size() == 0;
		m_ViewerCore->addImageList( data::IOFactory::load( dir.toStdString(), "", "" ), ImageHolder::anatomical_image, true );
		m_ViewerCore->updateScene();

	}
}


void MainWindow::checkImageStack( QListWidgetItem *item )
{
	if( item->checkState() == Qt::Checked ) {
		m_ViewerCore->getDataContainer().at( item->text().toStdString() )->getPropMap().setPropertyAs<bool>( "isVisible", true );

	} else if( item->checkState() == Qt::Unchecked ) {
		m_ViewerCore->getDataContainer().at( item->text().toStdString() )->getPropMap().setPropertyAs<bool>( "isVisible", false );
	}

	m_ViewerCore->updateScene();

}

void MainWindow::exitProgram()
{
	close();
}

void MainWindow::opacityChanged( int opacity )
{
	float opacityFloat = 1.0 / ( ui.opacity->maximum() - ui.opacity->minimum() ) * opacity;
	m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<float>( "opacity", opacityFloat );
	m_ViewerCore->updateScene();
}



void MainWindow::lowerThresholdChanged( int lowerThreshold )
{

	if( m_ViewerCore->getCurrentImage()->getImageProperties().imageType == ImageHolder::z_map ) {
		m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<float>( "lowerThreshold", ( m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() / 1000 ) * ( lowerThreshold  )  );
	} else {
		double range = m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>() - m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>();
		m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<float>( "lowerThreshold", ( range / 1000 ) * ( lowerThreshold  ) + m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() );
	}
	m_ViewerCore->updateScene();
}

void MainWindow::upperThresholdChanged( int upperThreshold )
{
	if( m_ViewerCore->getCurrentImage()->getImageProperties().imageType == ImageHolder::z_map ) {
		m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<float>( "upperThreshold", (  m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>() / 1000 ) * ( upperThreshold  ) );
	} else {
		double range = m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>() - m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>();
		m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<float>( "upperThreshold", ( range / 1000 ) * ( upperThreshold + 1 )  + m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() ) ;
	}

	m_ViewerCore->updateScene();

}

void MainWindow::interpolationChanged( int index )
{
	BOOST_FOREACH( QViewerCore::WidgetMap::reference widget, m_ViewerCore->getWidgets() ) 
	{
		widget.second->setInterpolationType( static_cast<InterpolationType>( index ) );
	}
	m_ViewerCore->updateScene();
}

void MainWindow::assembleViewInRows( )
{
	m_State = splitted;
	std::list< boost::shared_ptr< ImageHolder> > zMaps;
	std::list< boost::shared_ptr< ImageHolder> > relevantImages;
	std::list< boost::shared_ptr< ImageHolder> > anatomicalImages;
	BOOST_FOREACH( DataContainer::const_reference dataSet, m_ViewerCore->getDataContainer() ) {
		if( dataSet.second->getImageProperties().imageType == ImageHolder::z_map ) {
			zMaps.push_back( dataSet.second );
		} else if ( dataSet.second->getImageProperties().imageType == ImageHolder::anatomical_image ) {
			anatomicalImages.push_back( dataSet.second );
		}
	}

	if( !anatomicalImages.size() ) {
		LOG( Runtime, warning ) << "Could not find any anatomical image.";
	}

	if( zMaps.size() ) {
		relevantImages = zMaps;
	} else {
		relevantImages = anatomicalImages;
	}

	//some gui related modifications
	ui.gridLayout->addWidget( ui.coronalDockWidget, 0, 2 );
	ui.setupDockWidget->setVisible( false );
	ui.dockWidget_Control_Bottom->setVisible( true );
	//we create our widgets dynamically so we do not need the static ones
	ui.sagittalDockWidget->setVisible( false );
	ui.coronalDockWidget->setVisible( false );
	ui.axialDockWidget->setVisible( false );

	unsigned short row = 0;
	BOOST_FOREACH( std::list< boost::shared_ptr< ImageHolder> >::const_reference image, relevantImages ) {
		row++;
		//setup dockwidgets
		QDockWidget *axialDock = new QDockWidget( this );
		QDockWidget *sagittalDock = new QDockWidget( this );
		QDockWidget *coronalDock = new QDockWidget( this );
		WidgetImplementationBase *axialWidget = createView( axialDock, axial, row );
		WidgetImplementationBase *sagittalWidget = createView( sagittalDock, sagittal, row );
		WidgetImplementationBase *coronalWidget = createView( coronalDock, coronal, row );
		axialWidget->addImage( image );
		sagittalWidget->addImage( image );
		coronalWidget->addImage( image );

		if( zMaps.size() ) {
			axialWidget->addImage( anatomicalImages.front() );
			sagittalWidget->addImage( anatomicalImages.front() );
			coronalWidget->addImage( anatomicalImages.front() );
		}

		//setup title
		std::stringstream title;
		QFileInfo dir( tr( image->getFileNames().front().c_str() ) );
		title << dir.fileName().toStdString();

		if( m_ViewerCore->getSettings()->value( "UserProfile/showDesc", false ).toBool() ) {
			title << " (";

			if( image->getImage()->hasProperty( "sequenceDescription" ) ) {
				title <<  image->getImage()->getPropertyAs<std::string>( "sequenceDescription" ) << ")";
			} else {
				title << "no desc.)";
			}
		}

		axialDock->setWindowTitle( tr( title.str().c_str() ) );
		sagittalDock->setWindowTitle( tr( title.str().c_str() ) );
		coronalDock->setWindowTitle( tr( title.str().c_str() ) );
		ui.currentImageBox->insertItem( image->getID(), tr( title.str().c_str() ) );
		ui.gridLayout->addWidget( axialDock, row, 0 );
		ui.gridLayout->addWidget( sagittalDock, row, 1 );
		ui.gridLayout->addWidget( coronalDock, row, 2 );
	}
	ui.currentImageBox->setCurrentIndex( m_ViewerCore->getCurrentImage()->getID() );
}


WidgetImplementationBase *MainWindow::createView( QDockWidget *widget, PlaneOrientation orientation, unsigned short index )
{
	widget->setFloating( false );
	widget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
	widget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	QFrame *frame = new QFrame( widget );
	widget->setWidget( frame );
	widget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );
	widget->setMinimumHeight( 200 );
	widget->setMinimumWidth( 200 );
	WidgetImplementationBase *view = m_MasterWidget->createSharedWidget( frame, orientation );
	std::stringstream name;

	switch ( orientation ) {
	case axial:
		name << "axialView_" << index;
		break;
	case sagittal:
		name << "sagittalView_" << index;
		break;
	case coronal:
		name  << "coronalView_" << index;
		break;
	}

	m_ViewerCore->registerWidget( name.str(), view );
	return view;

}

}
} //end namespace
