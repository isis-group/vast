#include "uicore.hpp"
#include <DataStorage/io_interface.h>
#include "QImageWidgetImplementation.hpp"
#include <QSignalMapper>

namespace isis
{
namespace viewer
{


UICore::UICore( QViewerCore *core )
	: m_ViewerCore( core ),
	  m_MainWindow( new MainWindow( core ) )
{
	m_VoxelInformationWidget = new widget::VoxelInformationWidget( m_MainWindow, core );
	m_SliderWidget = new widget::SliderWidget( m_MainWindow, core );
	m_ImageStackWidget = new widget::ImageStackWidget( m_MainWindow, core );
	m_ViewWidgetArrangement = InRow;
	m_RowCount = m_MainWindow->getInterface().centralGridLayout->rowCount();
	m_VoxelInformationWidget->setVisible( false );
	m_ImageStackWidget->setVisible( false );
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

void UICore::showMessage( const qt4::QMessage &message  )
{
	QPalette pal;
	std::stringstream logStream;
	logStream << message.m_module << "(" << message.time_str << ") -> " << message.message ;
	m_MainWindow->getInterface().statusbar->setFont( QFont( "", 12 ) );
	const uint16_t logTime = m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>( "logDelayTime" );

	switch( message.m_level ) {
	case verbose_info:

		if( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showVerboseInfoMessages" ) ) {
			pal.setColor( QPalette::Foreground, Qt::black );
			m_MainWindow->getInterface().statusbar->setPalette( pal );
			m_MainWindow->getInterface().statusbar->showMessage( logStream.str().c_str(), logTime );
		}

		break;
	case info:

		if( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showInfoMessages" ) ) {
			pal.setColor( QPalette::Foreground, Qt::black );
			m_MainWindow->getInterface().statusbar->setPalette( pal );
			m_MainWindow->getInterface().statusbar->showMessage( logStream.str().c_str(), logTime );
		}

		break;
	case warning:

		if( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showWarningMessages" ) ) {
			pal.setColor( QPalette::Foreground, QColor( 184, 134, 11 )  );
			m_MainWindow->getInterface().statusbar->setPalette( pal );
			m_MainWindow->getInterface().statusbar->showMessage( logStream.str().c_str(), logTime );
		}

		break;
	case error:

		if( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showErrorMessages" ) ) {
			pal.setColor( QPalette::Foreground, Qt::red  );
			m_MainWindow->getInterface().statusbar->setPalette( pal );
			m_MainWindow->getInterface().statusbar->showMessage( logStream.str().c_str(), logTime );
		}

		break;
	case notice:

		if( m_ViewerCore->getOptionMap()->getPropertyAs<bool>( "showNoticeMessages" ) ) {
			pal.setColor( QPalette::Foreground, Qt::green  );
			m_MainWindow->getInterface().statusbar->setPalette( pal );
			m_MainWindow->getInterface().statusbar->showMessage( logStream.str().c_str(), logTime );
		}

		break;

	}

	QCoreApplication::processEvents();
}

void UICore::showMainWindow()
{
	m_MainWindow->getInterface().rightGridLayout->addWidget( m_SliderWidget );
	m_MainWindow->show();

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

UICore::ViewWidgetEnsembleType UICore::createViewWidgetEnsemble( const std::string &widgetType, boost::shared_ptr< ImageHolder > image, bool show )
{
	ViewWidgetEnsembleType ensemble = createViewWidgetEnsemble( widgetType, show );
	ensemble[0].widgetImplementation->addImage( image );
	ensemble[1].widgetImplementation->addImage( image );
	ensemble[2].widgetImplementation->addImage( image );
	return ensemble;
}


UICore::ViewWidgetEnsembleType UICore::createViewWidgetEnsemble( const std::string &widgetType, bool show )
{
	ViewWidgetEnsembleType ensemble;
	ensemble[0] =  createViewWidget( widgetType, axial );
	ensemble[1] =  createViewWidget( widgetType, sagittal );
	ensemble[2] =  createViewWidget( widgetType, coronal );

	if( show ) {
		attachViewWidgetEnsemble( ensemble );
	}

	m_EnsembleList.push_back( ensemble );
	return ensemble;
}

void UICore::removeViewWidgetEnsemble( WidgetInterface *widgetImplementation )
{
	BOOST_FOREACH( ViewWidgetEnsembleListType::reference ref, m_EnsembleList ) {
		if( ref[0].widgetImplementation == widgetImplementation
			|| ref[1].widgetImplementation == widgetImplementation
			|| ref[2].widgetImplementation == widgetImplementation ) {
			removeViewWidgetEnsemble( ref );
		}
	}
}

void UICore::removeViewWidgetEnsemble( UICore::ViewWidgetEnsembleType ensemble )
{
	for( unsigned short i = 0; i < 3; i++ ) {
		m_MainWindow->getInterface().centralGridLayout->removeWidget( ensemble[i].dockWidget );
	}

	m_EnsembleList.erase( std::find( m_EnsembleList.begin(), m_EnsembleList.end(), ensemble ) );
}

UICore::ViewWidgetEnsembleType UICore::detachViewWidgetEnsemble( WidgetInterface *widgetImplementation )
{
	BOOST_FOREACH( ViewWidgetEnsembleListType::reference ref, m_EnsembleList ) {
		if( ref[0].widgetImplementation == widgetImplementation
			|| ref[1].widgetImplementation == widgetImplementation
			|| ref[2].widgetImplementation == widgetImplementation ) {
			return detachViewWidgetEnsemble( ref );
		}
	}
	return ViewWidgetEnsembleType();
}

UICore::ViewWidgetEnsembleType  UICore::detachViewWidgetEnsemble( UICore::ViewWidgetEnsembleType ensemble )
{
	for( unsigned short i = 0; i < 3; i++ ) {
		m_MainWindow->getInterface().centralGridLayout->removeWidget( ensemble[i].dockWidget );
	}

	return ensemble;
}

void UICore::attachViewWidgetEnsemble( UICore::ViewWidgetEnsembleType ensemble )
{
	switch ( m_ViewWidgetArrangement ) {
	case Default: {
		if( m_EnsembleList.size() > 0 ) {
			m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble[0].dockWidget, m_RowCount, 0 );
			m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble[1].dockWidget, m_RowCount, 1 );
			m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble[2].dockWidget, m_RowCount, 2 );
		} else {
			m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble[0].dockWidget, 0, 0 );
			m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble[1].dockWidget, 0, 1 );
			m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble[2].dockWidget, 1, 0 );
		}

		break;
	}
	case InRow: {
		m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble[0].dockWidget, m_RowCount, 0 );
		m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble[1].dockWidget, m_RowCount, 1 );
		m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble[2].dockWidget, m_RowCount, 2 );
		break;
	}
	case InColumn: {
		int currentColumn = m_MainWindow->getInterface().centralGridLayout->columnCount() ;
		m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble[0].dockWidget, 0, currentColumn );
		m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble[1].dockWidget, 1, currentColumn );
		m_MainWindow->getInterface().centralGridLayout->addWidget( ensemble[2].dockWidget, 2, currentColumn );
	}
	}

	m_RowCount++;
}

void UICore::rearrangeViewWidgets()
{
	m_RowCount = 0;
	BOOST_FOREACH( ViewWidgetEnsembleListType::const_reference ensemble, m_EnsembleList ) {
		attachViewWidgetEnsemble( detachViewWidgetEnsemble( ensemble ) );
	}
	setOptionPosition( bottom );
}

UICore::ViewWidget UICore::createViewWidget( const std::string &widgetType, PlaneOrientation planeOrientation )
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


	WidgetInterface *widgetImpl = new QImageWidgetImplementation( m_ViewerCore, placeHolder, planeOrientation );

	ViewWidget viewWidget;
	viewWidget.placeHolder = placeHolder;
	viewWidget.dockWidget = dockWidget;
	viewWidget.frame = frameWidget;
	viewWidget.widgetImplementation = widgetImpl;
	viewWidget.planeOrientation = planeOrientation;
	viewWidget.widgetType = widgetType;
	registerWidget( viewWidget );
	return viewWidget;

}

void UICore::reloadPluginsToGUI()
{
	m_MainWindow->reloadPluginsToGUI();
}

void UICore::refreshUI( )
{
	m_SliderWidget->synchronize();
	m_ImageStackWidget->synchronize();
	m_VoxelInformationWidget->synchronize();
	BOOST_FOREACH( WidgetMap::reference widget, getWidgets() ) {
		WidgetInterface::ImageVectorType iVector = widget.second.widgetImplementation->getImageVector();

		if( !iVector.size() ) {
			widget.second.dockWidget->setVisible( false );
		} else {
			widget.second.dockWidget->setVisible( true );
		}

		widget.second.widgetImplementation->setCrossHairWidth( 1 );

		if( std::find( iVector.begin(), iVector.end(), m_ViewerCore->getCurrentImage() ) != iVector.end() ) {
			QPalette pal;
			pal.setColor( QPalette::Background, QColor( 119, 136, 153 ) );
			widget.second.frame->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
			widget.second.frame->setLineWidth( 1 );
			widget.second.frame->setPalette( pal );
			widget.second.frame->setAutoFillBackground( true );

			if( m_ViewerCore->getMode() == ViewerCoreBase::zmap ) {
				widget.second.widgetImplementation->setCrossHairColor( Qt::white );
				widget.second.widgetImplementation->updateScene();
			}
		} else {
			widget.second.frame->setFrameStyle( 0 );
			widget.second.frame->setAutoFillBackground( false );

			if ( m_ViewerCore->getMode() == ViewerCoreBase::zmap ) {
				widget.second.widgetImplementation->setCrossHairColor( QColor( 255, 102, 0 ) );
				widget.second.widgetImplementation->updateScene();
			}
		}

		if( m_ViewerCore->getMode() != ViewerCoreBase::zmap ) {
			widget.second.widgetImplementation->setCrossHairColor( QColor( 255, 102, 0 ) );
		}
	}
	m_MainWindow->refreshUI();
	m_VoxelInformationWidget->setVisible( m_ViewerCore->hasImage() );
	m_ImageStackWidget->setVisible( m_ViewerCore->hasImage() );
	m_SliderWidget->setVisible( m_ViewerCore->hasImage() );

}


bool UICore::registerWidget( ViewWidget widget )
{
	if( m_WidgetMap.find( widget.widgetImplementation ) != m_WidgetMap.end() ) {
		LOG( Runtime, warning ) << "Widget with id" << widget.widgetImplementation->getWidgetName() << "!";
		return false;
	}

	m_WidgetMap[widget.widgetImplementation] = widget;
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
		ViewWidgetEnsembleListType ensembleList = getEnsembleList();
		//preparation
		BOOST_FOREACH( ViewWidgetEnsembleListType::reference ensemble, ensembleList ) {
			for( unsigned short i = 0; i < 3; i++ ) {
				ensemble[i].frame->setFrameStyle( 0 );
				ensemble[i].frame->setAutoFillBackground( false );
				ensemble[i].widgetImplementation->setCrossHairWidth( 2 );
			}
		}
		const int widgetHeight = ensembleList.front()[0].placeHolder->height();
		const int widgetWidth = ensembleList.front()[0].placeHolder->width();
		QPixmap screenshot( 3 * widgetWidth, ensembleList.size() * widgetHeight + ( m_ViewerCore->getMode() == ViewerCoreBase::zmap ? 100 : 0 ) ) ;
		screenshot.fill( Qt::black );
		QPainter painter( &screenshot );
		unsigned short eIndex = 0;
		BOOST_FOREACH( ViewWidgetEnsembleListType::reference ensemble, ensembleList ) {
			for ( unsigned short i = 0; i < 3; i++ ) {
				QWidget *placeHolder = ensemble[i].placeHolder;
				ensemble[i].widgetImplementation->setCrossHairColor( Qt::white );
				ensemble[i].widgetImplementation->updateScene();
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
		if( m_ViewerCore->getMode() == ViewerCoreBase::zmap ) {
			if( m_ViewerCore->getCurrentImage()->minMax.first->as<double>() < 0 ) {
				const double lT = roundNumber<double>( m_ViewerCore->getCurrentImage()->lowerThreshold, 4 );
				const double min = roundNumber<double>( m_ViewerCore->getCurrentImage()->minMax.first->as<double>(), 4 );
				painter.drawPixmap( 100, widgetHeight * eIndex + 50, util::Singletons::get<color::Color, 10>().getIcon( m_ViewerCore->getCurrentImage()->lut, 150, 15, color::Color::lower_half ).pixmap( 150, 15 ) );
				painter.drawText( 20 + ( lT < 0 ? offset : 0 ), widgetHeight * eIndex + 65, QString::number( lT  ) );
				painter.drawText( 280 + ( min < 0 ? offset : 0 ), widgetHeight * eIndex + 65, QString::number( roundNumber<double>( m_ViewerCore->getCurrentImage()->minMax.first->as<double>(), 4 )  ) );
			}

			if ( m_ViewerCore->getCurrentImage()->minMax.second->as<double>() > 0  ) {
				painter.drawPixmap( 100, widgetHeight * eIndex + 20, util::Singletons::get<color::Color, 10>().getIcon( m_ViewerCore->getCurrentImage()->lut, 150, 15, color::Color::upper_half ).pixmap( 150, 15 ) );
				painter.drawText( 20, widgetHeight * eIndex + 35, QString::number( roundNumber<double>( m_ViewerCore->getCurrentImage()->upperThreshold, 4 )  ) );
				painter.drawText( 280, widgetHeight * eIndex + 35, QString::number( roundNumber<double>( m_ViewerCore->getCurrentImage()->minMax.second->as<double>(), 4 )  ) );
			}
		}
		painter.end();
		refreshUI();
		QImage screenshotImage ( screenshot.scaled( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("screenshotWidth"),
															m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("screenshotHeight"), 
															m_ViewerCore->getOptionMap()->getPropertyAs<bool>("screenshotKeepAspectRatio") ? Qt::KeepAspectRatioByExpanding : Qt::IgnoreAspectRatio,
															Qt::SmoothTransformation  														
  														).toImage() );
		const double dpiMeter = 39.3700787;
		screenshotImage.setDotsPerMeterX( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("screenshotDPIX") * dpiMeter );
		screenshotImage.setDotsPerMeterY( m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("screenshotDPIY") * dpiMeter );
		return screenshotImage;
	}
	return QImage();
}


}
}