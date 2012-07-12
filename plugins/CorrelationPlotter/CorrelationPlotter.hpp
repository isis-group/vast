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
 * Author: Erik Tuerke, tuerke@cbs.mpg.de
 *
 * CorrelationPlotter.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef CORRELATIONPLOTTER_HPP
#define CORRELATIONPLOTTER_HPP

#include "ui_correlationPlotter.h"
#include <QWidget>
#include "qviewercore.hpp"
#include <cmath>

namespace isis
{
namespace viewer
{
namespace plugin
{

class CorrelationPlotterDialog : public QDialog
{
	Q_OBJECT
	typedef double MapImageType;
	typedef uint16_t InternalFunctionalImageType;
public:
	CorrelationPlotterDialog( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	virtual void showEvent( QShowEvent * );
	virtual void closeEvent( QCloseEvent * );
	void physicalCoordsChanged( util::fvector3 );
	bool createCorrelationMap();
	void calculateCorrelation( bool all = false );
	void lockClicked();

private:
	ViewerCoreBase::Mode m_OrigMode;
	Ui::correlationPlotterDialog m_Interface;
	QViewerCore *m_ViewerCore;
	boost::shared_ptr<ImageHolder> m_CurrentCorrelationMap;
	boost::shared_ptr<ImageHolder> m_CurrentFunctionalImage;
	boost::scoped_ptr< isis::data::Chunk > m_InternalChunk;
	util::ivector4 m_CurrentVoxelPos;


	void _internCalculateCorrelation( const util::ivector4 &vec, const double &s_x, const double &_x, const InternalFunctionalImageType *vx, const size_t &n, const size_t &vol  );

};


}
}
}

#endif