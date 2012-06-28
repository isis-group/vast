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
 * uicore.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "uicore.hpp"
#include "qviewercore.hpp"
#include "widgetensemble.hpp"
#include <DataStorage/io_interface.h>

#include <QSignalMapper>

namespace isis
{
namespace viewer
{


UICore::UICore( QViewerCore *core )
	: m_ViewerCore( core ),
	  m_MainWindow( new MainWindow( core ) )
{
	m_VoxelInformationWidget = new ui::VoxelInformationWidget( m_MainWindow, core );
	m_SliderWidget = new ui::SliderWidget( m_MainWindow, core );
	m_ImageStackWidget = new ui::ImageStackWidget( m_MainWindow, core );
	m_VoxelInformationWidget->setVisible( false );
	m_ImageStackWidget->setVisible( false );
	setOptionPosition( );
	connect( m_MainWindow->getInterface().actionInformation_Areas, SIGNAL( triggered( bool ) ), SLOT( showInformationAreas( bool ) ) );

	m_ViewerCore->emitCurrentImageChanged.connect( boost::bind( &UICore::refreshUI, this, _1 ) );
}

void UICore::setOptionPosition( UICore::OptionPosition pos )
{
	m_VoxelInformationWidget->setVisible( true );
	m_ImageStackWidget->setVisible( true );

	switch ( pos ) {
	case bottom:
		m_MainWindow->getInterface().bottomGridLayout->addWidget( m_VoxelInformationWidget, 0, 0 );
		m_MainWindow->getInterface().bottomGridLayout->addWidget( m_ImageStackWidget, 0, 1 );
		break;
	case top:
		m_MainWindow->getInterface().topGridLayout->addWidget( m_ImageStackWidget, 0, 1 );
		m_MainWindow->getInterface().topGridLayout->addWidget( m_VoxelInformationWidget, 0, 0 );
		break;
	case central11: {
		QGridLayout *layout = new QGridLayout(  );
		layout->setVerticalSpacing( 0 );
		layout->setHorizontalSpacing( 0 );
		layout->setMargin( 0 );
		layout->setContentsMargins( 0, 0, 0, 0 );
		QFrame *frame = new QFrame( m_MainWindow );
		frame->setLayout( layout );
		m_MainWindow->getInterface().centralGridLayout->addWidget( frame, 1, 1 );
		layout->addWidget( m_VoxelInformationWidget, 0, 0 );
		layout->addWidget( m_ImageStackWidget, 1, 0 );
		break;
	}
	case left:
		break;
	case right:
		break;
	}
}

void UICore::showMainWindow( const std::list<FileInformation> &fileList )
{
	m_MainWindow->getInterface().rightGridLayout->addWidget( m_SliderWidget );
	m_MainWindow->show();

	if( fileList.empty() && m_ViewerCore->getSettings()->getPropertyAs<bool>( "showStartWidget" ) ) {
		getMainWindow()->startWidget->show();
	} else {
		m_ViewerCore->openFileList( fileList );
	}

	m_ViewerCore->settingsChanged();
}

QDockWidget *UICore::createDockingEnsemble( QWidget *widget )
{
	QDockWidget *dockWidget = new QDockWidget( m_MainWindow );
	dockWidget->setFloating( false );
	widget->setContentsMargins( 0, 0, 0, 0 );
	dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );
	dockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
	dockWidget->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum ) );
	dockWidget->setWidget( widget );
	dockWidget->setContentsMargins( 2, 0, 2, 0 );
	return dockWidget;

}

WidgetEnsemble::Pointer UICore::createViewWidgetEnsemble( const std::string &widgetIdentifier, ImageHolder::Pointer image, bool show )
{
	WidgetEnsemble::Pointer ensemble = createViewWidgetEnsemble( widgetIdentifier, show );
	ensemble->addImage( image );
	return ensemble;
}


WidgetEnsemble::Pointer UICore::createViewWidgetEnsemble( const std::string &widgetIdentifier, bool show )
{
	WidgetEnsemble::Pointer ensemble( new WidgetEnsemble );

	//if this widget has an option widget we are loading it and passing it to the ensemble
	if( m_ViewerCore->getWidgetProperties( widgetIdentifier )->hasProperty( "hasOptionWidget" ) ) {
		if( m_ViewerCore->getWidgetProperties( widgetIdentifier )->getPropertyAs<bool>( "hasOptionWidget" ) ) {
			widget::WidgetLoader::OptionDialogMapType optionMap = util::Singletons::get<widget::WidgetLoader, 10>().getOptionWidgetMap();
			const widget::WidgetLoader::OptionDialogMapType::const_iterator iter = optionMap.find( widgetIdentifier );

			if( iter != optionMap.end() ) {
				ensemble->setOptionWidget( iter->second() );  //looks strange since we are calling a funtion pointer
			}
		}
	}

	uint8_t numberWidgets;

	if( m_ViewerCore->getWidgetProperties( widgetIdentifier )->hasProperty( "numberOfEntitiesInEnsemble" ) ) {
		numberWidgets = m_ViewerCore->getWidgetProperties( widgetIdentifier )->getPropertyAs<uint8_t>( "numberOfEntitiesInEnsemble" );
	} else {
		LOG( Dev, error ) << "Your widget \"" << widgetIdentifier << "\" has no property \"numberOfEntitiesInEnsemble\" ! Setting it to 1";
		numberWidgets = 1;
	}

	if( numberWidgets == 1 ) {
		ensemble->insertComponent( createEnsembleComponent( widgetIdentifier, not_specified, ensemble ) );
	} else if ( numberWidgets == 3 ) {
		ensemble->insertComponent( createEnsembleComponent( widgetIdentifier, axial, ensemble ) );
		ensemble->insertComponent( createEnsembleComponent( widgetIdentifier, sagittal, ensemble ) );
		ensemble->insertComponent( createEnsembleComponent( widgetIdentifier, coronal, ensemble ) );
	} else {
		for( uint8_t i = 0; i < numberWidgets; i++ ) {
			ensemble->insertComponent( createEnsembleComponent( widgetIdentifier, not_specified, ensemble ) );
		}
	}

	if( show ) {
		attachWidgetEnsemble( ensemble );
	}

	m_EnsembleList.push_back( ensemble );
	return ensemble;
}

WidgetEnsemble::Pointer UICore::createViewWidgetEnsemble ( const std::string &widgetType, ImageHolder::Vector imageList, bool show )
{
	WidgetEnsemble::Pointer ensemble = createViewWidgetEnsemble( widgetType, show );

	if( show ) {
		BOOST_FOREACH( ImageHolder::Vector::const_reference image, imageList ) {
			ensemble->addImage( image );
		}
	}

	return ensemble;
}

WidgetEnsemble::Vector UICore::createViewWidgetEnsembleList( const std::string &widgetType, ImageHolder::Vector imageList, bool show )
{
	WidgetEnsemble::Vector retWidgetEnsembleList;
	BOOST_FOREACH( ImageHolder::Vector::const_reference image, imageList ) {
		WidgetEnsemble::Pointer ensemble = createViewWidgetEnsemble( widgetType, show );
		ensemble->addImage( image );
		retWidgetEnsembleList.push_back( ensemble );
	}
	return retWidgetEnsembleList;
}


ImageHolder::Vector UICore::closeWidgetEnsemble( WidgetEnsemble::Pointer ensemble )
{
	const WidgetEnsemble::Vector::iterator iter = std::find( m_EnsembleList.begin(), m_EnsembleList.end(), ensemble );

	if( iter != m_EnsembleList.end() ) {
		ImageHolder::Vector retList = ensemble->getImageVector();
		ensemble->close();
		m_EnsembleList.erase( iter );
		return retList;
	} else {
		LOG( Dev, error ) << "Tried to remove a widget ensemble that is not in the ensemble list!";
		return ImageHolder::Vector();
	}

	return ImageHolder::Vector();
}

void UICore::closeAllWidgetEnsembles()
{
	WidgetEnsemble::Vector cp = m_EnsembleList; //make a copy of the list to iterate on
	BOOST_FOREACH( WidgetEnsemble::Vector::reference ensemble, m_EnsembleList ) {
		closeWidgetEnsemble( ensemble );
	}
	LOG_IF( !m_EnsembleList.empty(), Dev, error ) << "Removed all widget ensembles, but ensemble list still contains "
			<< m_EnsembleList.size() << " elements!";
}

void UICore::attachWidgetEnsemble( WidgetEnsemble::Pointer ensemble )
{
	m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble->getFrame() );
	ensemble->connectToViewer();
}

WidgetEnsembleComponent::Pointer UICore::createEnsembleComponent( const std::string &widgetIdentifier, PlaneOrientation planeOrientation, WidgetEnsemble::Pointer ensemble )
{
	QFrame *frameWidget = new QFrame();
	QWidget *placeHolder = new QWidget( frameWidget );
	QDockWidget *dockWidget = createDockingEnsemble( frameWidget );
	dockWidget->setMinimumHeight( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "maxWidgetHeight" ) );
	dockWidget->setMinimumWidth( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "maxWidgetWidth" ) );
	dockWidget->setWidget( frameWidget );
	frameWidget->setParent( dockWidget );
	frameWidget->setLayout( new QGridLayout() );
	frameWidget->layout()->addWidget( placeHolder );
	frameWidget->layout()->setMargin( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "viewerWidgetMargin" ) );

	widget::WidgetInterface *widgetImpl = m_ViewerCore->getWidget( widgetIdentifier );
	widgetImpl->setWidgetEnsemble( ensemble );
	widgetImpl->setup( m_ViewerCore, placeHolder, planeOrientation );

	WidgetEnsembleComponent::Pointer component( new WidgetEnsembleComponent( frameWidget, dockWidget, placeHolder, widgetImpl ) );
	registerEnsembleComponent( component );
	return component;

}

void UICore::reloadPluginsToGUI()
{
	m_MainWindow->reloadPluginsToGUI();
}

void UICore::refreshUI( const bool &mainwindow )
{
	WidgetEnsemble::Vector cp = getEnsembleList();
	BOOST_FOREACH( WidgetEnsemble::Vector::reference ensemble, cp ) {
		if( ensemble->getImageVector().empty() ) {
			closeWidgetEnsemble( ensemble );
		} else {
			ensemble->update( m_ViewerCore );
		}
	}
	//refresh peripherals
	m_SliderWidget->synchronize();
	m_ImageStackWidget->synchronize();
	m_VoxelInformationWidget->synchronize();

	if( mainwindow ) {
		m_MainWindow->refreshUI();
	}
}

WidgetEnsemble::Pointer UICore::getCurrentEnsemble() const
{
	if( getEnsembleList().size() ) {
		BOOST_FOREACH( WidgetEnsemble::Vector::const_reference ensemble, getEnsembleList() ) {
			if( ensemble->isCurrent() ) {
				return ensemble;
			}
		}
		LOG( Dev, warning ) << "There is no ensemble that thinks it is the current one!";
		return getEnsembleList().front();
	} else {
		LOG( Dev, error ) << "Viewer has no ensemble yet. So can not pick the current one!";
	}

	return WidgetEnsemble::Pointer();
}


bool UICore::registerEnsembleComponent( WidgetEnsembleComponent::Pointer component )
{
	if( m_WidgetMap.find( component->getWidgetInterface() ) != m_WidgetMap.end() ) {
		LOG( Runtime, warning ) << "Widget with id" << component->getWidgetInterface()->getWidgetIdent() << "!";
		return false;
	}

	m_WidgetMap.insert( std::make_pair< widget::WidgetInterface *, WidgetEnsembleComponent::Pointer >( component->getWidgetInterface(), component ) );
	return true;

}

void UICore::showInformationAreas( bool show )
{
	m_VoxelInformationWidget->setVisible( show );
	m_ImageStackWidget->setVisible( show );
	m_SliderWidget->setVisible( show );
}

QImage UICore::getScreenshot()
{
	if( m_ViewerCore->hasImage() ) {
		WidgetEnsemble::Vector ensembleList = getEnsembleList();
		//preparation
		size_t biggestWidth = 0;
		BOOST_FOREACH( WidgetEnsemble::Vector::reference ensemble, ensembleList ) {
			if( ensemble->size() > biggestWidth ) {
				biggestWidth = ensemble->size();
			}

			BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, *ensemble ) {
				ensembleComponent->getFrame()->setFrameStyle( 0 );
				ensembleComponent->getFrame()->setAutoFillBackground( false );
				ensembleComponent->getWidgetInterface()->setCrossHairWidth( 2 );
			}
		}
		const int widgetHeight = ensembleList.front()->front()->getPlaceHolder()->height();
		const int widgetWidth = ensembleList.front()->front()->getPlaceHolder()->width();
		QPixmap screenshot( biggestWidth * widgetWidth, ensembleList.size() * widgetHeight + ( m_ViewerCore->getMode() == ViewerCoreBase::statistical_mode ? 100 : 0 ) ) ;
		screenshot.fill( Qt::black );
		QPainter painter( &screenshot );
		unsigned short eIndex = 0;
		BOOST_FOREACH( WidgetEnsemble::Vector::reference ensemble, ensembleList ) {
			unsigned short index = 0;
			BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, *ensemble ) {
				if( ensembleComponent->isNeeded() ) {
					QWidget *placeHolder = ensembleComponent->getPlaceHolder();
					painter.drawPixmap( index++ * placeHolder->width(), eIndex * placeHolder->height(), QPixmap::grabWidget( placeHolder ) );
				}
			}

			eIndex++;

		}
		//paint color table
		QFont font;
		font.setBold( true );
		font.setPointSize( 15 );
		painter.setFont( font );
		painter.setPen( QPen( Qt::white ) );
		const int offset = -7;

		if( m_ViewerCore->getMode() == ViewerCoreBase::statistical_mode ) {
			if( m_ViewerCore->getCurrentImage()->getImageProperties().minMax.first->as<double>() < 0 ) {
				const double lT = roundNumber<double>( m_ViewerCore->getCurrentImage()->getImageProperties().lowerThreshold, 4 );
				const double min = roundNumber<double>( m_ViewerCore->getCurrentImage()->getImageProperties().minMax.first->as<double>(), 4 );
				painter.drawPixmap( 100, widgetHeight * eIndex + 50, util::Singletons::get<color::Color, 10>().getIcon( m_ViewerCore->getCurrentImage()->getImageProperties().lut, 150, 15, color::Color::lower_half, true ).pixmap( 150, 15 ) );
				painter.drawText( 20 + ( lT < 0 ? offset : 0 ), widgetHeight * eIndex + 65, QString::number( lT  ) );
				painter.drawText( 280 + ( min < 0 ? offset : 0 ), widgetHeight * eIndex + 65, QString::number( roundNumber<double>( m_ViewerCore->getCurrentImage()->getImageProperties().minMax.first->as<double>(), 4 )  ) );
			}

			if ( m_ViewerCore->getCurrentImage()->getImageProperties().minMax.second->as<double>() > 0  ) {
				painter.drawPixmap( 100, widgetHeight * eIndex + 20, util::Singletons::get<color::Color, 10>().getIcon( m_ViewerCore->getCurrentImage()->getImageProperties().lut, 150, 15, color::Color::upper_half ).pixmap( 150, 15 ) );
				painter.drawText( 20, widgetHeight * eIndex + 35, QString::number( roundNumber<double>( m_ViewerCore->getCurrentImage()->getImageProperties().upperThreshold, 4 )  ) );
				painter.drawText( 280, widgetHeight * eIndex + 35, QString::number( roundNumber<double>( m_ViewerCore->getCurrentImage()->getImageProperties().minMax.second->as<double>(), 4 )  ) );
			}
		}

		painter.end();
		refreshUI();
		QImage screenshotImage ( m_ViewerCore->getSettings()->getPropertyAs<bool>( "screenshotManualScaling" ) ? screenshot.scaled( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotWidth" ),
								 m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotHeight" ),
								 m_ViewerCore->getSettings()->getPropertyAs<bool>( "screenshotKeepAspectRatio" ) ? Qt::KeepAspectRatioByExpanding : Qt::IgnoreAspectRatio,
								 Qt::SmoothTransformation
																																  ).toImage() : screenshot.toImage() );
		const double dpiMeter = 39.3700787;
		screenshotImage.setDotsPerMeterX( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotDPIX" ) * dpiMeter );
		screenshotImage.setDotsPerMeterY( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "screenshotDPIY" ) * dpiMeter );
		return screenshotImage;
	}

	return QImage();
}

void UICore::setViewPlaneOrientation( PlaneOrientation orientation, bool visible )
{
	BOOST_FOREACH( WidgetEnsemble::Vector::reference ensemble, getEnsembleList() ) {
		BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, *ensemble ) {
			if( ensembleComponent->getWidgetInterface()->getPlaneOrientation() == orientation ) {
				ensembleComponent->getDockWidget()->setVisible( visible );
			}
		}
	}
}

WidgetEnsemble::Pointer UICore::getEnsembleFromImage ( const ImageHolder::Pointer image ) const
{
	WidgetEnsemble::Pointer retEnsemble;
	bool found = false;
	BOOST_FOREACH( WidgetEnsemble::Vector::const_reference ensemble, m_EnsembleList ) {
		if( std::find( ensemble->getImageVector().begin(), ensemble->getImageVector().end(), image ) != ensemble->getImageVector().end() ) {
			retEnsemble = ensemble;
			found = true;
		}
	}
	LOG_IF( !found, Dev, error ) << "Trying to find the ensemble that contains the image \"" << image->getImageProperties().fileName
								 << "\", but there seems to be no such ensemble!";
	return retEnsemble;
}

void UICore::refreshEnsembles()
{
	unsigned short row = 0;
	BOOST_FOREACH( WidgetEnsemble::Vector::const_reference ensemble, m_EnsembleList ) {
		getMainWindow()->getInterface().centralGridLayout->addWidget( ensemble->getFrame(), row++, 0 );
	}
}

void UICore::toggleLoadingIcon ( bool start, const QString &text )
{
	if( text.length() ) {
		m_ViewerCore->receiveMessage( text.toStdString() );
	}

	getMainWindow()->m_StatusMovieLabel->setVisible( start );
	getMainWindow()->m_Interface.statusbar->setVisible( start );
	getMainWindow()->m_StatusTextLabel->setVisible( text.length() );

	if( start ) {
		getMainWindow()->m_StatusMovie->start();
		getMainWindow()->m_StatusTextLabel->setText( text );
	} else {
		getMainWindow()->m_StatusMovie->stop();
		m_ViewerCore->getProgressFeedback()->close();
	}

	QApplication::processEvents();
}

void UICore::openFromDropEvent ( QDropEvent */*e*/ )
{
	//  const QMimeData* mimeData = e->mimeData();
	//  if (mimeData->hasUrls()) {
	//      QStringList pathList;
	//         QList<QUrl> urlList = mimeData->urls();
	//
	//         // extract the local paths of the files
	//         for (int i = 0; i < urlList.size(); i++ )
	//         {
	//             std::cout << urlList.at(i).toLocalFile().toStdString() << std::endl;
	//         }
	//  }
}


}
}