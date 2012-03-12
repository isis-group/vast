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

void UICore::showMainWindow()
{
	m_MainWindow->getInterface().rightGridLayout->addWidget( m_SliderWidget );
	m_MainWindow->show();

}


void UICore::attachImageToEnsemble ( boost::shared_ptr< ImageHolder > image, WidgetEnsemble ensemble )
{
	BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, ensemble ) {
		attachImageToWidget( image, ensembleComponent.getWidgetInterface() );
	}
}

void UICore::attachImageListToEnsemble ( ImageHolder::List imageList, WidgetEnsemble ensemble )
{	
	BOOST_FOREACH( ImageHolder::List::const_reference image, imageList ) {
		BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, ensemble ) {
			attachImageToWidget( image, ensembleComponent.getWidgetInterface() );
		}
	}	
}

bool UICore::attachImageToWidget ( ImageHolder::Pointer image, widget::WidgetInterface* widget )
{
	if ( getWidgets().find ( widget ) == getWidgets().end() )
	{
		LOG ( Runtime, error ) << "There is no such widget "
							   << widget << ", so will not add image " << image->getFileNames().front() << " to it.";
		return false;
	}
	widget->addImage ( image );
	return true;
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

WidgetEnsemble UICore::createViewWidgetEnsemble( const std::string &widgetIdentifier, ImageHolder::Pointer image, bool show )
{
	WidgetEnsemble ensemble = createViewWidgetEnsemble( widgetIdentifier, show );
	BOOST_FOREACH( WidgetEnsemble::reference widget, ensemble ) {
		widget.getWidgetInterface()->addImage( image );
	}
	return ensemble;
}


WidgetEnsemble UICore::createViewWidgetEnsemble( const std::string &widgetIdentifier, bool show )
{
	WidgetEnsemble ensemble;
	uint8_t numberWidgets;
	if( m_ViewerCore->getWidgetProperties(widgetIdentifier)->hasProperty("numberOfEntitiesInEnsemble") ) {
		numberWidgets = m_ViewerCore->getWidgetProperties(widgetIdentifier)->getPropertyAs<uint8_t>("numberOfEntitiesInEnsemble");
	} else {
		LOG( Dev, error ) << "Your widget \"" << widgetIdentifier << "\" has no property \"numberOfEntitiesInEnsemble\" ! Setting it to 1";
		numberWidgets = 1;
	}
	if( numberWidgets == 1 ) {
		ensemble.insertComponent( createEnsembleComponent( widgetIdentifier, not_specified) );
	} else if ( numberWidgets == 3 ) {
		ensemble.insertComponent( createEnsembleComponent( widgetIdentifier, axial ) );
		ensemble.insertComponent( createEnsembleComponent( widgetIdentifier, sagittal ) );
		ensemble.insertComponent( createEnsembleComponent( widgetIdentifier, coronal ) );
	} else {
		for( uint8_t i = 0; i < numberWidgets; i++ ) {
			ensemble.insertComponent( createEnsembleComponent( widgetIdentifier, not_specified ) );
		}
	}
	if( show ) {
		attachWidgetEnsemble( ensemble );
	}
	m_EnsembleList.push_back( ensemble );
	return ensemble;
}

WidgetEnsemble UICore::createViewWidgetEnsemble ( const std::string& widgetType, ImageHolder::List imageList, bool show )
{
	WidgetEnsemble ensemble = createViewWidgetEnsemble( widgetType, show );
	if( show ) {
		attachImageListToEnsemble( imageList, ensemble );
	}
	return ensemble;
}

WidgetEnsembleListType UICore::createViewWidgetEnsembleList(const std::string& widgetType, ImageHolder::List imageList, bool show)
{
	WidgetEnsembleListType retWidgetEnsembleList;
	BOOST_FOREACH( ImageHolder::List::const_reference image, imageList ) {
		WidgetEnsemble ensemble = createViewWidgetEnsemble( widgetType, show );
		attachImageToEnsemble( image, ensemble );
		retWidgetEnsembleList.push_back( ensemble );
	}
	return retWidgetEnsembleList;
}


void UICore::removeWidgetEnsemble( WidgetEnsemble ensemble )
{
	m_MainWindow->getInterface().centralGridLayout->removeWidget( ensemble.getFrame() );
}


void UICore::attachWidgetEnsemble( WidgetEnsemble ensemble )
{
	m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble.getFrame() );
	if( ensemble.front().getWidgetInterface()->hasOptionWidget() ) {
// 		m_MainWindow->getInterface().leftGridLayout->addWidget( ensemble.front().getWidgetInterface()->getOptionWidget(), 0, 0 );
	}
}

void UICore::removeAllWidgetEnsembles()
{
	BOOST_FOREACH( WidgetEnsembleListType::reference ensemble, m_EnsembleList ) {
		removeWidgetEnsemble( ensemble );
	}
	m_EnsembleList.clear();
}


WidgetEnsembleComponent UICore::createEnsembleComponent( const std::string &widgetIdentifier, PlaneOrientation planeOrientation )
{
	QFrame *frameWidget = new QFrame();
	QWidget *placeHolder = new QWidget( frameWidget );
	QDockWidget *dockWidget = createDockingEnsemble( frameWidget );
	dockWidget->setMinimumHeight( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "maxWidgetHeight" ) );
	dockWidget->setMinimumWidth( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "maxWidgetWidth" ) );
	dockWidget->setWidget( frameWidget );
	frameWidget->setParent( dockWidget );
	frameWidget->setLayout( new QGridLayout() );
	frameWidget->layout()->addWidget( placeHolder );
	frameWidget->layout()->setMargin( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "viewerWidgetMargin" ) );

	widget::WidgetInterface * widgetImpl = m_ViewerCore->getWidget(widgetIdentifier);
	widgetImpl->setup( m_ViewerCore, placeHolder, planeOrientation );

	WidgetEnsembleComponent component( frameWidget, dockWidget, placeHolder, widgetImpl );
	registerEnsembleComponent( component );
	return component;

}

void UICore::reloadPluginsToGUI()
{
	m_MainWindow->reloadPluginsToGUI();
}

void UICore::refreshUI( bool complete )
{
	if( complete ) {
		m_SliderWidget->synchronize();
		m_ImageStackWidget->synchronize();
		m_VoxelInformationWidget->synchronize();
	}
	BOOST_FOREACH( WidgetEnsembleListType::reference ensemble, getEnsembleList() ) {
		widget::WidgetInterface::ImageVectorType iVector = ensemble.front().getWidgetInterface()->getImageVector();

		if( !iVector.size() ) {
			ensemble.front().getDockWidget()->setVisible( false );
		} else {
			ensemble.front().getDockWidget()->setVisible( true );
		}

		//go through all the images and check if we need this widget ( 2d data? )
		bool widgetNeeded = false;
		BOOST_FOREACH( widget::WidgetInterface::ImageVectorType::const_reference image, iVector ) {
			const util::ivector4 mappedSize = mapCoordsToOrientation( image->getImageSize(), image->getImageProperties().latchedOrientation, ensemble.front().getWidgetInterface()->getPlaneOrientation() );

			if( mappedSize[0] > 1 && mappedSize[1] > 1 ) {
				widgetNeeded = true;
			}
		}
		if ( m_ViewerCore->hasImage() ) {
			BOOST_FOREACH( WidgetEnsemble::reference ensembleComponent, ensemble ) {
				ensembleComponent.getWidgetInterface()->setCrossHairWidth( 1 );
				ensembleComponent.getDockWidget()->setVisible( widgetNeeded );

				ensembleComponent.setHasCurrentImage( std::find( iVector.begin(), iVector.end(), m_ViewerCore->getCurrentImage() ) != iVector.end() );
				if( ensembleComponent.hasCurrentImage() ) {
					QPalette pal;
					pal.setColor( QPalette::Background, QColor( 119, 136, 153 ) );
					ensembleComponent.getFrame()->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
					ensembleComponent.getFrame()->setLineWidth( 1 );
					ensembleComponent.getFrame()->setPalette( pal );
					ensembleComponent.getFrame()->setAutoFillBackground( true );

					if( m_ViewerCore->getMode() == ViewerCoreBase::statistical_mode ) {
						ensembleComponent.getWidgetInterface()->setCrossHairColor( Qt::white );
						ensembleComponent.getWidgetInterface()->updateScene();
					}
				} else {
					ensembleComponent.getFrame()->setFrameStyle( 0 );
					ensembleComponent.getFrame()->setAutoFillBackground( false );

					if ( m_ViewerCore->getMode() == ViewerCoreBase::statistical_mode ) {
						ensembleComponent.getWidgetInterface()->setCrossHairColor( QColor( 255, 102, 0 ) );
						ensembleComponent.getWidgetInterface()->updateScene();
					}
				}
				if( m_ViewerCore->getMode() != ViewerCoreBase::statistical_mode ) {
					ensembleComponent.getWidgetInterface()->setCrossHairColor( QColor( 255, 102, 0 ) );
				}
			}
		}
	}
	if( complete ) {
		m_MainWindow->refreshUI();
		m_VoxelInformationWidget->setVisible( m_ViewerCore->hasImage() );
		m_ImageStackWidget->setVisible( m_ViewerCore->hasImage() );
		m_SliderWidget->setVisible( m_ViewerCore->hasImage() );
	}

}

WidgetEnsemble UICore::getCurrentEnsemble() const
{
	if( getEnsembleList().size() ) {
		BOOST_FOREACH( WidgetEnsembleListType::const_reference ensemble, getEnsembleList() ) {
			if( ensemble.front().hasCurrentImage() ) {
				return ensemble;
			}
		}
	} else {
		LOG( Dev, error ) << "Viewer has no ensemble yet. So can not pick the current one!";
	}
}


bool UICore::registerEnsembleComponent( WidgetEnsembleComponent component )
{
	if( m_WidgetMap.find( component.getWidgetInterface() ) != m_WidgetMap.end() ) {
		LOG( Runtime, warning ) << "Widget with id" << component.getWidgetInterface()->getWidgetName() << "!";
		return false;
	}

	m_WidgetMap.insert( std::make_pair< widget::WidgetInterface*, WidgetEnsembleComponent >( component.getWidgetInterface(), component ) );
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
		WidgetEnsembleListType ensembleList = getEnsembleList();
		//preparation
		BOOST_FOREACH( WidgetEnsembleListType::reference ensemble, ensembleList ) {
			for( unsigned short i = 0; i < 3; i++ ) {
				ensemble[i].getFrame()->setFrameStyle( 0 );
				ensemble[i].getFrame()->setAutoFillBackground( false );
				ensemble[i].getWidgetInterface()->setCrossHairWidth( 2 );
			}
		}
		const int widgetHeight = ensembleList.front()[0].getPlaceHolder()->height();
		const int widgetWidth = ensembleList.front()[0].getPlaceHolder()->width();
		QPixmap screenshot( 3 * widgetWidth, ensembleList.size() * widgetHeight + ( m_ViewerCore->getMode() == ViewerCoreBase::statistical_mode ? 100 : 0 ) ) ;
		screenshot.fill( Qt::black );
		QPainter painter( &screenshot );
		unsigned short eIndex = 0;
		BOOST_FOREACH( WidgetEnsembleListType::reference ensemble, ensembleList ) {
			for ( unsigned short i = 0; i < 3; i++ ) {
				QWidget *placeHolder = ensemble[i].getPlaceHolder();
				ensemble[i].getWidgetInterface()->setCrossHairColor( Qt::white );
				ensemble[i].getWidgetInterface()->updateScene();
				painter.drawPixmap( i * placeHolder->width(), eIndex * placeHolder->height(), QPixmap::grabWidget( placeHolder ) );
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
				painter.drawPixmap( 100, widgetHeight * eIndex + 50, util::Singletons::get<color::Color, 10>().getIcon( m_ViewerCore->getCurrentImage()->getImageProperties().lut, 150, 15, color::Color::lower_half ).pixmap( 150, 15 ) );
				painter.drawText( 20 + ( lT < 0 ? offset : 0 ), widgetHeight * eIndex + 65, QString::number( lT  ) );
				painter.drawText( 28.0 + ( min < 0 ? offset : 0 ), widgetHeight * eIndex + 65, QString::number( roundNumber<double>( m_ViewerCore->getCurrentImage()->getImageProperties().minMax.first->as<double>(), 4 )  ) );
			}

			if ( m_ViewerCore->getCurrentImage()->getImageProperties().minMax.second->as<double>() > 0  ) {
				painter.drawPixmap( 100, widgetHeight * eIndex + 20, util::Singletons::get<color::Color, 10>().getIcon( m_ViewerCore->getCurrentImage()->getImageProperties().lut, 150, 15, color::Color::upper_half ).pixmap( 150, 15 ) );
				painter.drawText( 20, widgetHeight * eIndex + 35, QString::number( roundNumber<double>( m_ViewerCore->getCurrentImage()->getImageProperties().upperThreshold, 4 )  ) );
				painter.drawText( 280, widgetHeight * eIndex + 35, QString::number( roundNumber<double>( m_ViewerCore->getCurrentImage()->getImageProperties().minMax.second->as<double>(), 4 )  ) );
			}
		}

		painter.end();
		refreshUI();
		QImage screenshotImage ( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "screenshotManualScaling" ) ? screenshot.scaled( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "screenshotWidth" ),
								 m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "screenshotHeight" ),
								 m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "screenshotKeepAspectRatio" ) ? Qt::KeepAspectRatioByExpanding : Qt::IgnoreAspectRatio,
								 Qt::SmoothTransformation
																																   ).toImage() : screenshot.toImage() );
		const double dpiMeter = 39.3700787;
		screenshotImage.setDotsPerMeterX( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "screenshotDPIX" ) * dpiMeter );
		screenshotImage.setDotsPerMeterY( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "screenshotDPIY" ) * dpiMeter );
		return screenshotImage;
	}

	return QImage();
}

void UICore::setViewPlaneOrientation( PlaneOrientation orientation, bool visible )
{
	BOOST_FOREACH( WidgetEnsembleListType::reference ensemble, getEnsembleList() ) {
		for( unsigned short i = 0; i < 3; i++ ) {
			if( ensemble[i].getWidgetInterface()->getPlaneOrientation() == orientation ) {
				ensemble[i].getDockWidget()->setVisible( visible );
			}
		}
	}
}



}
}