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
	setMaximumHeight( m_Widget->m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "maxOptionWidgetHeight" ) - 4 );
	setMinimumHeight( m_Widget->m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "minOptionWidgetHeight" ) - 4 );
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
	m_Interface.layout->addWidget( m_ImageStack );

	m_ImageStack->setEditTriggers( QAbstractItemView::NoEditTriggers );
	connect( m_ImageStack, SIGNAL( itemActivated( QListWidgetItem * ) ), this, SLOT( itemSelected( QListWidgetItem * ) ) );
	connect( m_ImageStack, SIGNAL( itemChanged( QListWidgetItem * ) ), this, SLOT( itemChanged( QListWidgetItem * ) ) );
	connect( m_ImageStack, SIGNAL( itemPressed(QListWidgetItem*)), this, SLOT( itemClicked(QListWidgetItem*)));
	connect( m_Interface.actionClose_image, SIGNAL( triggered() ), this, SLOT( closeImage() ) );
	connect( m_Interface.actionDistribute_images, SIGNAL( triggered() ), this, SLOT( distributeImages() ) );
	connect( m_Interface.actionClose_all_images, SIGNAL( triggered() ), this, SLOT( closeAllImages() ) );
	connect( m_Interface.actionImage_type_stats, SIGNAL( triggered(bool)), this, SLOT(toggleStatsType()));
	connect( m_Interface.actionStructural_image, SIGNAL( triggered(bool)), this, SLOT( toggleStructsType()));

}

void ImageStackWidget::toggleStatsType()
{
	m_Interface.actionStructural_image->setChecked( false );
	m_Interface.actionImage_type_stats->setChecked( true );
	boost::shared_ptr< ImageHolder> image = m_ViewerCore->getDataContainer().at( m_ImageStack->currentItem()->text().toStdString() );
	image->getImageProperties().imageType = ImageHolder::statistical_image;
	image->getImageProperties().lut = m_ViewerCore->getSettings()->getPropertyAs<std::string>("LutZMap");
	image->updateColorMap();
	m_ViewerCore->setMode( ViewerCoreBase::statistical_mode );
	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->updateScene();
}

void ImageStackWidget::toggleStructsType()
{
	m_Interface.actionStructural_image->setChecked( true );
	m_Interface.actionImage_type_stats->setChecked( false );
	boost::shared_ptr< ImageHolder> image = m_ViewerCore->getDataContainer().at( m_ImageStack->currentItem()->text().toStdString() );
	image->getImageProperties().imageType = ImageHolder::structural_image;
	image->getImageProperties().lut = m_ViewerCore->getSettings()->getPropertyAs<std::string>("lutStructural");
	image->updateColorMap();
	m_ViewerCore->setMode( ViewerCoreBase::default_mode );
	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->updateScene();
}



void ImageStackWidget::synchronize()
{
	m_Interface.frame->setMaximumHeight( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "maxOptionWidgetHeight" ) );
	m_Interface.frame->setMinimumHeight( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "minOptionWidgetHeight" ) );
	m_ImageStack->clear();
	BOOST_FOREACH( DataContainer::const_reference imageRef, m_ViewerCore->getDataContainer() ) {
		if( !( m_ViewerCore->getMode() == ViewerCoreBase::statistical_mode && imageRef.second->getImageProperties().imageType == ImageHolder::structural_image ) ) {
			QListWidgetItem *item = new QListWidgetItem;
			QString sD = imageRef.second->getPropMap().getPropertyAs<std::string>( "sequenceDescription" ).c_str();
			item->setText( QString( imageRef.second->getFileNames().front().c_str() ) );
			item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable );

			if( imageRef.second->getImageProperties().isVisible ) {
				item->setCheckState( Qt::Checked );
			} else {
				item->setCheckState( Qt::Unchecked );
			}

			if( m_ViewerCore->getCurrentImage().get() == imageRef.second.get() ) {
				item->setIcon( QIcon( ":/common/currentImage.gif" ) );
			}

			m_ImageStack->addItem( item );
		}
	}

}
void ImageStackWidget::itemClicked ( QListWidgetItem* item)
{
	if( m_ViewerCore->hasImage() ) {
		boost::shared_ptr< ImageHolder > image = m_ViewerCore->getDataContainer().at( item->text().toStdString() );
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
		boost::shared_ptr< ImageHolder > image = m_ViewerCore->getDataContainer().at( item->text().toStdString() );
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
	m_ViewerCore->setCurrentImage( m_ViewerCore->getDataContainer().at( item->text().toStdString() ) );
	synchronize();
	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->updateScene();
}

void ImageStackWidget::closeAllImages()
{
	QList<QListWidgetItem *> items = m_ImageStack->findItems( QString( "*" ), Qt::MatchWrap | Qt::MatchWildcard );
	DataContainer::iterator iter;
	BOOST_FOREACH( QList<QListWidgetItem *>::const_reference item, items ) {
		iter = m_ViewerCore->getDataContainer().find( item->text().toStdString() );

		if( iter != m_ViewerCore->getDataContainer().end() ) {
			m_ViewerCore->closeImage( iter->second, false );
		}
	}
	m_ViewerCore->getUICore()->refreshUI();
}


void ImageStackWidget::closeImage()
{
	if(  m_ImageStack->currentItem() ) {
		m_ViewerCore->closeImage( m_ViewerCore->getDataContainer().at( m_ImageStack->currentItem()->text().toStdString() ) );
	}
}

void ImageStackWidget::distributeImages()
{
	DataContainer tmpContainer;
	BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) {

		tmpContainer.insert( image );
		m_ViewerCore->getDataContainer().erase( image.first );
		BOOST_FOREACH( WidgetEnsembleComponent::Map::reference ensemble, m_ViewerCore->getUICore()->getWidgets() ) {
			ensemble.first->removeImage(image.second);
		}
	}
	m_ViewerCore->getUICore()->removeAllWidgetEnsembles();
	m_ViewerCore->getUICore()->refreshUI();
	BOOST_FOREACH( DataContainer::const_reference image, tmpContainer ) {
		m_ViewerCore->getDataContainer().insert( image );
		m_ViewerCore->getUICore()->createViewWidgetEnsemble( m_ViewerCore->getSettings()->getPropertyAs<std::string>("defaultViewWidgetIdentifier"), image.second );
	}
	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->settingsChanged();
	m_ViewerCore->updateScene();
}


}
}
}