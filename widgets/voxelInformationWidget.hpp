#ifndef VOXELINFORMATIONWIDGET_HPP
#define VOXELINFORMATIONWIDGET_HPP

#include "ui_voxelInformationWidget.h"
#include "common.hpp"
#include "qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{

class VoxelInformationWidget : public QWidget
{
	Q_OBJECT
public:
	VoxelInformationWidget( QWidget *parent, QViewerCore *core );

	Ui::voxelInformationWidget &getInterface() { return m_Interface; }

public Q_SLOTS:
	void synchronize();
	void synchronizePos( util::ivector4 voxelCoords );
	void synchronizePos( util::fvector4 physicalCoords );
	void voxPosChanged( int dummy );
	void physPosChanged( double dummy );

private:
	isis::viewer::QViewerCore *m_Core;
	Ui::voxelInformationWidget m_Interface;

	template<typename TYPE>
	void displayIntensity( util::ivector4 coords ) {
		util::Value<TYPE> vIntensity ( m_Core->getCurrentImage()->getISISImage()->voxel<TYPE>( coords[0], coords[1], coords[2], coords[3] ) );
		double intensity = roundNumber<double>( vIntensity, 2 );
		m_Interface.intensityValue->setText( QString::number( intensity ) );
		m_Core->getCurrentImage()->getPropMap().setPropertyAs<double>( "currentIntensity", intensity );
	}
};

}
}
}




#endif