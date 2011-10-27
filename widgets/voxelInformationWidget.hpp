#ifndef VOXELINFORMATIONWIDGET_HPP
#define VOXELINFORMATIONWIDGET_HPP

#include "ui_voxelInformationWidget.h"
#include "viewercorebase.hpp"

namespace isis {
namespace viewer {
namespace widget {
	
class VoxelInformationWidget : public QWidget {
	Q_OBJECT
public:
	VoxelInformationWidget(QWidget* parent, ViewerCoreBase *core )
		:QWidget(parent),
		m_Core(core)
	{
		interface.setupUi( this );
	}
	
	Ui::voxelInformationWidget interface;
	void synchronize() 
	{
		QSize size = interface.colromapLabel->size();
		std::string lutName = m_Core->getCurrentImage()->getPropMap().getPropertyAs<std::string>("lut");
		interface.colromapLabel->setPixmap( 
			m_Core->getColorHandler()->getIcon(lutName, size.width(), size.height()-10 ).pixmap(size.width(), size.height()-10) );
		interface.labelMin->setText( m_Core->getCurrentImage()->getMinMax().first->as<std::string>().c_str() );
		interface.labelMax->setText( m_Core->getCurrentImage()->getMinMax().second->as<std::string>().c_str() );
	}
private:
	ViewerCoreBase *m_Core;
};

	

	
	
}}}




#endif