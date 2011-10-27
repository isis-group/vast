#include "qviewercore.hpp"


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


void QViewerCore::addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType )
{
	isis::viewer::ViewerCoreBase::addImageList( imageList, imageType );
	settingsChanged();

}

void QViewerCore::setImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType  )
{
	isis::viewer::ViewerCoreBase::setImageList( imageList, imageType );
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

	if( getCurrentImage()->getImageProperties().imageType == ImageHolder::z_map ) {
		getCurrentImage()->getPropMap().setPropertyAs<std::string>( "lut", getSettings()->value( "lut", "fallback" ).toString().toStdString() );
	}

	BOOST_FOREACH( WidgetMap::reference widget, m_WidgetMap ) {
		widget.second->setInterpolationType( static_cast<InterpolationType>( getSettings()->value( "interpolationType", "standard_grey_values" ).toUInt() ) );
	}
	getSettings()->endGroup();
}

void QViewerCore::updateScene( bool center )
{
	emitUpdateScene( center );
}

void QViewerCore::setAutomaticScaling( bool s )
{
#warning implement this
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
		LOG( Runtime, error )
				<< "Core does not own a parent. Before calling addPlugin/addPlugins you have to use setParentWidget!";
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


bool QViewerCore::callPlugin( QString name )
{
	BOOST_FOREACH( PluginListType::const_reference plugin, m_PluginList ) {
		if( plugin->getName() == name.toStdString() ) {
			return plugin->call();
		}
	}
	LOG( Runtime, error ) << "No such plugin " << name.toStdString() << "!";
	return false;
}



}
}
