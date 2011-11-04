#include "qviewercore.hpp"
#include <DataStorage/io_factory.hpp>
#include "nativeimageops.hpp"

namespace isis
{
namespace viewer
{

QViewerCore::QViewerCore( const std::string &appName, const std::string &orgName, QWidget *parent )
	: ViewerCoreBase( ),
	  m_Parent( parent ),
	  m_Settings( new QSettings( appName.c_str(), orgName.c_str() ) ),
	  m_CurrentPath( QDir::currentPath().toStdString() ),
	  m_ProgressFeedback( boost::shared_ptr<QProgressFeedback>( new QProgressFeedback() )),
	  m_UI( new isis::viewer::UICore( this ) )
{
	QCoreApplication::setApplicationName( QString( appName.c_str() ) );
	QCoreApplication::setOrganizationName( QString( orgName.c_str() ) );
	

	setParentWidget( m_UI->getMainWindow() );
	data::IOFactory::setProgressFeedback( m_ProgressFeedback );
	operation::NativeImageOps::setProgressFeedBack( m_ProgressFeedback );
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


std::list<boost::shared_ptr<ImageHolder> > QViewerCore::addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType )
{
	std::list<boost::shared_ptr<ImageHolder> > retList = isis::viewer::ViewerCoreBase::addImageList( imageList, imageType );
	settingsChanged();
	return retList;

}

void QViewerCore::setImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType  )
{
	isis::viewer::ViewerCoreBase::setImageList( imageList, imageType );
	//  settingsChanged();


}

void QViewerCore::setShowLabels( bool l )
{
	getSettings()->beginGroup( "UserProfile" );
	getSettings()->setValue("showLabels",l);
	getSettings()->endGroup();
	emitShowLabels( l );
	updateScene();
}

void QViewerCore::settingsChanged()
{	
	getSettings()->beginGroup( "UserProfile" );
	if( getCurrentImage().get() ) {
	
		if( getCurrentImage()->getImageProperties().imageType == ImageHolder::z_map ) {
			getCurrentImage()->getPropMap().setPropertyAs<std::string>( "lut", getSettings()->value( "lut", "fallback" ).toString().toStdString() );
		}
	}
	BOOST_FOREACH( UICore::WidgetMap::const_reference widget, getUI()->getWidgets() ) {
		widget.first->setInterpolationType( static_cast<InterpolationType>( getSettings()->value( "interpolationType", 0 ).toUInt() ) );
	}
	emitShowLabels( getSettings()->value("showLabels", false).toBool());
	getSettings()->endGroup();
	
}

void QViewerCore::updateScene( bool center )
{
	emitUpdateScene( center );
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

bool QViewerCore::attachImageToWidget( boost::shared_ptr<ImageHolder> image, QWidgetImplementationBase *widget )
{
	if ( getUI()->getWidgets().find( widget ) == getUI()->getWidgets().end() ) {
		LOG( Runtime, error ) << "There is no such widget "
							  << widget << ", so will not add image " << image->getFileNames().front() << " to it.";
		return false;
	}

	if( std::find( m_ImageList.begin(), m_ImageList.end(), image ) == m_ImageList.end()  ) {
		LOG( Runtime, error ) << "There is no such image "
							  << image->getFileNames().front() << ", so will not add it to widget " << widget << ".";
		return false;
	}

	widget->addImage( image );
	return true;
}



}
}
