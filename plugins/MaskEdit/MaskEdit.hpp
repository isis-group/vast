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
#include <CoreUtils/value_base.hpp>
#include <boost/assign/list_of.hpp>


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
	void physicalCoordChanged( util::fvector4 physCoord, Qt::MouseButton );
	void radiusChange( int );
	void paintClicked();
	void cutClicked();
	void createEmptyMask();
	void editCurrentImage();
	virtual void closeEvent( QCloseEvent * );
	virtual void showEvent( QShowEvent * );

private:
	Ui::maskEditDialog m_Interface;
	QViewerCore *m_ViewerCore;
	boost::shared_ptr<ImageHolder> m_CurrentMask;
	unsigned short m_Radius;

	CreateMaskDialog *m_CreateMaskDialog;

	WidgetEnsemble::Pointer m_CurrentWidgetEnsemble;

	template<typename TYPE>
	void manipulateVoxel( const util::fvector4 physCoord, boost::shared_ptr<ImageHolder> image ) {
		const util::ivector4 voxel = image->getISISImage()->getIndexFromPhysicalCoords( physCoord, true );
		const util::ivector4 imageSize = image->getImageSize();
		util::ivector4 start;
		util::ivector4 end;
		const bool cut = m_Interface.cut->isChecked();

		for( unsigned short i = 0; i < 3; i++ ) {
			start[i] =  voxel[i] - m_Radius;
			end[i] =  voxel[i] + m_Radius;
		}

		unsigned short radSquare = m_Radius * m_Radius;


		for( short k = start[2] + 1; k < end[2]; k++ ) {
			for( short j = start[1] + 1; j < end[1]; j++ ) {
#ifdef _OPENMP
#pragma omp parallel for
#endif

				for( short i = start[0] + 1; i < end[0]; i++ ) {
					int x = voxel[0] - i;
					int y = voxel[1] - j;
					int z = voxel[2] - k;

					if( x *x + y *y + z *z <= radSquare ) {
						util::ivector4 finalVoxel( i, j, k );
						image->checkVoxelCoords( finalVoxel );

						if ( !cut ) {
							image->setTypedVoxel<TYPE>( finalVoxel[0], finalVoxel[1], finalVoxel[2], 0, image->getImageProperties().minMax.second->as<TYPE>() );
						} else {
							image->setTypedVoxel<TYPE>( finalVoxel[0], finalVoxel[1], finalVoxel[2], 0, image->getImageProperties().minMax.first->as<TYPE>() );
						}
					}
				}
			}
		}

	}


};


}
}
}
#include "CreateMaskDialog.hpp"




#endif