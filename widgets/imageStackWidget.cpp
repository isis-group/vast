#include "imageStackWidget.hpp"
#include <viewercorebase.hpp>
#include <qviewercore.hpp>
#include <uicore.hpp>

namespace isis
{
namespace viewer
{
namespace widget
{

ImageStack::ImageStack(QWidget* parent, ImageStackWidget *widget )
	: QListWidget(parent), 
	m_Widget(widget)
{
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setVerticalScrollMode(ScrollPerItem);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setMaximumHeight(m_Widget->m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("maxOptionWidgetHeight") - 4);
	setMinimumHeight(m_Widget->m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("minOptionWidgetHeight") - 4);	
}

void ImageStack::contextMenuEvent(QContextMenuEvent* event )
{
	QMenu menu(this);
	menu.addAction( m_Widget->m_Interface.actionClose_image );
	menu.addAction( m_Widget->m_Interface.actionDistribute_images);
	menu.addSeparator();
	menu.addAction( m_Widget->m_Interface.actionClose_all_images);
	menu.exec( event->globalPos() );
}

void ImageStack::mousePressEvent(QMouseEvent *e )
{
	if( e->button() == Qt::LeftButton && geometry().contains( e->pos() ) && QApplication::keyboardModifiers() == Qt::AltModifier) {
		QDrag *drag = new QDrag(this);
		QMimeData *mimeData = new QMimeData;
		mimeData->setText( itemAt(e->pos())->text() );
		drag->setMimeData( mimeData );
		drag->setPixmap( QIcon( ":/common/vast.jpg" ).pixmap(15) );
		drag->exec();
	}
	QListWidget::mousePressEvent(e);
}

	

//ImageStackWidget
	
	
ImageStackWidget::ImageStackWidget( QWidget *parent, QViewerCore *core )
	: QWidget( parent ), m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	m_Interface.actionClose_image->setIconVisibleInMenu(true);
	m_Interface.actionDistribute_images->setIconVisibleInMenu(true);
	m_Interface.actionClose_all_images->setIconVisibleInMenu(true);
	m_ImageStack = new ImageStack(this, this);
	m_Interface.layout->addWidget( m_ImageStack );

	m_ImageStack->setEditTriggers( QAbstractItemView::NoEditTriggers );
	connect( m_ImageStack, SIGNAL( itemActivated( QListWidgetItem * ) ), this, SLOT( itemSelected( QListWidgetItem * ) ) );
	connect( m_ImageStack, SIGNAL( itemChanged( QListWidgetItem * ) ), this, SLOT( itemClicked( QListWidgetItem * ) ) );
	connect( m_Interface.actionClose_image, SIGNAL( triggered()), this, SLOT( closeImage()));
	connect( m_Interface.actionDistribute_images, SIGNAL( triggered()), this, SLOT( distributeImages()) );
	connect( m_Interface.actionClose_all_images, SIGNAL( triggered()), this, SLOT( closeAllImages()));

}

void ImageStackWidget::synchronize()
{
	m_Interface.frame->setMaximumHeight(m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("maxOptionWidgetHeight"));
	m_Interface.frame->setMinimumHeight(m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("minOptionWidgetHeight"));
	m_ImageStack->clear();
	BOOST_FOREACH( DataContainer::const_reference imageRef, m_ViewerCore->getDataContainer() ) 
	{
		if( !( m_ViewerCore->getMode() == ViewerCoreBase::zmap && imageRef.second->imageType == ImageHolder::anatomical_image ) ) 
		{
			QListWidgetItem *item = new QListWidgetItem;
			QString sD = imageRef.second->getPropMap().getPropertyAs<std::string>( "sequenceDescription" ).c_str();
			item->setText( QString( imageRef.second->getFileNames().front().c_str() ) );
			item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable );

			if( imageRef.second->isVisible ) {
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


void ImageStackWidget::itemClicked( QListWidgetItem *item )
{
	if( item->checkState() == Qt::Checked ) {
		m_ViewerCore->getDataContainer().at( item->text().toStdString() )->isVisible = true ;
	} else {
		m_ViewerCore->getDataContainer().at( item->text().toStdString() )->isVisible = false ;
	}

	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->updateScene();

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
	QList<QListWidgetItem *> items = m_ImageStack->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
	BOOST_FOREACH( QList<QListWidgetItem *>::const_reference item, items ) 
	{
		m_ViewerCore->closeImage( m_ViewerCore->getDataContainer().at( item->text().toStdString() ) );
	}
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
		BOOST_FOREACH( std::list< WidgetInterface *>::const_reference widget, image.second->getWidgetList() ) {
			widget->removeImage( image.second );
		}
	}
	m_ViewerCore->getUICore()->getEnsembleList().clear();
	m_ViewerCore->getUICore()->refreshUI();
	BOOST_FOREACH( DataContainer::const_reference image, tmpContainer ) {
		m_ViewerCore->getDataContainer().insert( image );
		m_ViewerCore->getUICore()->createViewWidgetEnsemble( "", image.second );
	}
	m_ViewerCore->getUICore()->refreshUI();
	m_ViewerCore->settingsChanged();
	m_ViewerCore->updateScene();
}


}
}
}