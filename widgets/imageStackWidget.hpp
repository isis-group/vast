#ifndef IMAGESTACKWIDGET_HPP
#define IMAGESTACKWIDGET_HPP

#include "ui_imageStackWidget.h"
#include "../viewer/qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace ui
{


class ImageStack : public QListWidget
{
	Q_OBJECT
public:
	ImageStack( QWidget *parent, ImageStackWidget *widget, QViewerCore *core );
public Q_SLOTS:
	void contextMenuEvent( QContextMenuEvent * );
	void mousePressEvent( QMouseEvent * );
	void dropEvent( QDropEvent *e );

private:
	ImageStackWidget *m_Widget;
	std::list<QAction *> m_WidgetActions;
	QViewerCore *m_ViewerCore;

};


class ImageStackWidget : public QWidget
{
	Q_OBJECT
public:
	ImageStackWidget( QWidget *parent, QViewerCore *core );

	Ui::imageStackWidget &getInterface() { return m_Interface; }



	friend class ImageStack;

public Q_SLOTS:
	void itemClicked( QListWidgetItem * );
	void itemChanged( QListWidgetItem * );
	void itemSelected( QListWidgetItem * );
	void closeImage();
	void distributeImages();
	void closeAllImages();
	void synchronize();
	void moveUp();
	void moveDown();
	void viewAllImagesClicked();
	void openImageInWidget( QString );
	void setZeroToBlack();


private:
	QViewerCore *m_ViewerCore;
	Ui::imageStackWidget m_Interface;
	ImageStack *m_ImageStack;

	bool checkEnsembleCanUp( const WidgetEnsemble::Pointer );
	bool checkEnsembleCanDown( const WidgetEnsemble::Pointer );
	const WidgetEnsemble::Pointer getEnsembleFromItem( const QListWidgetItem * );

	WidgetEnsemble::Pointer m_CurrentSelectedEnsemble;



};


}
}
}



#endif
