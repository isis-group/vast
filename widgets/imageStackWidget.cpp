#include "imageStackWidget.hpp"

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
}

void ImageStackWidget::updateImageStack()
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



}
}
}