#include "QViewerCore.hpp"


namespace isis
{
namespace viewer
{

QViewerCore::QViewerCore( const std::string &appName, const std::string &orgName, QWidget *parent )
	: ViewerCoreBase( ),
	m_Parent( parent )
{
	QCoreApplication::setApplicationName( QString( appName.c_str() ) );
	QCoreApplication::setOrganizationName( QString( orgName.c_str() ) );
	m_Settings = new QSettings( appName.c_str(), orgName.c_str() );
}



bool
QViewerCore::registerWidget( std::string key, QWidgetImplementationBase *widget, QViewerCore::Actions action )
{
	if( m_WidgetMap.find( key ) == m_WidgetMap.end() ) {
		widget->setWidgetName( key );
		m_WidgetMap.insert( std::make_pair< std::string,  QWidgetImplementationBase * >( key, widget ) );
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

	util::ivector4 voxelCoords = getCurrentImage()->getPropMap().getPropertyAs<util::ivector4>( "voxelCoords" );
	voxelCoords[3] = timestep;
	getCurrentImage()->getPropMap().setPropertyAs<util::ivector4>( "voxelCoords", voxelCoords );
	getCurrentImage()->getPropMap().setPropertyAs<uint16_t>( "currentTimestep", timestep );
	updateScene();
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
		emitImagesChanged( getDataContainer() );
	}

	settingsChanged();

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
		emitImagesChanged( getDataContainer() );
	}

	settingsChanged();


}

void QViewerCore::setShowLabels( bool l )
{
	if( l ) {
		emitShowLabels( true );
	} else {
		emitShowLabels( false );
	}
}

void QViewerCore::settingsChanged()
{
	getSettings()->beginGroup( "UserProfile" );
	getCurrentImage()->getPropMap().setPropertyAs<unsigned short>( "lut", getSettings()->value( "lut", 0 ).toUInt() );
	BOOST_FOREACH( WidgetMap::reference widget, m_WidgetMap ) {
		widget.second->setInterpolationType( static_cast<InterpolationType>( getSettings()->value( "interpolationType", 0 ).toUInt() ) );
	}
	getSettings()->endGroup();
}

void QViewerCore::updateScene( bool center )
{
	emitUpdateScene( center );
}

void QViewerCore::setAutomaticScaling( bool s )
{
	BOOST_FOREACH( WidgetMap::reference widget, m_WidgetMap ) {
		if( s ) {
			widget.second->setScalingType( automatic_scaling );
		} else {
			widget.second->setScalingType( no_scaling );
		}

		widget.second->updateScene( false );
	}
}


void QViewerCore::zoomChanged( float zoomFactor )
{
	if( m_Options->propagateZooming ) {
		emitZoomChanged( zoomFactor );
	}
}


void QViewerCore::addPlugin( boost::shared_ptr< plugin::PluginInterface > plugin )
{
	if( !m_Parent && plugin->isGUI() ) {
		LOG( Runtime, error ) << "Core does not own a parent. Before calling addPlugin/addPlugins you have to use setParentWidget!";
	} else {
		plugin->setViewerCore( this );
		plugin->setParentWidget( m_Parent );
		m_PluginList.push_back( plugin );
	}
}

void QViewerCore::addPlugins( isis::viewer::plugin::PluginLoader::PluginListType plugins )
{
	BOOST_FOREACH( PluginListType::const_reference plugin, plugins ) {
		addPlugin( plugin );
	}
}


bool QViewerCore::callPlugin( const std::string &name )
{
	BOOST_FOREACH( PluginListType::const_reference plugin, m_PluginList ) {
		if( plugin->getName() == name ) {
			return plugin->call();
		}
	}
	return false;
}



}
}
