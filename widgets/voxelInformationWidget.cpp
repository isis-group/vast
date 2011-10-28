#include "voxelInformationWidget.hpp"

namespace isis {
namespace viewer {
namespace widget {
	
VoxelInformationWidget::VoxelInformationWidget(QWidget* parent, ViewerCoreBase* core)
	:QWidget(parent),
		m_Core(core)
{
	m_Interface.setupUi( this );
}
	
void VoxelInformationWidget::synchronize()
{
	QSize size = m_Interface.colromapLabel->size();
	std::string lutName = m_Core->getCurrentImage()->getPropMap().getPropertyAs<std::string>("lut");
	m_Interface.colromapLabel->setPixmap( 
		m_Core->getColorHandler()->getIcon(lutName, size.width(), size.height()-10 ).pixmap(size.width(), size.height()-10) );
	m_Interface.labelMin->setText( m_Core->getCurrentImage()->getMinMax().first->as<std::string>().c_str() );
	m_Interface.labelMax->setText( m_Core->getCurrentImage()->getMinMax().second->as<std::string>().c_str() );
}

	
}}}