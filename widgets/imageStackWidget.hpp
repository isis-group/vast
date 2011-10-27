#ifndef IMAGESTACKWIDGET_HPP
#define IMAGESTACKWIDGET_HPP

#include "ui_imageStackWidget.h"
#include "viewercorebase.hpp"

namespace isis {
namespace viewer {
namespace widget {
	


class ImageStackWidget : public QWidget
{
	Q_OBJECT
public:
	ImageStackWidget( QWidget *parent, ViewerCoreBase *core ) : QWidget( parent ), m_Core(core) 
	{
		interface.setupUi(this);
	}
	
	Ui::imageStackWidget interface;
	
	void updateImageStack() {
		interface.imageStack->clear();
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
			interface.imageStack->addItem( item );
		}
	}
private:
	ViewerCoreBase *m_Core;

};
	
	
}}}



#endif
