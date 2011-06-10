#include "QViewerCore.hpp"


namespace isis
{
namespace viewer
{

QViewerCore::QViewerCore( ): ViewerCoreBase( )
{
}



bool
QViewerCore::registerWidget( std::string key, QWidget *widget, QViewerCore::Actions action )
{
	if( m_WidgetMap.find( key ) == m_WidgetMap.end() ) {
		widget->setObjectName( QString( key.c_str() ) );
		m_WidgetMap.insert( std::make_pair< std::string,  QWidget * >( key, widget ) );

		if( dynamic_cast<GL::QGLWidgetImplementation *>( widget ) ) {
			GL::QGLWidgetImplementation *w = dynamic_cast<GL::QGLWidgetImplementation *>( widget );
			connect( w, SIGNAL( voxelCoordsChanged( util::ivector4 ) ), this, SLOT( voxelCoordsChanged ( util::ivector4 ) ) );
			connect( w, SIGNAL( physicalCoordsChanged( util::fvector4 ) ), this, SLOT( physicalCoordsChanged ( util::fvector4 ) ) );
			connect( this, SIGNAL( emitVoxelCoordChanged( util::ivector4 ) ), w, SLOT( lookAtVoxel( util::ivector4 ) ) );
			connect( this, SIGNAL( emitPhysicalCoordsChanged( util::fvector4 ) ), w, SLOT( lookAtPhysicalCoords( util::fvector4 ) ) );
			connect( this, SIGNAL( emitTimeStepChange( unsigned int ) ), w, SLOT( timestepChanged( unsigned int ) ) );
			connect( this, SIGNAL( emitShowLabels( bool ) ), w, SLOT( setShowLabels( bool ) ) );
			connect( this, SIGNAL( emitUpdateScene( bool ) ), w, SLOT( updateScene( bool ) ) );
			connect( this, SIGNAL( emitSetAutomaticScaling( bool ) ), w, SLOT( setAutomaticScaling( bool ) ) );
		}
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
	util::ivector4 voxelCoords = getCurrentImage()->getImageState().voxelCoords;
	voxelCoords[3] = timestep;
	getCurrentImage()->setCurrentVoxelCoords( voxelCoords );
	getCurrentImage()->setTimestep( timestep );
	emitTimeStepChange( timestep );
}

bool QViewerCore::widgetsAreIntitialized() const
{
	BOOST_FOREACH( WidgetMap::const_reference widget, m_WidgetMap ) {
		if( !dynamic_cast<GL::QGLWidgetImplementation *>( widget.second )->isInitialized() ){
			return false;
		}
	}
	return true;

}


void QViewerCore::addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType, bool passToWidgets )
{
	isis::viewer::ViewerCoreBase::addImageList( imageList, imageType );

	if( passToWidgets ) {
		emitImagesChanged( getDataContainer() );
		BOOST_FOREACH( WidgetMap::reference widget, m_WidgetMap ) {
			BOOST_FOREACH( DataContainer::const_reference data, getDataContainer() ) {
				dynamic_cast<GL::QGLWidgetImplementation *>( widget.second )->addImage( data.second );
			}
		}
	}
	emitImagesChanged( getDataContainer() );
}

void QViewerCore::setImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType, bool passToWidgets  )
{
	isis::viewer::ViewerCoreBase::setImageList( imageList, imageType );

	if( passToWidgets ) {
		emitImagesChanged( getDataContainer() );
		BOOST_FOREACH( WidgetMap::reference widget, m_WidgetMap ) {
			BOOST_FOREACH( DataContainer::const_reference data, getDataContainer() ) {
				dynamic_cast<GL::QGLWidgetImplementation *>( widget.second )->addImage( data.second );
			}
		}
	}
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

}
}