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
 * Author: Erik Türke, tuerke@cbs.mpg.de
 *
 * imageStackWidget.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "imageStackWidget.hpp"
#include <viewercorebase.hpp>
#include <qviewercore.hpp>
#include <uicore.hpp>

namespace isis
{
namespace viewer
{
namespace ui
{

ImageStack::ImageStack( QWidget *parent, ImageStackWidget *widget )
	: QListWidget( parent ),
	  m_Widget( widget )
{
	setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	setVerticalScrollMode( ScrollPerItem );
	setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
}

void ImageStack::contextMenuEvent( QContextMenuEvent *event )
{
	
	QMenu menu( this );
	menu.addAction( m_Widget->m_Interface.actionClose_image );
	QMenu *imageTypeMenu = new QMenu( tr("Image type"), this );
	imageTypeMenu->addAction( m_Widget->m_Interface.actionStructural_image );
	imageTypeMenu->addAction( m_Widget->m_Interface.actionImage_type_stats );
	//TODO
// 	menu.addMenu( imageTypeMenu );
	menu.addSeparator();
	menu.addAction( m_Widget->m_Interface.actionDistribute_images );
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
	m_ImageStack = new ImageStack( this, this );
	m_Interface.stackLayout->addWidget( m_ImageStack );
// 	m_ImageStack->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
	
	m_ImageStack->setEditTriggers( QAbstractItemView::NoEditTriggers );
	connect( m_ImageStack, SIGNAL( itemActivated( QListWidgetItem * ) ), this, SLOT( itemSelected( QListWidgetItem * ) ) );
	connect( m_ImageStack, SIGNAL( itemChanged( QListWidgetItem * ) ), this, SLOT( itemChanged( QListWidgetItem * ) ) );
	connect( m_ImageStack, SIGNAL( itemPressed(QListWidgetItem*)), this, SLOT( itemClicked(QListWidgetItem*)));
	connect( m_Interface.actionClose_image, SIGNAL( triggered() ), this, SLOT( closeImage() ) );
	connect( m_Interface.actionDistribute_images, SIGNAL( triggered() ), this, SLOT( distributeImages() ) );
	connect( m_Interface.actionClose_all_images, SIGNAL( triggered() ), this, SLOT( closeAllImages() ) );
	connect( m_Interface.actionImage_type_stats, SIGNAL( triggered(bool)), this, SLOT(toggleStatsType()));
	connect( m_Interface.actionStructural_image, SIGNAL( triggered(bool)), this, SLOT( toggleStructsType()));
	connect( m_Interface.checkViewAllImages, SIGNAL( clicked(bool)), this, SLOT( synchronize()) );
	connect( m_Interface.moveDown, SIGNAL( clicked(bool)), this, SLOT( moveDown()) );
	connect( m_Interface.moveUp, SIGNAL( clicked(bool)), this, SLOT( moveUp()) );

}

void ImageStackWidget::toggleStatsType()
{
// 	m_Interface.actionStructural_image->setChecked( false );
// 	m_Interface.actionImage_type_stats->setChecked( true );
// 	boost::shared_ptr< ImageHolder> image = m_ViewerCore->getDataContainer().at( m_ImageStack->currentItem()->text().toStdString() );
// 	image->getImageProperties().imageType = ImageHolder::statistical_image;
// 	image->getImageProperties().lut = m_ViewerCore->getSettings()->getPropertyAs<std::string>("LutZMap");
// 	image->updateColorMap();
// 	m_ViewerCore->setMode( ViewerCoreBase::statistical_mode );
// 	m_ViewerCore->getUICore()->refreshUI();
// 	m_ViewerCore->updateScene();
}

void ImageStackWidget::toggleStructsType()
{
// 	m_Interface.actionStructural_image->setChecked( true );
// 	m_Interface.actionImage_type_stats->setChecked( false );
// 	boost::shared_ptr< ImageHolder> image = m_ViewerCore->getDataContainer().at( m_ImageStack->currentItem()->text().toStdString() );
// 	image->getImageProperties().imageType = ImageHolder::structural_image;
// 	image->getImageProperties().lut = m_ViewerCore->getSettings()->getPropertyAs<std::string>("lutStructural");
// 	image->updateColorMap();
// 	m_ViewerCore->setMode( ViewerCoreBase::default_mode );
// 	m_ViewerCore->getUICore()->refreshUI();
// 	m_ViewerCore->updateScene();
}



void ImageStackWidget::synchronize()
{
	m_Interface.frame->setMaximumHeight( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "maxOptionWidgetHeight" ) );
	m_Interface.frame->setMinimumHeight( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "minOptionWidgetHeight" ) );

	m_ImageStack->clear();
	ImageHolder::List imageList;
	if( m_ViewerCore->hasImage() ) {
		if( m_Interface.checkViewAllImages->isChecked() ) {
			imageList = m_ViewerCore->getImageList() ;
		} else {
			imageList = m_ViewerCore->getUICore()->getCurrentEnsemble()->getImageList();
		}
		BOOST_FOREACH( ImageHolder::List::const_reference image, imageList ) {
			if( !( m_ViewerCore->getMode() == ViewerCoreBase::statistical_mode && image->getImageProperties().imageType == ImageHolder::structural_image ) ) {
				QListWidgetItem *item = new QListWidgetItem;
				QString sD = image->getPropMap().getPropertyAs<std::string>( "sequenceDescription" ).c_str();
				item->setText( QString( image->getImageProperties().fileName.c_str() ) );
				item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable );
				item->setData(Qt::UserRole, QVariant( image->getImageProperties().id.c_str() ) );

				if( image->getImageProperties().isVisible ) {
					item->setCheckState( Qt::Checked );
				} else {
					item->setCheckState( Qt::Unchecked );
				}

				if( m_ViewerCore->getCurrentImage().get() == image.get() ) {
					item->setIcon( QIcon( ":/common/currentImage.gif" ) );
					item->setTextColor( QColor( 34, 139, 34 ) );
				}

				m_ImageStack->addItem( item );
			}
		}
	}
	const int numItems = m_ImageStack->findItems( QString( "*" ), Qt::MatchWrap | Qt::MatchWildcard ).size() ;
	if ( numItems > 1 && m_ImageStack->currentItem() ) {
		m_Interface.moveUp->setEnabled(m_ImageStack->currentIndex().row() > 0);
		m_Interface.moveDown->setEnabled( m_ImageStack->currentIndex().row() <= numItems - 1);
	} else {
		m_Interface.moveDown->setEnabled(false);
		m_Interface.moveUp->setEnabled(false );
	}
	m_Interface.checkViewAllImages->setEnabled( m_ViewerCore->getUICore()->getEnsembleList().size() > 1 );
}

void ImageStackWidget::itemClicked ( QListWidgetItem* item)
{
	if( m_ViewerCore->hasImage() ) {
		const int numItems = m_ImageStack->findItems( QString( "*" ), Qt::MatchWrap | Qt::MatchWildcard ).size() ;
		if ( numItems > 1 && m_ImageStack->currentItem() ) {
			m_Interface.moveUp->setEnabled(m_ImageStack->currentIndex().row() > 0);
			m_Interface.moveDown->setEnabled( m_ImageStack->currentIndex().row() < numItems - 1);
		}else {
			m_Interface.moveDown->setEnabled(false);
			m_Interface.moveUp->setEnabled(false );
		}
		ImageHolder::Pointer image = m_ViewerCore->getImageMap().at( item->data(Qt::UserRole).toString().toStdString() );
		if( image->getImageProperties().imageType == ImageHolder::statistical_image ) {
			m_Interface.actionImage_type_stats->setChecked(true);
			m_Interface.actionStructural_image->setChecked(false);
		} else {
			m_Interface.actionImage_type_stats->setChecked(false);
			m_Interface.actionStructural_image->setChecked(true);
		}
	}
	
}


void ImageStackWidget::itemChanged( QListWidgetItem *item )
{
	if( m_ViewerCore->hasImage() ) {
		ImageHolder::Pointer image = m_ViewerCore->getImageMap().at( item->data(Qt::UserRole).toString().toStdString() );
		if( item->checkState() == Qt::Checked ) {
			image->getImageProperties().isVisible = true ;
		} else {
			image->getImageProperties().isVisible = false ;
		}
		m_ViewerCore->getUICore()->refreshUI();
		m_ViewerCore->updateScene();
	}

}

void ImageStackWidget::itemSelected( QListWidgetItem *item )
{
	m_ViewerCore->setCurrentImage( m_ViewerCore->getImageMap().at( item->data(Qt::UserRole).toString().toStdString() ) );
	synchronize();
	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->updateScene();
}

void ImageStackWidget::closeAllImages()
{
	QList<QListWidgetItem *> items = m_ImageStack->findItems( QString( "*" ), Qt::MatchWrap | Qt::MatchWildcard );
	BOOST_FOREACH( QList<QListWidgetItem *>::const_reference item, items ) {
		ImageHolder::Map::iterator iter = m_ViewerCore->getImageMap().find( item->data(Qt::UserRole).toString().toStdString() );

		if( iter != m_ViewerCore->getImageMap().end() ) {
			m_ViewerCore->closeImage( iter->second, false );
		}
	}
	m_ViewerCore->getUICore()->refreshUI();
}


void ImageStackWidget::closeImage()
{
	if(  m_ImageStack->currentItem() ) {
		m_ViewerCore->closeImage( m_ViewerCore->getImageMap().at( m_ImageStack->currentItem()->data(Qt::UserRole).toString().toStdString() ) );
	}
}

void ImageStackWidget::distributeImages()
{
// 	ImageHolder::List tmpImageList;
// 	BOOST_FOREACH( ImageHolder::List::reference image, m_ViewerCore->getImageList() ) {
// 		tmpImageList.push_back( image);
// 		m_ViewerCore->getDataContainer().erase( image.first );
// 		BOOST_FOREACH( WidgetEnsembleComponent::Map::reference ensemble, m_ViewerCore->getUICore()->getWidgets() ) {
// 			ensemble.first->removeImage(image.second);
// 		}
// 	}
// 	m_ViewerCore->getUICore()->removeAllWidgetEnsembles();
// 	m_ViewerCore->getUICore()->refreshUI();
// 	BOOST_FOREACH( DataContainer::const_reference image, tmpContainer ) {
// 		m_ViewerCore->getDataContainer().insert( image );
// 		m_ViewerCore->getUICore()->createViewWidgetEnsemble( m_ViewerCore->getSettings()->getPropertyAs<std::string>("defaultViewWidgetIdentifier"), image.second );
// 	}
// 	m_ViewerCore->getUICore()->refreshUI();
// 	m_ViewerCore->settingsChanged();
// 	m_ViewerCore->updateScene();
}

void ImageStackWidget::moveDown()
{
	ImageHolder::Pointer image = m_ViewerCore->getImageMap().at( m_ImageStack->currentItem()->data(Qt::UserRole).toString().toStdString() );
	
}

void ImageStackWidget::moveUp()
{

}


}
}
}