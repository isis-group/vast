/****************************************************************
 *
 * <Copyright information>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Author: Erik Türke, tuerke@cbs.mpg.de
 *
 * voxelInformationWidget.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef VOXELINFORMATIONWIDGET_HPP
#define VOXELINFORMATIONWIDGET_HPP

#include "ui_voxelInformationWidget.h"
#include "../viewer/common.hpp"
#include "../viewer/qviewercore.hpp"
#include <QThread>

namespace isis
{
namespace viewer
{
namespace ui
{


class VoxelInformationWidget : public QWidget
{
	Q_OBJECT
	class TimePlayThread : public QThread
	{
		QViewerCore *m_core;
		int m_start;
		int m_end;
		Ui::voxelInformationWidget *m_interface;

	public:
		TimePlayThread( QObject *parent, QViewerCore *core, Ui::voxelInformationWidget *interface )
			: QThread( parent ), m_core( core ), m_start( 0 ), m_end( 0 ), m_interface( interface ) {} ;
		void setStartStop( int start, int stop ) { m_start = start; m_end = stop; }
		void run() {
			uint16_t deleyTime = m_core->getSettings()->getPropertyAs<uint16_t>( "timeseriesPlayDelayTime" );
			uint16_t t = m_start;

			while( true ) {
				t = t == m_end ? 0 : t;
				m_interface->timestepSpinBox->setValue( t );
				msleep( deleyTime );
				QApplication::processEvents( QEventLoop::AllEvents );
				t++;

			}
		}
	};

public:
	VoxelInformationWidget( QWidget *parent, QViewerCore *core );

	Ui::voxelInformationWidget &getInterface() { return m_Interface; }

public Q_SLOTS:
	void synchronize();
	void synchronizePos( util::ivector4 voxelCoords );
	void synchronizePos( util::fvector3 physicalCoords );
	void voxPosChanged();
	void physPosChanged();
	void updateLowerUpperThreshold(  );
	void playTimecourse();
	void timePlayFinished();
	void onLUTMenuClicked();
	void timeStepChanged( int );

private:
	isis::viewer::QViewerCore *m_ViewerCore;
	Ui::voxelInformationWidget m_Interface;
	bool isConnected;
	void connectSignals();
	void disconnectSignals();
	void reconnectSignals();
	TimePlayThread *m_tThread;
	QLabel *m_UpperHalfColormapLabel;
	QWidget *m_sepWidget;
	QLabel *m_LowerHalfColormapLabel;
	QLabel *m_LabelMin;
	QLabel *m_LabelMax;
	QLabel *m_LowerThreshold;
	QLabel *m_UpperThreshold;
	QVBoxLayout *m_LayoutLeft;
	QVBoxLayout *m_LayoutRight;


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
		const std::string intensityStr = static_cast<const util::ValueBase &>( vIntensity ).as<std::string>();
		m_Interface.intensityValue->setText( intensityStr.c_str() );
	}

};

}
}
}




#endif
