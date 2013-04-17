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
 * imageStackWidget.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "imageStackWidget.hpp"
#include "../viewer/viewercorebase.hpp"
#include "../viewer/qviewercore.hpp"
#include "../viewer/uicore.hpp"
#include "../viewer/color.hpp"

namespace isis
{
namespace viewer
{
namespace ui
{

ImageStack::ImageStack( QWidget *parent, ImageStackWidget *widget, QViewerCore *core )
	: QListWidget( parent ),
	  m_Widget( widget ),
	  m_ViewerCore( core )
{
	setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	setVerticalScrollMode( ScrollPerItem );
	setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	setSortingEnabled( false );
	typedef widget::WidgetLoader::WidgetMapType::const_reference WRef;
	QSignalMapper *signalMapper = new QSignalMapper( this );
	const widget::WidgetLoader::WidgetMapType &widgetMap = util::Singletons::get<widget::WidgetLoader, 10>().getWidgetMap();
	const widget::WidgetLoader::WidgetPropertyMapType &optionsMap = util::Singletons::get<widget::WidgetLoader, 10>().getWidgetPropertyMap();
	BOOST_FOREACH( WRef w, widgetMap ) {
		QAction *action = new QAction( this );
		action->setText( optionsMap.at( w.first )->getPropertyAs<std::string>( "widgetName" ).c_str() );
		m_WidgetActions.push_back( action );
		signalMapper->setMapping( action, w.first.c_str() );
		connect( action, SIGNAL( triggered( bool ) ), signalMapper, SLOT( map() ) );
	}
	connect( signalMapper, SIGNAL( mapped( QString ) ), m_Widget, SLOT( openImageInWidget( QString ) ) );
	setAcceptDrops( true );
}

void ImageStackWidget::openImageInWidget ( QString widget )
{
	const std::string imageName = m_ImageStack->currentItem()->data( Qt::UserRole ).toString().toStdString();

	if( m_ViewerCore->getImageMap().find( imageName ) != m_ViewerCore->getImageMap().end() ) {
		const ImageHolder::Pointer image = m_ViewerCore->getImageMap().at( imageName );
		std::stringstream ss;
		ss << "Show \"" << image->getImageProperties().fileName << "\" in new widget...";
		QMessageBox msgBox;
		msgBox.setText( ss.str().c_str() );
		msgBox.setInformativeText( "Do you want to copy this image?" );
		msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
		msgBox.setIcon( QMessageBox::Question );
		const int ret = msgBox.exec();

		switch( ret ) {
		case QMessageBox::Yes:
			//TODO actually we do not have to do a copy of the image
			m_ViewerCore->getUICore()->createViewWidgetEnsemble( widget.toStdString(), m_ViewerCore->addImage( *image->getISISImage().get(), image->getImageProperties().imageType ) );
			break;
		case QMessageBox::No:
			const WidgetEnsemble::Pointer ensemble = m_ViewerCore->getUICore()->getEnsembleFromImage( image );
			ensemble->removeImage( image );

			if( !ensemble->getImageVector().size() )  {
				m_ViewerCore->getUICore()->closeWidgetEnsemble( ensemble );
			}

			m_ViewerCore->getUICore()->createViewWidgetEnsemble( widget.toStdString(), image );
			break;
		}

		m_ViewerCore->getUICore()->refreshUI();
	}
}

void ImageStack::contextMenuEvent( QContextMenuEvent *event )
{
	QMenu menu( this );

	if(  currentItem() ) {
		const ImageHolder::Pointer image = m_ViewerCore->getImageMap().at( currentItem()->data( Qt::UserRole ).toString().toStdString() );
		QMenu *openInWidgetMenu = new QMenu( "Show in new widget...", this );
		BOOST_FOREACH( const std::list<QAction *>::const_reference action, m_WidgetActions ) {
			openInWidgetMenu->addAction( action );
		}
		menu.addAction( m_Widget->m_Interface.actionClose_image );


		if( !image->getImageProperties().zeroIsReserved && !image->getImageProperties().isRGB && image->getImageProperties().minMax.first->as<double>() < 0 ) {
			menu.addAction( m_Widget->m_Interface.actionSet_0_to_black );
		}

		menu.addMenu( openInWidgetMenu );
		menu.addSeparator();
	}
	if( m_ViewerCore->getMode() != ViewerCoreBase::statistical_mode ) {
		menu.addAction( m_Widget->m_Interface.actionDistribute_images );
	}
	menu.addAction( m_Widget->m_Interface.actionClose_all_images );
	menu.exec( event->globalPos() );

}

void ImageStack::mousePressEvent( QMouseEvent *e )
{
	if( e->button() == Qt::LeftButton && geometry().contains( e->pos() ) && QApplication::keyboardModifiers() == Qt::ControlModifier ) {
		QDrag *drag = new QDrag( this );
		QMimeData *mimeData = new QMimeData;
		mimeData->setText( itemAt( e->pos() )->text() );
		drag->setMimeData( mimeData );
		drag->setPixmap( QIcon( ":/common/vast.jpg" ).pixmap( 15 ) );
		drag->exec();
	}

	QListWidget::mousePressEvent( e );
}



//ImageStackWidget


ImageStackWidget::ImageStackWidget( QWidget *parent, QViewerCore *core )
	: QWidget( parent ), m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	m_Interface.actionClose_image->setIconVisibleInMenu( true );
	m_Interface.actionDistribute_images->setIconVisibleInMenu( true );
	m_Interface.actionClose_all_images->setIconVisibleInMenu( true );
	m_ImageStack = new ImageStack( this, this, core );
	m_Interface.stackLayout->addWidget( m_ImageStack );
	//  m_ImageStack->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

	m_ImageStack->setEditTriggers( QAbstractItemView::NoEditTriggers );
	connect( m_ImageStack, SIGNAL( itemActivated( QListWidgetItem * ) ), this, SLOT( itemSelected( QListWidgetItem * ) ) );
	connect( m_ImageStack, SIGNAL( itemChanged( QListWidgetItem * ) ), this, SLOT( itemChanged( QListWidgetItem * ) ) );
	connect( m_ImageStack, SIGNAL( itemPressed( QListWidgetItem * ) ), this, SLOT( itemClicked( QListWidgetItem * ) ) );
	connect( m_Interface.actionClose_image, SIGNAL( triggered() ), this, SLOT( closeImage() ) );
	connect( m_Interface.actionSet_0_to_black, SIGNAL( triggered() ), this, SLOT( setZeroToBlack() ) );
	connect( m_Interface.actionDistribute_images, SIGNAL( triggered() ), this, SLOT( distributeImages() ) );
	connect( m_Interface.actionClose_all_images, SIGNAL( triggered() ), this, SLOT( closeAllImages() ) );
	connect( m_Interface.checkViewAllImages, SIGNAL( clicked( bool ) ), this, SLOT( viewAllImagesClicked() ) );
	connect( m_Interface.moveDown, SIGNAL( clicked( bool ) ), this, SLOT( moveDown() ) );
	connect( m_Interface.moveUp, SIGNAL( clicked( bool ) ), this, SLOT( moveUp() ) );

}

void ImageStackWidget::viewAllImagesClicked()
{
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "viewAllImagesInStack", m_Interface.checkViewAllImages->isChecked() );
	synchronize();
}

void ImageStackWidget::setZeroToBlack()
{
	if(  m_ImageStack->currentItem() ) {
		boost::shared_ptr<ImageHolder> image = m_ViewerCore->getImageMap().at( m_ImageStack->currentItem()->data( Qt::UserRole ).toString().toStdString() );
		operation::NativeImageOps::setTrueZero( image );
		m_ViewerCore->updateScene();
	}
}


void ImageStackWidget::synchronize()
{
	setVisible( m_ViewerCore->hasImage() );

	m_Interface.frame->setMaximumHeight( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "maxOptionWidgetHeight" ) );
	m_Interface.frame->setMinimumHeight( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "minOptionWidgetHeight" ) );

	disconnect( m_Interface.checkViewAllImages, SIGNAL( clicked( bool ) ), this, SLOT( viewAllImagesClicked() ) );
	m_Interface.checkViewAllImages->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "viewAllImagesInStack" ) );
	connect( m_Interface.checkViewAllImages, SIGNAL( clicked( bool ) ), this, SLOT( viewAllImagesClicked() ) );

	m_ImageStack->clear();
	ImageHolder::Vector imageList;

	if( m_ViewerCore->hasImage() ) {
		m_CurrentSelectedEnsemble = m_ViewerCore->getUICore()->getCurrentEnsemble();

		if( m_Interface.checkViewAllImages->isChecked() ) {
			imageList = m_ViewerCore->getImageVector() ;
		} else {
			imageList = m_CurrentSelectedEnsemble->getImageVector();
		}

		BOOST_FOREACH( ImageHolder::Vector::const_reference image, imageList ) {
			if( !( m_ViewerCore->getMode() == ViewerCoreBase::statistical_mode && image->getImageProperties().imageType == ImageHolder::structural_image ) ) {
				QListWidgetItem *item = new QListWidgetItem;
				QString sD = image->getPropMap().getPropertyAs<std::string>( "sequenceDescription" ).c_str();

				if( m_ViewerCore->getSettings()->getPropertyAs<bool>( "showFullFilePath" ) ) {
					item->setText( QString( image->getImageProperties().filePath.c_str() ) );
				} else {
					item->setText( QString( image->getImageProperties().fileName.c_str() ) );
				}

				item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable );
				item->setData( Qt::UserRole, QVariant( image->getImageProperties().filePath.c_str() ) );

				if( image->getImageProperties().isVisible ) {
					item->setCheckState( Qt::Checked );
				} else {
					item->setCheckState( Qt::Unchecked );
				}

				if ( m_Interface.checkViewAllImages->isChecked() && m_ViewerCore->getUICore()->getEnsembleList().size() > 1 ) {
					const ImageHolder::Vector iList = m_CurrentSelectedEnsemble->getImageVector();

					if( std::find( iList.begin(), iList.end(), image ) != iList.end() ) {
						item->setBackgroundColor( color::currentEnsemble );
					}
				}

				if( m_ViewerCore->getCurrentImage().get() == image.get() ) {
					item->setIcon( QIcon( ":/common/currentImage.gif" ) );
					item->setTextColor( color::currentImage );
				}

				m_ImageStack->addItem( item );
			}
		}

		if( m_ViewerCore->getUICore()->getEnsembleList().size() > 1 ) {
			m_Interface.checkViewAllImages->setVisible( true );
			m_Interface.moveDown->setVisible( true );
			m_Interface.moveUp->setVisible( true );

			if ( m_ImageStack->currentItem() ) {
				m_Interface.moveUp->setEnabled( checkEnsembleCanUp( getEnsembleFromItem( m_ImageStack->currentItem() ) ) );
				m_Interface.moveDown->setEnabled( checkEnsembleCanDown( getEnsembleFromItem( m_ImageStack->currentItem() ) ) );
			} else {
				m_Interface.moveDown->setEnabled( checkEnsembleCanDown( m_CurrentSelectedEnsemble ) );
				m_Interface.moveUp->setEnabled( checkEnsembleCanUp( m_CurrentSelectedEnsemble ) );
			}
		} else {
			m_Interface.moveDown->setVisible( false );
			m_Interface.moveUp->setVisible( false );
			m_Interface.checkViewAllImages->setVisible( false );

		}
	}


}

void ImageStackWidget::itemClicked ( QListWidgetItem */*item*/ )
{
	if( m_ViewerCore->hasImage() ) {
		if( m_ImageStack->currentItem() ) {
			m_CurrentSelectedEnsemble = getEnsembleFromItem( m_ImageStack->currentItem() );
		} else {
			m_CurrentSelectedEnsemble = m_ViewerCore->getUICore()->getCurrentEnsemble();
		}

		m_Interface.moveDown->setEnabled( checkEnsembleCanDown( m_CurrentSelectedEnsemble ) );
		m_Interface.moveUp->setEnabled( checkEnsembleCanUp( m_CurrentSelectedEnsemble ) );
	}
}

void ImageStackWidget::itemChanged( QListWidgetItem *item )
{
	if( m_ViewerCore->hasImage() ) {
		const ImageHolder::Pointer image = m_ViewerCore->getImageMap().at( item->data( Qt::UserRole ).toString().toStdString() );

		if( item->checkState() == Qt::Checked ) {
			image->getImageProperties().isVisible = true ;
		} else {
			image->getImageProperties().isVisible = false ;
		}

		m_ViewerCore->getUICore()->refreshUI( false ); //no update of the mainwindow is needed here
		m_ViewerCore->updateScene();
	}

}

void ImageStackWidget::itemSelected( QListWidgetItem *item )
{
	m_ViewerCore->setCurrentImage( m_ViewerCore->getImageMap().at( item->data( Qt::UserRole ).toString().toStdString() ) );
}

void ImageStackWidget::closeAllImages()
{
	//ok we assume that "close all images" actually means to close all images - not only those that are listed by the imagestack
	ImageHolder::Vector cp = m_ViewerCore->getImageVector();
	BOOST_FOREACH( ImageHolder::Vector::const_reference image, cp ) {
		m_ViewerCore->closeImage( image, false ); //do not refresh the ui with each close
	}
	m_ViewerCore->getUICore()->refreshUI();
	LOG_IF( !m_ViewerCore->getUICore()->getEnsembleList().empty(), Dev, error ) << "Closed all images. But the amount of widget ensembles is not 0 ("
			<< m_ViewerCore->getUICore()->getEnsembleList().size() << ") !";
	LOG_IF( !m_ViewerCore->getImageVector().empty(), Dev, error ) << "Closed all images. But there are still "
			<< m_ViewerCore->getImageVector().size() << " in the global image list!";

}


void ImageStackWidget::closeImage()
{
	if(  m_ImageStack->currentItem() ) {
		m_ViewerCore->closeImage( m_ViewerCore->getImageMap().at( m_ImageStack->currentItem()->data( Qt::UserRole ).toString().toStdString() ) );
	}
}

void ImageStackWidget::distributeImages()
{
	const std::string widgetIdent = m_ViewerCore->getUICore()->getCurrentEnsemble()->front()->getWidgetInterface()->getWidgetIdent();
	m_ViewerCore->getUICore()->closeAllWidgetEnsembles();
	BOOST_FOREACH( ImageHolder::Vector::const_reference image, m_ViewerCore->getImageVector() ) {
		m_ViewerCore->getUICore()->createViewWidgetEnsemble( widgetIdent, image );
	}
	LOG_IF( m_ViewerCore->getImageVector().size() != m_ViewerCore->getUICore()->getEnsembleList().size(), Dev, error ) << "Distributed the images. But amount of images ("
			<< m_ViewerCore->getImageVector().size() << ") and amount of widget ensembles (" << m_ViewerCore->getUICore()->getEnsembleList().size()
			<< ") does not coincide!";
	m_ViewerCore->getUICore()->refreshUI( false );
	m_ViewerCore->updateScene();
	m_ViewerCore->settingsChanged();
}

void ImageStackWidget::moveDown()
{
	const WidgetEnsemble::Vector::iterator eIter = std::find( m_ViewerCore->getUICore()->getEnsembleList().begin(), m_ViewerCore->getUICore()->getEnsembleList().end(), m_CurrentSelectedEnsemble );

	if( eIter != m_ViewerCore->getUICore()->getEnsembleList().end() && ( eIter + 1 ) != m_ViewerCore->getUICore()->getEnsembleList().end() ) {
		std::iter_swap( eIter, eIter + 1 );
	}

	ImageHolder::Vector newImageList;
	BOOST_FOREACH( WidgetEnsemble::Vector::const_reference e, m_ViewerCore->getUICore()->getEnsembleList() ) {
		BOOST_FOREACH( ImageHolder::Vector::const_reference imageInE, e->getImageVector() ) {
			newImageList.push_back( imageInE );
		}
	}
	m_ViewerCore->getImageVector() = newImageList;
	m_ViewerCore->getUICore()->refreshEnsembles();
	m_ViewerCore->getUICore()->refreshUI();


}

void ImageStackWidget::moveUp()
{
	const WidgetEnsemble::Vector::iterator eIter = std::find( m_ViewerCore->getUICore()->getEnsembleList().begin(), m_ViewerCore->getUICore()->getEnsembleList().end(), m_CurrentSelectedEnsemble );

	if( eIter != m_ViewerCore->getUICore()->getEnsembleList().begin() ) {
		std::iter_swap( eIter, eIter - 1 );
	}

	ImageHolder::Vector newImageList;
	BOOST_FOREACH( WidgetEnsemble::Vector::const_reference e, m_ViewerCore->getUICore()->getEnsembleList() ) {
		BOOST_FOREACH( ImageHolder::Vector::const_reference imageInE, e->getImageVector() ) {
			newImageList.push_back( imageInE );
		}
	}
	m_ViewerCore->getImageVector() = newImageList;
	m_ViewerCore->getUICore()->refreshEnsembles();
	m_ViewerCore->getUICore()->refreshUI();
}

bool ImageStackWidget::checkEnsembleCanDown ( const WidgetEnsemble::Pointer ensemble )
{
	const WidgetEnsemble::Vector eL = m_ViewerCore->getUICore()->getEnsembleList();
	return std::find( eL.begin(), eL.end(), ensemble ) != eL.end() - 1;
}

bool ImageStackWidget::checkEnsembleCanUp ( const WidgetEnsemble::Pointer ensemble )
{
	const WidgetEnsemble::Vector eL = m_ViewerCore->getUICore()->getEnsembleList();
	return std::find( eL.begin(), eL.end(), ensemble ) != eL.begin();
}

const WidgetEnsemble::Pointer ImageStackWidget::getEnsembleFromItem ( const QListWidgetItem *item )
{
	const ImageHolder::Pointer image = m_ViewerCore->getImageMap().at( item->data( Qt::UserRole ).toString().toStdString() );
	return m_ViewerCore->getUICore()->getEnsembleFromImage( image );
}

void ImageStack::dropEvent ( QDropEvent *e )
{
	std::cout << "drop" << std::endl;
	m_ViewerCore->getUICore()->openFromDropEvent( e );
}


}
}
}
