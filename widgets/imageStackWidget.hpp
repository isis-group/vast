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



class ImageStackWidget : public QWidget
{
	Q_OBJECT
public:
	ImageStackWidget( QWidget *parent, QViewerCore *core );

	Ui::imageStackWidget &getInterface() { return m_Interface; }

	void synchronize();

public Q_SLOTS:
	void itemClicked( QListWidgetItem *);
	void itemSelected( QListWidgetItem * );
	void removeButtonClicked();
	
private:
	QViewerCore *m_Core;
	Ui::imageStackWidget m_Interface;

};


}
}
}



#endif
