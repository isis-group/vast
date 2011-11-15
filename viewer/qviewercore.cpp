#include "qviewercore.hpp"
#include <DataStorage/io_factory.hpp>
#include "nativeimageops.hpp"
#include "uicore.hpp"

namespace isis
{
namespace viewer
{

QViewerCore::QViewerCore( const std::string &appName, const std::string &orgName, QWidget *parent )
	: ViewerCoreBase( ),
	  m_Parent( parent ),
	  m_Settings( new QSettings( appName.c_str(), orgName.c_str() ) ),
	  m_CurrentPath( QDir::currentPath().toStdString() ),
	  m_ProgressFeedback( boost::shared_ptr<QProgressFeedback>( new QProgressFeedback() ) ),
	  m_UI( new isis::viewer::UICore( this ) )
{
	QCoreApplication::setApplicationName( QString( appName.c_str() ) );
	QCoreApplication::setOrganizationName( QString( orgName.c_str() ) );


	setParentWidget( m_UI->getMainWindow() );
	data::IOFactory::setProgressFeedback( m_ProgressFeedback );
	operation::NativeImageOps::setProgressFeedBack( m_ProgressFeedback );

}


void QViewerCore::addMessageHandler( qt4::QDefaultMessagePrint *handler )
{
	connect( handler, SIGNAL( commitMessage( qt4::QMessage ) ) , this, SLOT( receiveMessage( qt4::QMessage ) ) );
}

void QViewerCore::receiveMessage( qt4::QMessage message )
{
	m_MessageLog.push_back( message );
	getUI()->showMessage( message );
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
	getCurrentImage()->voxelCoords[3] = timestep;
	updateScene();
}


std::list<boost::shared_ptr<ImageHolder> > QViewerCore::addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType )
{
	std::list<boost::shared_ptr<ImageHolder> > retList = isis::viewer::ViewerCoreBase::addImageList( imageList, imageType );
	return retList;

}

void QViewerCore::setImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType  )
{
	isis::viewer::ViewerCoreBase::setImageList( imageList, imageType );


}
void QViewerCore::centerImages()
{
	if( hasImage() ) {
		const util::ivector4 size = getCurrentImage()->getImageSize();
		const util::ivector4 center( size[0] / 2, size[1] / 2, size[2] / 2,
										 getCurrentImage()->voxelCoords[3] );
		getCurrentImage()->voxelCoords = center;
		m_UI->refreshUI();
		updateScene();
	}
}


void QViewerCore::setShowLabels( bool l )
{
	getOptionMap()->setPropertyAs<bool>( "showLabels", l );
	emitShowLabels( l );
	updateScene();
}

void QViewerCore::setShowCrosshair(bool c )
{
	getOptionMap()->setPropertyAs<bool>( "showCrosshair", c );
	emitSetEnableCrosshair( c );
	updateScene();
}


void QViewerCore::settingsChanged()
{
	getSettings()->beginGroup( "UserProfile" );

	if( hasImage() ) {
		if( getCurrentImage()->imageType == ImageHolder::z_map ) {
			getCurrentImage()->lut = getSettings()->value( "lut", "fallback" ).toString().toStdString();
		}
	}

	BOOST_FOREACH( UICore::WidgetMap::const_reference widget, getUI()->getWidgets() ) {
		widget.first->setInterpolationType( static_cast<InterpolationType>( getSettings()->value( "interpolationType", 0 ).toUInt() ) );
	}
	emitShowLabels( getOptionMap()->getPropertyAs<bool>( "showLabels" ) );
	m_UI->getMainWindow()->getUI().actionPropagate_Zooming->setChecked( getOptionMap()->getPropertyAs<bool>( "propagateZooming" ) );
	getSettings()->endGroup();
	m_UI->refreshUI();
}

void QViewerCore::updateScene()
{
	emitUpdateScene();
}

void QViewerCore::zoomChanged( float zoomFactor )
{
	if( m_OptionsMap->getPropertyAs<bool>( "propagateZooming" ) ) {
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

void QViewerCore::openPath( QStringList fileList, ImageHolder::ImageType imageType, const std::string &rdialect, const std::string &rf, bool newWidget )
{
	if( !fileList.empty() ) {
		QDir dir;
		setCurrentPath( dir.absoluteFilePath( fileList.front() ).toStdString() );
		std::list<data::Image> imgList;
		util::slist pathList;

		if( ( getDataContainer().size() + fileList.size() ) > 1 ) {
			getUI()->setViewWidgetArrangement( isis::viewer::UICore::InRow );
		} else {
			getUI()->setViewWidgetArrangement( isis::viewer::UICore::Default );
		}
		UICore::ViewWidgetEnsembleType ensemble;
		if( getUI()->getEnsembleList().size() ) {
			ensemble = getUI()->getEnsembleList().front();
		}

		BOOST_FOREACH( QStringList::const_reference filename, fileList ) {
			std::stringstream msg;
			boost::filesystem::path p( filename.toStdString() );

			if( boost::filesystem::is_directory( p ) ) {
				msg << "Loading images from directory \"" << p.filename() << "\"...";
			} else {
				msg << "Loading image \"" << p.filename() << "\"...";
			}

			getUI()->setShowWorkingLabel( msg.str() );
			std::list<data::Image> tempImgList = isis::data::IOFactory::load( filename.toStdString() , rf, rdialect );
			pathList.push_back( filename.toStdString() );

			if( tempImgList.size() > 1 ) {
				msg.clear();
				msg << "Found " << tempImgList.size() << " images. Loading...";
				getUI()->setShowWorkingLabel( msg.str() );
			}

			BOOST_FOREACH( std::list<data::Image>::const_reference image, tempImgList ) {
				imgList.push_back( image );
				boost::shared_ptr<ImageHolder> imageHolder = addImage( image, imageType );

				if( newWidget ) {
					ensemble = getUI()->createViewWidgetEnsemble( "" );
				}

				attachImageToWidget( imageHolder, ensemble[0].widgetImplementation );
				attachImageToWidget( imageHolder, ensemble[1].widgetImplementation );
				attachImageToWidget( imageHolder, ensemble[2].widgetImplementation );
				setCurrentImage( imageHolder );
			}
		}
		getUI()->setShowWorkingLabel( "", false );
		getUI()->rearrangeViewWidgets();
		getUI()->refreshUI();
		centerImages();
		
	}
}




}
}
