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
 * MaskEdit.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef MASKEDIT_HPP
#define MASKEDIT_HPP

#include "ui_maskEdit.h"
#include "qviewercore.hpp"
#include "widgetensemble.hpp"
#include <DataStorage/chunk.hpp>

namespace isis
{
namespace viewer
{
namespace plugin
{

class CreateMaskDialog;

class MaskEditDialog : public QDialog
{
	Q_OBJECT
public:
	friend class CreateMaskDialog;

	MaskEditDialog( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	void physicalCoordChanged( util::fvector3 physCoord, Qt::MouseButton );
	void radiusChange( int );
	void paintToggled();
	void pickColorClicked();
	void createEmptyMask();
	void editCurrentImage();
	virtual void closeEvent( QCloseEvent * );
	virtual void showEvent( QShowEvent * );
	void paintClicked();
	void cutClicked();

private:

	Ui::maskEditDialog m_Interface;
	QViewerCore *m_ViewerCore;
	boost::shared_ptr<ImageHolder> m_CurrentMask;
	unsigned short m_Radius;

	CreateMaskDialog *m_CreateMaskDialog;

	WidgetEnsemble::Pointer m_CurrentWidgetEnsemble;

	template<typename TYPE>
	void manipulateVoxel( const util::fvector3 physCoord, boost::shared_ptr<ImageHolder> image ) {
		util::ivector4 voxel = image->getISISImage()->getIndexFromPhysicalCoords( physCoord );
		image->correctVoxelCoords<3>( voxel );
		util::ivector4 start;
		util::ivector4 end;
		const size_t timestep = image->getImageProperties().voxelCoords[dim_time];

		for( unsigned short i = 0; i < 3; i++ ) {
			start[i] =  voxel[i] - m_Radius;
			end[i] =  voxel[i] + m_Radius;
		}

		unsigned short radSquare = m_Radius * m_Radius;

		util::Value<double> colorValue( m_Interface.colorEdit->value() );

		for( short k = start[2] + 1; k < end[2]; k++ ) {
			for( short j = start[1] + 1; j < end[1]; j++ ) {
				for( short i = start[0] + 1; i < end[0]; i++ ) {
					int x = voxel[0] - i;
					int y = voxel[1] - j;
					int z = voxel[2] - k;

					if( x *x + y *y + z *z <= radSquare ) {
						util::ivector4 finalVoxel( i, j, k );
						image->correctVoxelCoords<3>( finalVoxel );
						image->setTypedVoxel<TYPE>( finalVoxel[0], finalVoxel[1], finalVoxel[2], timestep, colorValue.as<TYPE>() );
					}
				}
			}
		}

		m_ViewerCore->getUICore()->refreshUI();
	}


};


}
}
}
#include "CreateMaskDialog.hpp"




#endif