#ifndef MASKEDIT_HPP
#define MASKEDIT_HPP

#include "ui_maskEdit.h"
#include "qviewercore.hpp"
#include <DataStorage/chunk.hpp>
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
	void physicalCoordChanged( util::fvector4 physCoord );
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

	UICore::ViewWidgetEnsembleType m_CurrentWidgetEnsemble;

	template<typename TYPE>
	void manipulateVoxel( const util::fvector4 physCoord, const TYPE &value, boost::shared_ptr<ImageHolder> image ) {
		const util::ivector4 voxel = image->getISISImage()->getIndexFromPhysicalCoords( physCoord, true );
		const util::ivector4 imageSize = image->getImageSize();
		util::ivector4 start;
		util::ivector4 end;
		const bool cut = m_Interface.cut->isChecked();

		for( unsigned short i = 0; i < 3; i++ ) {
			start[i] = ( voxel[i] - m_Radius ) < 0 ? 0 : voxel[i] - m_Radius;
			end[i] = ( voxel[i] + m_Radius ) > imageSize[i] ? imageSize[i] : voxel[i] + m_Radius;
		}

		unsigned short radSquare = m_Radius * m_Radius;
		#pragma omp parallel for

		for( unsigned short k = start[2] + 1; k < end[2]; k++ ) {
			for( unsigned short j = start[1] + 1; j < end[1]; j++ ) {
				for( unsigned short i = start[0] + 1; i < end[0]; i++ ) {
					int x = voxel[0] - i;
					int y = voxel[1] - j;
					int z = voxel[2] - k;

					if( x *x + y *y + z *z <= radSquare ) {
						if ( !cut ) {
							image->getISISImage()->voxel<TYPE>( i, j, k ) = value;
							image->getChunkVector()[0].voxel<InternalImageType>( i, j, k ) = value;
						} else {
							image->getISISImage()->voxel<TYPE>( i, j, k ) = std::numeric_limits<TYPE>::min();
							image->getChunkVector()[0].voxel<InternalImageType>( i, j, k ) = std::numeric_limits<InternalImageType>::min();
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