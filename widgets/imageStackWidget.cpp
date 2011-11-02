#include "imageStackWidget.hpp"
#include <viewercorebase.hpp>

namespace isis
{
namespace viewer
{
namespace widget
{

ImageStackWidget::ImageStackWidget( QWidget *parent, QViewerCore *core )
	: QWidget( parent ), m_Core( core )
{
	m_Interface.setupUi( this );
	
	m_Interface.imageStack->setEditTriggers(QAbstractItemView::NoEditTriggers);
	connect( m_Interface.imageStack, SIGNAL( itemActivated(QListWidgetItem*) ), this, SLOT( itemSelected(QListWidgetItem*)));
	connect( m_Interface.imageStack, SIGNAL( itemChanged(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));

}

void ImageStackWidget::synchronize()
{
	m_Interface.imageStack->clear();
	BOOST_FOREACH( DataContainer::const_reference imageRef, m_Core->getDataContainer() ) {
		QListWidgetItem *item = new QListWidgetItem;
		QString sD = imageRef.second->getPropMap().getPropertyAs<std::string>( "sequenceDescription" ).c_str();
		item->setText( QString( imageRef.second->getFileNames().front().c_str() ) );
		item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable );

		if( imageRef.second->getPropMap().getPropertyAs<bool>( "isVisible" ) ) {
			item->setCheckState( Qt::Checked );
		} else {
			item->setCheckState( Qt::Unchecked );
		}

		if( m_Core->getCurrentImage().get() == imageRef.second.get() ) {
			item->setIcon( QIcon( ":/common/currentImage.gif" ) );
		}

		m_Interface.imageStack->addItem( item );
	}
}


void ImageStackWidget::itemClicked( QListWidgetItem *item )
{
	if( item->checkState() == Qt::Checked ) {
		m_Core->getDataContainer().at( item->text().toStdString() )->getPropMap().setPropertyAs<bool>("isVisible", true ) ;
	} else {
		m_Core->getDataContainer().at( item->text().toStdString() )->getPropMap().setPropertyAs<bool>("isVisible", false ) ;
	}
	m_Core->getUI()->refreshUI();
	m_Core->updateScene();
		
}

void ImageStackWidget::itemSelected(QListWidgetItem *item )
{
	m_Core->setCurrentImage( m_Core->getDataContainer().at( item->text().toStdString() ) );
	synchronize();
	m_Core->getUI()->refreshUI();
	m_Core->updateScene();
}


}
}
}