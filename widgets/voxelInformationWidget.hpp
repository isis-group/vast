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

private:
	ViewerCoreBase *m_Core;
};

	

	
	
}}}




#endif