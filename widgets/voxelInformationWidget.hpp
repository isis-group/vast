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
	void voxPosChanged();
	void physPosChanged();
	void updateLowerUpperThreshold(  );

private:
	isis::viewer::QViewerCore *m_ViewerCore;
	Ui::voxelInformationWidget m_Interface;
	bool isConnected;
	void connectSignals();
	void disconnectSignals();
	void reconnectSignals();

	template<typename TYPE>
	void displayIntensity( const util::ivector4 &coords ) const {
		const util::Value<TYPE> vIntensity ( m_ViewerCore->getCurrentImage()->getISISImage()->voxel<TYPE>( coords[0], coords[1], coords[2], coords[3] ) );
		const double intensity = roundNumber<double>( vIntensity, 4 );
		m_Interface.intensityValue->setText( QString::number( intensity ) );
	}

	template<typename TYPE>
	void displayIntensityColor( const util::ivector4 &coords ) const {
		util::checkType<TYPE>();
		const util::Value<TYPE> vIntensity ( m_ViewerCore->getCurrentImage()->getISISImage()->voxel<TYPE>( coords[0], coords[1], coords[2], coords[3] ) );
		const std::string intensityStr = static_cast<const util::_internal::ValueBase &>( vIntensity ).as<std::string>();
		m_Interface.intensityValue->setText( intensityStr.c_str() );
	}

};

}
}
}




#endif