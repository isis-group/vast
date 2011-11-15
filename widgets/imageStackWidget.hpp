#ifndef IMAGESTACKWIDGET_HPP
#define IMAGESTACKWIDGET_HPP

#include "ui_imageStackWidget.h"
#include "qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{

	
class ImageStack : public QListWidget
{
	Q_OBJECT
public:
	ImageStack( QWidget *parent, ImageStackWidget *widget );
public Q_SLOTS:
	void contextMenuEvent( QContextMenuEvent *);
private:
	ImageStackWidget *m_Widget;
	
};


class ImageStackWidget : public QWidget
{
	Q_OBJECT
public:
	ImageStackWidget( QWidget *parent, QViewerCore *core );

	Ui::imageStackWidget &getInterface() { return m_Interface; }

	void synchronize();
	
	friend class ImageStack;

public Q_SLOTS:
	void itemClicked( QListWidgetItem * );
	void itemSelected( QListWidgetItem * );
	void closeImage();
	void distributeImages();
	void closeAllImages();

private:
	QViewerCore *m_ViewerCore;
	Ui::imageStackWidget m_Interface;
	ImageStack *m_ImageStack;
	void _closeImage( QString );

};


}
}
}



#endif
