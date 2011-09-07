#include "QViewerCore.hpp"


namespace isis
{
namespace viewer
{

QViewerCore::QViewerCore( const std::string &appName, const std::string &orgName ): ViewerCoreBase( )
{
	QCoreApplication::setApplicationName( QString( appName.c_str() ) );
	QCoreApplication::setOrganizationName( QString( orgName.c_str() ) );
	m_Settings = new QSettings( appName.c_str(), orgName.c_str() );
}



bool
QViewerCore::registerWidget( std::string key, WidgetImplementationBase *widget, QViewerCore::Actions action )
{
	if( m_WidgetMap.find( key ) == m_WidgetMap.end() ) {
		widget->setWidgetName( key );
		m_WidgetMap.insert( std::make_pair< std::string,  WidgetImplementationBase * >( key, widget ) );
	} else {
		LOG( Runtime, error ) << "A widget with the name " << key << " already exists! Wont add this";
		return false;
	}
}

void QViewerCore::voxelCoordsChanged( util::ivector4 voxelCoords )
{
	emitVoxelCoordChanged( voxelCoords );
}

void QViewerCore::physicalCoordsChanged( util::fvector4 physicalCoords )
{
	emitPhysicalCoordsChanged( physicalCoords );
}


void QViewerCore::timestepChanged( int timestep )
{
	if( !getCurrentImage()->getImageSize()[3] > timestep ) {
		timestep = getCurrentImage()->getImageSize()[3] - 1;
	}
	util::ivector4 voxelCoords = getCurrentImage()->getImageProperties().voxelCoords;
	voxelCoords[3] = timestep;
	getCurrentImage()->setCurrentVoxelCoords( voxelCoords );
	getCurrentImage()->setTimestep( timestep );
	emitTimeStepChange( timestep );
}


void QViewerCore::addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType, bool passToWidgets )
{
	std::list<boost::shared_ptr<ImageHolder> > imageHolderList = 
		isis::viewer::ViewerCoreBase::addImageList( imageList, imageType );

	if( passToWidgets ) {
		BOOST_FOREACH( WidgetMap::reference widget, m_WidgetMap ) {
			BOOST_FOREACH( std::list<boost::shared_ptr<ImageHolder> >::const_reference data, imageHolderList ) {
				widget.second->addImage( data );
			}
		}
	}
	emitImagesChanged( getDataContainer() );
}

void QViewerCore::setImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType, bool passToWidgets  )
{
	isis::viewer::ViewerCoreBase::setImageList( imageList, imageType );

	if( passToWidgets ) {
		BOOST_FOREACH( WidgetMap::reference widget, m_WidgetMap ) {
			BOOST_FOREACH( DataContainer::const_reference data, getDataContainer() ) {
				widget.second->addImage( data.second );
			}
		}
	}
	emitImagesChanged( getDataContainer() );
	
}

void QViewerCore::setShowLabels( bool l )
{
	if( l ) {
		emitShowLabels( true );
	} else {
		emitShowLabels( false );
	}
}

void QViewerCore::updateScene( bool center )
{
	emitUpdateScene( center );
}

void QViewerCore::setAutomaticScaling( bool s )
{
	emitSetAutomaticScaling( s );
}


void QViewerCore::zoomChanged(float zoomFactor)
{
	if(m_Options->propagateZooming) {
		emitZoomChanged( zoomFactor );
	}
}

}
}