#include "imageStackWidget.hpp"
#include <viewercorebase.hpp>

namespace isis
{
namespace viewer
{
namespace widget
{

ImageStackWidget::ImageStackWidget( QWidget *parent, QViewerCore *core )
	: QWidget( parent ), m_ViewerCore( core )
{
	m_Interface.setupUi( this );

	m_Interface.imageStack->setEditTriggers( QAbstractItemView::NoEditTriggers );
	connect( m_Interface.imageStack, SIGNAL( itemActivated( QListWidgetItem * ) ), this, SLOT( itemSelected( QListWidgetItem * ) ) );
	connect( m_Interface.imageStack, SIGNAL( itemChanged( QListWidgetItem * ) ), this, SLOT( itemClicked( QListWidgetItem * ) ) );
	connect( m_Interface.buttonRemoveImage, SIGNAL( clicked() ), this, SLOT( removeButtonClicked() ) );

}

void ImageStackWidget::synchronize()
{
	m_Interface.imageStack->clear();
	BOOST_FOREACH( DataContainer::const_reference imageRef, m_ViewerCore->getDataContainer() ) {
		QListWidgetItem *item = new QListWidgetItem;
		QString sD = imageRef.second->getPropMap().getPropertyAs<std::string>( "sequenceDescription" ).c_str();
		item->setText( QString( imageRef.second->getFileNames().front().c_str() ) );
		item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable );

		if( imageRef.second->getPropMap().getPropertyAs<bool>( "isVisible" ) ) {
			item->setCheckState( Qt::Checked );
		} else {
			item->setCheckState( Qt::Unchecked );
		}

		if( m_ViewerCore->getCurrentImage().get() == imageRef.second.get() ) {
			item->setIcon( QIcon( ":/common/currentImage.gif" ) );
		}

		m_Interface.imageStack->addItem( item );
	}
}


void ImageStackWidget::itemClicked( QListWidgetItem *item )
{
	if( item->checkState() == Qt::Checked ) {
		m_ViewerCore->getDataContainer().at( item->text().toStdString() )->getPropMap().setPropertyAs<bool>( "isVisible", true ) ;
	} else {
		m_ViewerCore->getDataContainer().at( item->text().toStdString() )->getPropMap().setPropertyAs<bool>( "isVisible", false ) ;
	}

	m_ViewerCore->getUI()->refreshUI();
	m_ViewerCore->updateScene();

}

void ImageStackWidget::itemSelected( QListWidgetItem *item )
{
	m_ViewerCore->setCurrentImage( m_ViewerCore->getDataContainer().at( item->text().toStdString() ) );
	synchronize();
	m_ViewerCore->getUI()->refreshUI();
	m_ViewerCore->updateScene();
}

void ImageStackWidget::removeButtonClicked()
{
	boost::shared_ptr<ImageHolder> image = m_ViewerCore->getDataContainer().at( m_Interface.imageStack->currentItem()->text().toStdString() );
	BOOST_FOREACH( std::list< QWidgetImplementationBase *>::const_reference widget, image->getWidgetList() ) {
		widget->removeImage( image );
	}
	
	if( m_ViewerCore->getCurrentImage().get() == m_ViewerCore->getDataContainer().at( m_Interface.imageStack->currentItem()->text().toStdString() ).get() ) {
		std::list<std::string> tmpList;
		BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() )
		{
			tmpList.push_back( image.first );
		}
		tmpList.erase( std::find( tmpList.begin(), tmpList.end(), m_Interface.imageStack->currentItem()->text().toStdString() ) );
		if( tmpList.size() ) {
			m_ViewerCore->setCurrentImage( m_ViewerCore->getDataContainer().at( tmpList.front() ) );
		} else {
			m_ViewerCore->setCurrentImage(boost::shared_ptr<ImageHolder>());
		}
	}
	m_ViewerCore->getDataContainer().erase( m_Interface.imageStack->currentItem()->text().toStdString() );
	m_ViewerCore->getUI()->refreshUI();
	m_ViewerCore->updateScene();
}

}
}
}