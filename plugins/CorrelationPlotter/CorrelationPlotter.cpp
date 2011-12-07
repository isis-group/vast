#include "CorrelationPlotter.hpp"


isis::viewer::plugin::CorrelationPlotterDialog::CorrelationPlotterDialog(QWidget* parent, isis::viewer::QViewerCore* core)
	: QDialog(parent),
	m_ViewerCore( core ),
	m_OrigMode( core->getMode() )
{
	m_Interface.setupUi( this );
	m_Interface.correlationType->addItem( "linear" );
	m_Interface.correlationType->setCurrentIndex(0);
	m_Interface.lock->setChecked( false );
	connect( m_Interface.lock, SIGNAL( clicked(bool)), this, SLOT( lockClicked()));
	

}

void isis::viewer::plugin::CorrelationPlotterDialog::lockClicked()
{
	if( m_Interface.lock->isChecked() )  {
		calculateCorrelation(true);
	}
}


void isis::viewer::plugin::CorrelationPlotterDialog::physicalCoordsChanged(isis::util::fvector4 physicalCoords)
{
	if( !m_Interface.lock->isChecked() ) {
		m_CurrentVoxelPos = m_CurrentFunctionalImage->voxelCoords;
		calculateCorrelation();
		m_ViewerCore->updateScene();
	}	
}



void isis::viewer::plugin::CorrelationPlotterDialog::closeEvent(QCloseEvent* )
{
	disconnect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged(util::fvector4)), this, SLOT( physicalCoordsChanged(util::fvector4)));
	if( m_CurrentFunctionalImage ) {
		calculateCorrelation(true);
	}
	m_ViewerCore->setMode( m_OrigMode );
	m_ViewerCore->getUI()->refreshUI();
	
}


void isis::viewer::plugin::CorrelationPlotterDialog::showEvent(QShowEvent* )
{
	if( m_ViewerCore->hasImage()) {
		bool hasFunctionalImage = false;
		if( !m_CurrentFunctionalImage ) {
			if( m_ViewerCore->getCurrentImage()->getImageSize()[3] == 1) {
				BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) {
					if( image.second->getImageSize()[3] > 1 ) {
						hasFunctionalImage = true;
						m_CurrentFunctionalImage = image.second;
					}
				}
			} else {
				hasFunctionalImage = true;
				m_CurrentFunctionalImage = m_ViewerCore->getCurrentImage();
			}
		} else {
			hasFunctionalImage = true;
		}
		if( !hasFunctionalImage ) {
			disconnect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged(util::fvector4)), this, SLOT( physicalCoordsChanged(util::fvector4)));
			LOG( Runtime, warning ) << "Can not find any functional dataset. Won´t calculate correlation map";
			QMessageBox msgBox;
			msgBox.setText( "Can not find any functional dataset. Won´t calculate correlation map!" );
			msgBox.exec();
			return;
		} else {
			if ( !m_CurrentCorrelationMap ) {
				createCorrelationMap() ;
				BOOST_FOREACH( UICore::ViewWidgetEnsembleListType::const_reference ensemble, m_ViewerCore->getUI()->getEnsembleList() ) {
					WidgetInterface::ImageVectorType iVector;
					for( unsigned short i = 0; i < 3; i++ ) {
						iVector = ensemble[i].widgetImplementation->getImageVector();
						if( std::find( iVector.begin(), iVector.end(), m_CurrentFunctionalImage ) != iVector.end() ) {
							m_ViewerCore->attachImageToWidget( m_CurrentCorrelationMap, ensemble[i].widgetImplementation ) ;
						}
					}
				}
			}			
			connect( m_ViewerCore, SIGNAL( emitPhysicalCoordsChanged(util::fvector4)), this, SLOT( physicalCoordsChanged(util::fvector4)));

			if( m_ViewerCore->getMode() != ViewerCoreBase::zmap ) {
				m_ViewerCore->setMode( ViewerCoreBase::zmap );	
			}
			m_ViewerCore->getUI()->refreshUI();
			
		}
		physicalCoordsChanged( m_CurrentFunctionalImage->physicalCoords );
	}

}


bool isis::viewer::plugin::CorrelationPlotterDialog::createCorrelationMap()
{
	if( m_CurrentFunctionalImage ) {
		if( m_CurrentFunctionalImage->getImageSize()[3] > 1 ) {
			const util::ivector4 size = m_CurrentFunctionalImage->getImageSize();
			isis::data::MemChunk<MapImageType> ch( size[0], size[1], size[2] );		
			
			ch.join( static_cast<isis::util::PropertyMap&>( *m_CurrentFunctionalImage->getISISImage() ) );	
			if( !ch.hasProperty("acquisitionNumber") ) {
				ch.setPropertyAs<uint16_t>("acquisitionNumber", 0 );
			}
			isis::data::Image corrMap ( ch );
			corrMap.setPropertyAs<std::string>("source", "correlation_map" );

			m_CurrentCorrelationMap = m_ViewerCore->addImage( corrMap, ImageHolder::z_map );
			m_CurrentCorrelationMap->lut = std::string("standard_zmap");
			m_CurrentCorrelationMap->minMax.first = util::Value<MapImageType>(-1);
			m_CurrentCorrelationMap->minMax.second = util::Value<MapImageType>(1);
			m_CurrentCorrelationMap->internMinMax.first = util::Value<MapImageType>(-1);		
			m_CurrentCorrelationMap->internMinMax.second = util::Value<MapImageType>(1);
			m_CurrentCorrelationMap->extent = m_CurrentCorrelationMap->minMax.second->as<double>() -  m_CurrentCorrelationMap->minMax.first->as<double>();
			isis::data::ValuePtr<InternalFunctionalImageType> imagePtr( ( InternalFunctionalImageType * ) 
						calloc( m_CurrentFunctionalImage->getISISImage()->getVolume(), sizeof( InternalFunctionalImageType ) ), m_CurrentFunctionalImage->getISISImage()->getVolume() );
			m_CurrentFunctionalImage->getISISImage()->copyToMem<InternalFunctionalImageType>( &imagePtr[0], m_CurrentFunctionalImage->getISISImage()->getVolume() );	
			m_InternalChunk.reset( new isis::data::Chunk( imagePtr, size[0], size[1], size[2], size[3] ) );
			
			return true;
		}
	}
	return false;
}


void isis::viewer::plugin::CorrelationPlotterDialog::calculateCorrelation(bool all)
{	
	const size_t vol = m_CurrentFunctionalImage->getImageSize()[0] * m_CurrentFunctionalImage->getImageSize()[1] * m_CurrentFunctionalImage->getImageSize()[2];
	const size_t n = m_CurrentFunctionalImage->getImageSize()[3];
	double sum_x = 0;
	double sum_quad_x = 0;
	InternalFunctionalImageType *vx = &m_InternalChunk->voxel<InternalFunctionalImageType>(m_CurrentVoxelPos[0], m_CurrentVoxelPos[1], m_CurrentVoxelPos[2] );
	for( size_t t = 0; t < n * vol ; t+=vol ) {
		sum_x += vx[t];
		sum_quad_x += vx[t] * vx[t];
	}
	const double _x = sum_x / n;	
	const double s_x = std::sqrt((1/float(n-1))*(sum_quad_x - n * _x * _x ));
	
	if( !all ) {
#pragma omp parallel for
		for( unsigned int z = 0; z < m_CurrentFunctionalImage->getImageSize()[2]; z++ ) {
			for( unsigned int y = 0; y < m_CurrentFunctionalImage->getImageSize()[1]; y++ ) {
				_internCalculateCorrelation( util::ivector4( m_CurrentVoxelPos[0], y,z ), s_x, _x, vx, n, vol );
			}
		}
#pragma omp parallel for
		for( unsigned int y = 0; y < m_CurrentFunctionalImage->getImageSize()[1]; y++ ) {
			for( unsigned int x = 0; x < m_CurrentFunctionalImage->getImageSize()[0]; x++ ) {
				_internCalculateCorrelation( util::ivector4( x,y,m_CurrentVoxelPos[2] ), s_x, _x, vx, n, vol );
			}
		}
#pragma omp parallel for
		for( unsigned int z = 0; z < m_CurrentFunctionalImage->getImageSize()[2]; z++ ) {
			for( unsigned int x = 0; x < m_CurrentFunctionalImage->getImageSize()[0]; x++ ) {
				_internCalculateCorrelation( util::ivector4( x, m_CurrentVoxelPos[1], z ), s_x, _x, vx, n, vol );
			}
		}	
	} else {
#pragma omp parallel for
		for( unsigned int z = 0; z < m_CurrentFunctionalImage->getImageSize()[2]; z++ ) {
			for( unsigned int y = 0; y < m_CurrentFunctionalImage->getImageSize()[1]; y++ ) {
				for( unsigned int x = 0; x < m_CurrentFunctionalImage->getImageSize()[0]; x++ ) {
					_internCalculateCorrelation( util::ivector4( x, y, z ), s_x, _x, vx, n, vol );
				}
			}
		}
	}

	

}

void isis::viewer::plugin::CorrelationPlotterDialog::_internCalculateCorrelation(const isis::util::ivector4& vec, const double& s_x, const double& _x, const InternalFunctionalImageType* vx, const size_t &n, const size_t &vol)
{
	double sum_quad_y = 0;
	double sum_y = 0;
	double sum_xy = 0;
	InternalFunctionalImageType *vy = &m_InternalChunk->voxel<InternalFunctionalImageType>( vec[0], vec[1], vec[2] );
	for ( size_t t = 0; t < n * vol; t+=vol ) {
		sum_quad_y += vy[t] * vy[t];
		sum_y += vy[t];
		sum_xy += vx[t] * vy[t];
	}
	const double _y = sum_y / n;
	const double s_xy = (1 / float( n - 1 ) ) * ( sum_xy - n * _x * _y );
	const double s_y = std::sqrt((1/ float(n-1)) * (sum_quad_y - n * _y * _y ) );
	double r_xy = s_xy / ( s_x * s_y );
	if( !std::isnan( r_xy ) ) {
		r_xy += 1;
		r_xy *= 128;
		m_CurrentCorrelationMap->getChunkVector()[0].voxel<InternalImageType>( vec[0], vec[1], vec[2] ) = r_xy;
	} else {
		m_CurrentCorrelationMap->getChunkVector()[0].voxel<InternalImageType>( vec[0], vec[1], vec[2] ) = 128;
	}

	
}


