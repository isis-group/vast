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

MainWindow::MainWindow( QViewerCore *core )
	: m_ViewerCore( core )
{
	actionMakeCurrent = new QAction( "Make current", this );
	actionAsZMap = new QAction( "Show as zmap", this );
	actionAsZMap->setCheckable( true );
	connect( ui.action_Exit, SIGNAL( triggered() ), this, SLOT( exitProgram() ) );
	connect( ui.actionShow_labels, SIGNAL( toggled( bool ) ), m_ViewerCore, SLOT( setShowLabels( bool ) ) );
	connect( ui.actionAutomatic_Scaling, SIGNAL( toggled( bool ) ), m_ViewerCore, SLOT( setAutomaticScaling( bool ) ) );
	connect( actionMakeCurrent, SIGNAL( triggered( bool ) ), this, SLOT( triggeredMakeCurrentImage( bool ) ) );
	connect( actionAsZMap, SIGNAL( triggered( bool ) ), this, SLOT( triggeredMakeCurrentImageZmap( bool ) ) );
	connect( core, SIGNAL( emitImagesChanged( DataContainer ) ), this, SLOT( imagesChanged( DataContainer ) ) );
	connect( core, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), this, SLOT( physicalCoordsChanged( util::fvector4 ) ) );
	connect( ui.imageStack, SIGNAL( itemClicked( QListWidgetItem * ) ), this, SLOT( checkImageStack( QListWidgetItem * ) ) );
	connect( ui.imageStack, SIGNAL( itemDoubleClicked( QListWidgetItem * ) ), this, SLOT( doubleClickedMakeCurrentImage( QListWidgetItem * ) ) );
	connect( ui.action_Open_Image, SIGNAL( triggered() ), this, SLOT( openImage() ) );
	connect( ui.actionOpen_DICOM, SIGNAL( triggered() ), this, SLOT( openDICOMDir() ) );
	connect( ui.upperThreshold, SIGNAL( sliderMoved( int ) ), this, SLOT( upperThresholdChanged( int ) ) );
	connect( ui.lowerThreshold, SIGNAL( sliderMoved( int ) ), this, SLOT( lowerThresholdChanged( int ) ) );
	connect( ui.opacity, SIGNAL( sliderMoved(int)), this, SLOT( opacityChanged( int ) ) );
	connect( ui.timestepSpinBox, SIGNAL( valueChanged( int ) ), m_ViewerCore, SLOT( timestepChanged( int ) ) ) ;
	connect( ui.interpolationType, SIGNAL( currentIndexChanged( int ) ), this, SLOT( interpolationChanged( int ) ) );
	connect( ui.currentImageBox, SIGNAL( currentIndexChanged(int)), this, SLOT( currentImageChanged( int )));
	//attach all textFields
	connect( ui.row_value, SIGNAL( textChanged(QString) ), ui.row_value_2, SLOT( setText(QString)) );
	connect( ui.column_value, SIGNAL( textChanged(QString) ), ui.column_value_2, SLOT( setText(QString)) );
	connect( ui.slice_value, SIGNAL( textChanged(QString) ), ui.slice_value_2, SLOT( setText(QString)) );
	connect( ui.x_value, SIGNAL( textChanged(QString)), ui.x_value_2, SLOT( setText(QString)) );
	connect( ui.y_value, SIGNAL( textChanged(QString)), ui.y_value_2, SLOT( setText(QString)) );
	connect( ui.z_value, SIGNAL( textChanged(QString)), ui.z_value_2, SLOT( setText(QString)) );
	connect( ui.timestepSpinBox_2, SIGNAL( valueChanged(int)), ui.timestepSpinBox, SLOT(setValue(int)));
	
	//we need a master widget to keep opengl running in case all visible widgets were closed

	m_MasterWidget = new GL::QGLWidgetImplementation( core, 0, axial );

	m_AxialWidget =  m_MasterWidget->createSharedWidget( ui.axialWidget, axial );
	m_ViewerCore->registerWidget( "axialView", m_AxialWidget );

	m_CoronalWidget = m_MasterWidget->createSharedWidget( ui.coronalWidget, coronal );
	m_ViewerCore->registerWidget( "coronalView", m_CoronalWidget );

	m_SagittalWidget = m_MasterWidget->createSharedWidget( ui.sagittalWidget, sagittal );
	m_ViewerCore->registerWidget( "sagittalView", m_SagittalWidget );

	ui.actionShow_labels->setCheckable( true );
	ui.actionShow_labels->setChecked( false );
	ui.imageStack->setContextMenuPolicy( Qt::CustomContextMenu );
	ui.bottomCoordsFrame->setVisible(false);
	connect( ui.imageStack, SIGNAL( customContextMenuRequested( QPoint ) ), this, SLOT( contextMenuImageStack( QPoint ) ) );
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

void MainWindow::currentImageChanged(int index )
{
	m_ViewerCore->setCurrentImage( m_ViewerCore->getDataContainer().getImageByID(index) );
	imagesChanged( m_ViewerCore->getDataContainer() );
}


void MainWindow::triggeredMakeCurrentImage( bool triggered )
{
	m_ViewerCore->setCurrentImage( m_ViewerCore->getDataContainer().at( ui.imageStack->currentItem()->text().toStdString() ) );

	if( m_ViewerCore->getCurrentImage()->getImageState().imageType == ImageHolder::z_map ) {
		ui.lowerThreshold->setSliderPosition( 1000.0 / m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() *
											  ( m_ViewerCore->getCurrentImage()->getImageState().zmapThreshold.first  ) );
		ui.upperThreshold->setSliderPosition( 1000.0 / m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>() *
											  ( m_ViewerCore->getCurrentImage()->getImageState().zmapThreshold.second ) );
	} else {
		double range = m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>() - m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>();
		ui.lowerThreshold->setSliderPosition( 1000.0 / range *
											  ( m_ViewerCore->getCurrentImage()->getImageState().threshold.first - m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() ) );
		ui.upperThreshold->setSliderPosition( 1000.0 / range *
											  ( m_ViewerCore->getCurrentImage()->getImageState().threshold.second - m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() ) );
	}


	imagesChanged( m_ViewerCore->getDataContainer() );
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
	imagesChanged(m_ViewerCore->getDataContainer());
}


void MainWindow::doubleClickedMakeCurrentImage( QListWidgetItem * )
{
	triggeredMakeCurrentImage( true );
}


void MainWindow::physicalCoordsChanged( util::fvector4 coords )
{
	util::ivector4 voxelCoords = m_ViewerCore->getCurrentImage()->getImage()->getIndexFromPhysicalCoords( coords );
	ui.row_value->setText( QString::number(voxelCoords[0]));
	ui.column_value->setText( QString::number(voxelCoords[1]) );
	ui.slice_value->setText( QString::number(voxelCoords[2]) );
	ui.x_value->setText( QString::number( coords[0] ) );
	ui.y_value->setText( QString::number( coords[1] ) );
	ui.z_value->setText( QString::number( coords[2] ) );
	bool isOutside = false;
	for( size_t i = 0;i<4; i++ ) {
		if(voxelCoords[i] < 0 || voxelCoords[i] > m_ViewerCore->getCurrentImage()->getImageSize()[i] -  1)
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

		if( imageRef.second->getImageState().visible ) {
			item->setCheckState( Qt::Checked );
		} else {
			item->setCheckState( Qt::Unchecked );
		}

		if( m_ViewerCore->getCurrentImage().get() == imageRef.second.get() ) {
			item->setIcon( QIcon( ":/common/icon_check.png" ) );
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
		ui.timestepSpinBox_2->setMaximum( m_ViewerCore->getCurrentImage()->getImageSize()[3] -1 );
		ui.timestepSpinBox->setValue( m_ViewerCore->getCurrentImage()->getImageState().timestep );
		ui.timestepSpinBox_2->setValue( m_ViewerCore->getCurrentImage()->getImageState().timestep );
		
	} else {
		ui.timestepSpinBox->setEnabled( false );
		ui.timestepSpinBox_2->setEnabled( false );
	}
	if( m_ViewerCore->getCurrentImage()->getImageState().imageType == ImageHolder::anatomical_image ) {
		ui.upperThreshold->setVisible(false);
		ui.lowerThreshold->setVisible(false);
		ui.maxLabel->setVisible( false );
		ui.minLabel->setVisible( false );
		ui.maxValueLabel->setVisible( false );
		ui.minValueLabel->setVisible( false );
		ui.opacity->setVisible( true );
		ui.labelOpacity->setVisible( true );
		ui.opacity->setValue( (ui.opacity->maximum() - ui.opacity->minimum()) * m_ViewerCore->getCurrentImage()->getImageState().opacity );
	} else if (m_ViewerCore->getCurrentImage()->getImageState().imageType == ImageHolder::z_map ) {
		ui.upperThreshold->setVisible(true);
		ui.lowerThreshold->setVisible(true);
		ui.maxLabel->setVisible( true );
		ui.minLabel->setVisible( true );
		ui.maxValueLabel->setVisible( true );
		ui.minValueLabel->setVisible( true );
		ui.opacity->setVisible( false );
		ui.labelOpacity->setVisible( false );
	}
	if( m_ViewerCore->widgetsAreIntitialized() ) {
		m_ViewerCore->updateScene();
	}
}

void MainWindow::openImage()
{
	std::stringstream fileFormats;
	fileFormats << "Image files (" << getFileFormatsAsString(std::string("*.") ) << ")";
	QStringList filenames = QFileDialog::getOpenFileNames( this,
							tr( "Open images" ),
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
		m_ViewerCore->addImageList( imgList, ImageHolder::anatomical_image, true );
		m_ViewerCore->updateScene( isFirstImage );
	}
}

void MainWindow::openDICOMDir()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr( "Open directory" ), m_CurrentPath);
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
		m_ViewerCore->getDataContainer().at( item->text().toStdString() )->setVisible( true );

	} else if( item->checkState() == Qt::Unchecked ) {
		m_ViewerCore->getDataContainer().at( item->text().toStdString() )->setVisible( false );
	}

	m_ViewerCore->updateScene();

}

void MainWindow::exitProgram()
{
	close();
}

void MainWindow::opacityChanged( int opacity )
{
	float opacityFloat = 1.0 / ( ui.opacity->maximum() - ui.opacity->minimum()) * opacity;
	m_ViewerCore->getCurrentImage()->setOpacity( opacityFloat );
	m_ViewerCore->updateScene();
}



void MainWindow::lowerThresholdChanged( int lowerThreshold )
{

	if( m_ViewerCore->getCurrentImage()->getImageState().imageType == ImageHolder::z_map ) {
		m_ViewerCore->getCurrentImage()->setLowerThreshold( ( m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() / 1000 ) * ( lowerThreshold  ) );
	} else {
		double range = m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>() - m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>();
		m_ViewerCore->getCurrentImage()->setLowerThreshold( ( range / 1000 ) * ( lowerThreshold  ) + m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() );
	}
	
	m_ViewerCore->updateScene();
}

void MainWindow::upperThresholdChanged( int upperThreshold )
{
	if( m_ViewerCore->getCurrentImage()->getImageState().imageType == ImageHolder::z_map ) {
		m_ViewerCore->getCurrentImage()->setUpperThreshold( (  m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>() / 1000 ) * ( upperThreshold  ) );	} else {
		double range = m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>() - m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>();
		m_ViewerCore->getCurrentImage()->setUpperThreshold( ( range / 1000 ) * ( upperThreshold + 1 )  + m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() );
	}

	m_ViewerCore->updateScene();

}

void MainWindow::interpolationChanged( int index )
{
	util::Singletons::get<GL::GLTextureHandler, 10>().forceReloadingAllOfType( ImageHolder::anatomical_image, static_cast<GL::GLTextureHandler::InterpolationType>( index ) );
	util::Singletons::get<GL::GLTextureHandler, 10>().forceReloadingAllOfType( ImageHolder::z_map, static_cast<GL::GLTextureHandler::InterpolationType>( index ) );
	m_ViewerCore->updateScene();
}

void MainWindow::assembleViewInRows( )
{
	std::list< boost::shared_ptr< ImageHolder> > zMaps;
	std::list< boost::shared_ptr< ImageHolder> > relevantImages;
	std::list< boost::shared_ptr< ImageHolder> > anatomicalImages;
	BOOST_FOREACH( DataContainer::const_reference dataSet, m_ViewerCore->getDataContainer() ) 
	{
		if( dataSet.second->getImageState().imageType == ImageHolder::z_map ) {
			zMaps.push_back( dataSet.second );
		} else if ( dataSet.second->getImageState().imageType == ImageHolder::anatomical_image ) {
			anatomicalImages.push_back( dataSet.second );
		}
	}
	if( !anatomicalImages.size() ) {
		LOG(Runtime, warning) << "Could not find any anatomical image.";
	}
	if(zMaps.size()) {
		relevantImages = zMaps;
	} else {
		relevantImages = anatomicalImages;
	}
	//some gui related modifications	
	ui.gridLayout->addWidget( ui.coronalDockWidget, 0, 2 );
	ui.setupDockWidget->setVisible( false );
	ui.bottomCoordsFrame->setVisible( true );
	//we create our widgets dynamically so we do not need the static ones
	ui.sagittalDockWidget->setVisible(false);
	ui.coronalDockWidget->setVisible(false);
	ui.axialDockWidget->setVisible(false);
	
	unsigned short row = 0;
	BOOST_FOREACH( std::list< boost::shared_ptr< ImageHolder> >::const_reference image, relevantImages ) 
	{
		row++;
		//setup dockwidgets
		QDockWidget *axialDock = new QDockWidget( this );
		QDockWidget *sagittalDock = new QDockWidget( this );
		QDockWidget *coronalDock = new QDockWidget( this );
		GL::QGLWidgetImplementation* axialWidget = createADockWidget( axialDock, axial, row );
		GL::QGLWidgetImplementation* sagittalWidget = createADockWidget( sagittalDock, sagittal, row );
		GL::QGLWidgetImplementation* coronalWidget = createADockWidget( coronalDock, coronal, row );
		axialWidget->addImage( image );
		sagittalWidget->addImage( image );
		coronalWidget->addImage( image );
		if(zMaps.size()) {
			axialWidget->addImage( anatomicalImages.front() );
			sagittalWidget->addImage( anatomicalImages.front() );
			coronalWidget->addImage( anatomicalImages.front() );
		}
		//setup title
		std::stringstream title;
		QFileInfo dir(tr( image->getFileNames().front().c_str()));
		title << dir.fileName().toStdString() << " (";
		title <<  image->getImage()->getPropertyAs<std::string>("sequenceDescription") << ")";
		axialDock->setWindowTitle( tr( title.str().c_str()));
		ui.currentImageBox->addItem( tr( title.str().c_str()) );
		ui.gridLayout->addWidget( axialDock, row, 0 );
		ui.gridLayout->addWidget( sagittalDock, row, 1 );
		ui.gridLayout->addWidget( coronalDock, row, 2 );
	}	
	ui.currentImageBox->setCurrentIndex( m_ViewerCore->getCurrentImage()->getID() );
}


GL::QGLWidgetImplementation* MainWindow::createADockWidget(QDockWidget* widget, PlaneOrientation orientation, unsigned short index )
{
	widget->setFloating( false );
	widget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
	widget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	QFrame *frame = new QFrame( widget );
	widget->setWidget( frame );
	GL::QGLWidgetImplementation *view = m_MasterWidget->createSharedWidget( frame, orientation );
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
