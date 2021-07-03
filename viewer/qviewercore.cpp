/****************************************************************
 *
 * <Copyright information>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Author: Erik Tuerke, tuerke@cbs.mpg.de
 *
 * qviewercore.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "qviewercore.hpp"
#include <isis/core/io_factory.hpp>
#include <isis/core/fileptr.hpp>
#include "nativeimageops.hpp"
#include "uicore.hpp"
#include "../widgets/mainwindow.hpp"

#include <fstream>
#include <QMessageBox>

namespace isis::viewer
{

QViewerCore::QViewerCore ()
	: ViewerCoreBase( ),
	  m_CurrentPath ( QDir::currentPath().toStdString() ),
	  m_UI ( new isis::viewer::UICore ( this ) )
{
	QApplication::setStartDragTime ( 1000 );

	setParentWidget ( m_UI->getMainWindow() );
	operation::NativeImageOps::setViewerCore( this );
	m_Settings->load();
	getUICore()->refreshUI();

	checkForErrors();
}

void QViewerCore::checkForErrors()
{
	getSettings()->getQSettings()->beginGroup( "ErrorHandling" );
	const bool vastExitedSuccessfully = getSettings()->getQSettings()->value( "vastExitedSuccessfully", true ).toBool();

	if( !vastExitedSuccessfully && getSettings()->getValueAs<bool>( "showCrashMessage" ) ) {
		QMessageBox msgBox;
		msgBox.setWindowFlags( Qt::WindowSystemMenuHint );

		QSpacerItem *horizontalSpacer = new QSpacerItem( 700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding );
		QGridLayout *layout = ( QGridLayout * )msgBox.layout();
		layout->addItem( horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount() );
		msgBox.setIcon( QMessageBox::Information );
		std::stringstream text;
		text << "vast crashed last time!";
		msgBox.setText( text.str().c_str() );
		msgBox.setInformativeText( "Below you see the developer information. Feel free to copy it and send it to the vast developer." );
		std::stringstream detailedText;
		detailedText << "Developer information: " << std::endl;
		data::FilePtr logFile( getCrashLogFilePath().c_str() );

		if( !logFile.good() ) {
			QMessageBox errorMessage;
			errorMessage.setIcon( QMessageBox::Critical );
			errorMessage.setText( "Could not open the crash log file!" );
			errorMessage.exec();
		} else {
			msgBox.setDetailedText( reinterpret_cast<const char *>( &logFile.at<uint8_t>( 0 )[0] ) ); //do not do this at home!!
			msgBox.setStandardButtons( QMessageBox::Ok );
			msgBox.exec();
		}

	} else {
		getSettings()->getQSettings()->setValue( "vastExitedSuccessfully", false );
		getSettings()->getQSettings()->sync();
	}

	getSettings()->getQSettings()->endGroup();
}

void QViewerCore::onWidgetClicked ( widget::WidgetInterface *origin, util::fvector3 physicalCoords, Qt::MouseButton mouseButton )
{
	ImageHolder::ImageType iType;

	if( getMode() == statistical_mode ) {
		iType = ImageHolder::statistical_image;
	} else {
		iType = ImageHolder::structural_image;
	}

	if( !origin->getWidgetEnsemble()->isCurrent() ) {
		setCurrentImage( origin->getWidgetEnsemble()->getFirstImageOfType( iType ) );
	}

	physicalCoordsChanged( physicalCoords );
	emitOnWidgetClicked( physicalCoords, mouseButton );
}

void QViewerCore::onWidgetMoved ( widget::WidgetInterface* /*origin*/, util::fvector3 physicalCoords, Qt::MouseButton mouseButton )
{
	physicalCoordsChanged( physicalCoords );
	emitOnWidgetMoved( physicalCoords, mouseButton );
}


void QViewerCore::timestepChanged ( int timestep )
{
	if ( hasImage() ) {
		if ( !getCurrentImage()->getImageSize() [3] > timestep ) {
			timestep = getCurrentImage()->getImageSize() [3] - 1;
		}

		for ( const auto &image: getImageVector() ) {
			if ( static_cast<size_t> ( timestep ) < image->getImageSize() [3] ) {
				image->getImageProperties().timestep = timestep;
			}
		}
		emitPhysicalCoordsChanged( getCurrentImage()->getImageProperties().physicalCoords );
	}
}

ImageHolder::Vector QViewerCore::addImageList ( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType )
{
	ImageHolder::Vector retList = isis::viewer::ViewerCoreBase::addImageList ( imageList, imageType );
	return retList;

}

void QViewerCore::centerImages ( bool ca )
{
	if ( hasImage() ) {
		if ( !ca ) {
			auto center = getCurrentImage()->getImageSize()/2;
			center[3] = getCurrentImage()->getImageProperties().timestep;
			getCurrentImage()->getImageProperties().voxelCoords = center;
		} else {
			getCurrentImage()->getImageProperties().physicalCoords = util::fvector3();
		}

		updateScene();
	}
}


void QViewerCore::setShowLabels ( bool l )
{
	getSettings()->setValueAs<bool> ( "showLabels", l );
	emitShowLabels ( l );
	updateScene();
}

void QViewerCore::setShowCrosshair ( bool c )
{
	getSettings()->setValueAs<bool> ( "showCrosshair", c );
	emitSetEnableCrosshair ( c );
	updateScene();
}


void QViewerCore::settingsChanged()
{
	BOOST_FOREACH ( WidgetEnsembleComponent::Map::const_reference widget, getUICore()->getWidgets() ) {
		widget.first->setInterpolationType ( static_cast<InterpolationType> ( getSettings()->getValueAs<uint16_t> ( "interpolationType" ) ) );
	}
	emitShowLabels ( getSettings()->getValueAs<bool> ( "showLabels" ) );
	m_UI->getMainWindow()->getInterface().actionPropagate_Zooming->setChecked ( getSettings()->getValueAs<bool> ( "propagateZooming" ) );

	if ( hasImage() ) {
		if ( getCurrentImage()->getImageProperties().imageType == ImageHolder::statistical_image ) {
			getCurrentImage()->getImageProperties().lut = getSettings()->getValueAs<std::string> ( "lutZMap" );
		} else {
			getCurrentImage()->getImageProperties().lut = getSettings()->getValueAs<std::string> ( "lutStructural" );
		}

		if ( getMode() == ViewerCoreBase::statistical_mode && getCurrentAnatomicalRefernce().get() ) {
			getCurrentAnatomicalRefernce()->getImageProperties().lut = getSettings()->getValueAs<std::string> ( "lutStructural" );
			getCurrentAnatomicalRefernce()->updateColorMap();
		}

		getCurrentImage()->updateColorMap();
	}

	if ( getMode() == ViewerCoreBase::statistical_mode ) {
		BOOST_FOREACH ( ImageHolder::Vector::const_reference image, getImageVector() ) {
			if ( image->getImageProperties().imageType == ImageHolder::structural_image ) {
				image->getImageProperties().lut = getSettings()->getValueAs<std::string> ( "lutStructural" );
				image->updateColorMap();
			}
		}
	}

	if ( getMode() == ViewerCoreBase::statistical_mode && getSettings()->getValueAs<bool> ( "zmapGlobal" ) ) {
		BOOST_FOREACH ( ImageHolder::Vector::const_reference image, getImageVector() ) {
			if ( image->getImageProperties().imageType == ImageHolder::statistical_image ) {
				image->getImageProperties().lut = getSettings()->getValueAs<std::string> ( "lutZMap" );
				image->updateColorMap();
			}
		}
	}

	BOOST_FOREACH( ImageHolder::Vector::const_reference image, getImageVector() ) {
		image->updateOrientation();
	}
	if( m_Settings->getValueAs<bool>("ignoreOrientationAlways") ) {
		getUICore()->getMainWindow()->ignoreOrientation(true);
	}
	updateScene();
	m_UI->refreshUI();
}
void QViewerCore::physicalCoordsChanged ( util::fvector3 physicalCoords )
{
	BOOST_FOREACH( ImageHolder::Vector::const_reference image, getImageVector() ) {
		image->phyisicalCoordsChanged( physicalCoords );
	}
	emitPhysicalCoordsChanged( physicalCoords );
}

void QViewerCore::updateScene()
{
	emitUpdateScene();
}

void QViewerCore::zoomChanged ( float zoomFactor )
{
	if ( getSettings()->getValueAs<bool> ( "propagateZooming" ) ) {
		emitZoomChanged( zoomFactor );
	}
}

void QViewerCore::addPlugin ( std::shared_ptr< plugin::PluginInterface > plugin )
{
	if ( !m_Parent && plugin->isGUI() ) {
		LOG ( Runtime, error )
				<< "Core does not own a parent. Before calling addPlugin/addPlugins you have to use setParentWidget!";
	} else {
		plugin->setViewerCore ( this );
		plugin->setParentWidget ( m_Parent );
		m_PluginList.push_back ( plugin );
	}
}

void QViewerCore::addPlugins ( isis::viewer::plugin::PluginLoader::PluginListType plugins )
{
	BOOST_FOREACH ( plugin::PluginLoader::PluginListType::const_reference plugin, plugins ) {
		addPlugin ( plugin );
	}
}


bool QViewerCore::callPlugin ( QString name )
{
	BOOST_FOREACH ( plugin::PluginLoader::PluginListType::const_reference plugin, m_PluginList ) {
		if ( plugin->getName() == name.toStdString() ) {
			return plugin->call();
		}
	}
	LOG ( Runtime, error ) << "No such plugin " << name.toStdString() << "!";
	return false;
}



ImageHolder::Vector QViewerCore::openFile ( const FileInformation &fileInfo, bool show )
{
	if ( !fileInfo.filename().empty() ) {
		FileInformation _fileInfo = fileInfo;
		util::istring dialect = _fileInfo.getDialect();
		LOG( Dev, info ) << "Opening path " << std::filesystem::canonical(fileInfo) << " with rdialect: "
						 << _fileInfo.getDialect() << ", rf: " << _fileInfo.getReadFormat()
						 << ", widget: " << _fileInfo.getWidgetIdentifier();

		QDir dir( _fileInfo.c_str() );

		std::filesystem::path p = std::filesystem::canonical(_fileInfo);

		getUICore()->toggleLoadingIcon( true, QString( "Opening image \"" ) + QString( p.filename().c_str() ) + QString( "\"..." ) );

		setCurrentPath ( p.parent_path().string() );

		//this is a vista thing. if we load a vista image and the option "visualizeOnlyFirstVista" is enabled we should do so
//		if( std::filesystem::extension( p ) == std::string( "v" ) && getSettings()->getValueAs<bool>( "visualizeOnlyFirstVista" ) && !dialect.size() ) {
//			dialect = util::istring( "onlyfirst" );
//		}

		//load the file into an isis image
		std::list<data::Image> tempImgList = isis::data::IOFactory::load ( _fileInfo, {_fileInfo.getReadFormat()}, {dialect} );

		if( !tempImgList.empty() ) {
			//add this file to the recent opened files
			m_Settings->getRecentFiles().insertSave( _fileInfo );
			LOG( Dev, info ) << "Loaded " << tempImgList.size() << " images from path " << _fileInfo;
		} else {
			LOG( Dev, warning ) << "Tried to load " << _fileInfo << ", but image list is empty.";
			getUICore()->toggleLoadingIcon( false );
			return ImageHolder::Vector();
		}

		//creating the viewer image objects
		getUICore()->toggleLoadingIcon( true, "Preparing image for visualization..." );
		ImageHolder::Vector imgList = addImageList( tempImgList, _fileInfo.getImageType() );
		getUICore()->toggleLoadingIcon( false );

		if( show ) {
			for( const auto &image: imgList ) {
				if( _fileInfo.isNewEnsemble() ) {
					getUICore()->createViewWidgetEnsemble( _fileInfo.getWidgetIdentifier(), image, true );
				} else {
					if( !getUICore()->getEnsembleList().size() ) {
						getUICore()->createViewWidgetEnsemble( _fileInfo.getWidgetIdentifier(), image, true );
					} else {
						getUICore()->getCurrentEnsemble()->addImage( image );
					}
				}

				setCurrentImage( imgList.front() );
				physicalCoordsChanged( getCurrentImage()->getImageProperties().physicalCoords );
			}
		}

		getUICore()->toggleLoadingIcon( false );
		return imgList;
	} else {
		LOG( Dev, warning ) << "Tried to open path without any given filename!";
		return ImageHolder::Vector();
	}

	//  getUICore()->refreshUI();

}
void QViewerCore::openFileList( const std::list< FileInformation > fileInfoList )
{
	if( fileInfoList.empty() ) {
		LOG( Dev, warning ) << "Trying to open an empty file info list. Abort!";
		return;
	}

	ImageHolder::Vector structuralImageList;
	ImageHolder::Vector statisticalImageList;
	for( const auto &file: fileInfoList ) {
		ImageHolder::Vector imageList = openFile( file, false );
		for( const auto &image: imageList ) {
			if( file.getImageType() == ImageHolder::statistical_image ) {
				statisticalImageList.push_back( image );
			} else {
				structuralImageList.push_back( image );
			}
		}
	}
	WidgetEnsemble::Vector widgetList = getUICore()->getEnsembleList();

	// in statistical_mode we ignore the newEnsemble parameter and open as many ensembles as we have statistical images
	// we also ignore the amount of structural images, taking only the first and using it to underlay it
	if( getMode() == statistical_mode ) {
		if ( statisticalImageList.size() ) {
			widgetList = getUICore()->createViewWidgetEnsembleList( fileInfoList.front().getWidgetIdentifier(), statisticalImageList, true );

			if ( structuralImageList.size() ) {
				ImageHolder::Vector::iterator iIter = structuralImageList.begin();
				unsigned short structuralIndex = 0;

				for( WidgetEnsemble::Vector::const_iterator wIter = widgetList.begin(); wIter != widgetList.end(); wIter++ ) {
					( *wIter )->addImage( *iIter );

					if( ++structuralIndex < structuralImageList.size() ) {
						iIter++;
					}
				}
			}

			setCurrentImage( statisticalImageList.front() );
		} else if ( structuralImageList.size() ) {
			for( const auto &image: structuralImageList ) {
				getUICore()->createViewWidgetEnsemble( fileInfoList.front().getWidgetIdentifier(), image, true );
			}
			setCurrentImage( structuralImageList.front() );
		}
	} else {
		if ( !fileInfoList.front().isNewEnsemble() ) {
			if ( widgetList.empty() ) {
				widgetList.push_back( getUICore()->createViewWidgetEnsemble( fileInfoList.front().getWidgetIdentifier() ) );
			}

			for( const auto &image: structuralImageList ) {
				getUICore()->getCurrentEnsemble()->addImage( image );
			}
		} else {
			getUICore()->createViewWidgetEnsembleList( fileInfoList.front().getWidgetIdentifier(), structuralImageList, true );
		}

		if( !structuralImageList.empty() ) {
			setCurrentImage( structuralImageList.front() );
		}

	}

	if( hasImage() ) {
		physicalCoordsChanged( getCurrentImage()->getImageProperties().physicalCoords );
	}
}

void QViewerCore::closeImage ( ImageHolder::Pointer image, bool refreshUI )
{
	for( auto &ensemble: getUICore()->getEnsembleList() ) {
		ensemble->removeImage( image );
	}

	const bool ok = removeImage( image );

	if( ok ) {
		LOG( Dev, info ) << "Successfully removed image.";
	} else {
		LOG( Dev, error ) << "Error during removing of image " << image->getImageProperties().fileName;
	}

	updateScene();

	if( refreshUI ) {
		getUICore()->refreshUI( false );
	}
}


void QViewerCore::close ()
{
	getSettings()->getQSettings()->beginGroup( "ErrorHandling" );
	getSettings()->getQSettings()->setValue( "vastExitedSuccessfully", true );
	getSettings()->getQSettings()->sync();
	getSettings()->getQSettings()->endGroup();
}
void QViewerCore::images_loaded(isis::qt5::IsisImageList images, QStringList rejects)
{

}

}
