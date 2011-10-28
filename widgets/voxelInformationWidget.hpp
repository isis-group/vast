#ifndef VOXELINFORMATIONWIDGET_HPP
#define VOXELINFORMATIONWIDGET_HPP

#include "ui_voxelInformationWidget.h"
#include "viewercorebase.hpp"

namespace isis {
namespace viewer {
namespace widget {
	
class VoxelInformationWidget : public QWidget 
{
	Q_OBJECT
public:
	VoxelInformationWidget(QWidget* parent, ViewerCoreBase *core );
	
	Ui::voxelInformationWidget &getInterface() { return m_Interface; }
	
	void synchronize();

private:
	ViewerCoreBase *m_Core;
	Ui::voxelInformationWidget m_Interface;
};

	

	
	
}}}




#endif