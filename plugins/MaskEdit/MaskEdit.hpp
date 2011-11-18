#ifndef MASKEDIT_HPP
#define MASKEDIT_HPP

#include "ui_maskEdit.h"
#include "qviewercore.hpp"
#include <DataStorage/chunk.hpp>

namespace isis {
namespace viewer {
namespace plugin {

class MaskEditDialog : public QDialog
{
	Q_OBJECT
public:
	MaskEditDialog( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	void physicalCoordChanged( util::fvector4 physCoord );
	void createEmptyMask();
	void radiusChange(int);
	void paintClicked();
	void cutClicked();
	virtual void closeEvent( QCloseEvent * );
	virtual void showEvent( QShowEvent *);
	
private:
	Ui::maskEditDialog m_Interface;
	QViewerCore *m_ViewerCore;
	boost::shared_ptr<ImageHolder> m_CurrentMask;
	unsigned short m_CurrentMajorTypeID;
	unsigned short m_Radius;
	
	UICore::ViewWidgetEnsembleType m_CurrentWidgetEnsemble;
	
	template<typename TYPE>
	void manipulateVoxel( const util::fvector4 physCoord, const TYPE &value, boost::shared_ptr<ImageHolder> image )
	{		
		const util::ivector4 voxel = image->getISISImage()->getIndexFromPhysicalCoords( physCoord, true );
		const util::ivector4 imageSize = image->getImageSize();
		util::ivector4 start;
		util::ivector4 end;
		const bool cut = m_Interface.cut->isChecked();
		for( size_t i = 0; i < 3; i++ ) {
			start[i] = ( voxel[i] - m_Radius ) < 0 ? 0 : voxel[i] - m_Radius;
			end[i] = ( voxel[i] + m_Radius ) > imageSize[i] ? imageSize[i] : voxel[i] + m_Radius;
		}
		unsigned short radSquare = m_Radius * m_Radius;
		for( unsigned short k = start[2] + 1; k < end[2]; k++ ) {
			for( unsigned short j = start[1] + 1; j < end[1]; j++ ) {
				for( unsigned short i = start[0] + 1; i < end[0]; i++ ) {
					int x = voxel[0] - i;
					int y = voxel[1] - j;
					int z = voxel[2] - k;
					if( x * x + y * y + z * z <= radSquare ) {
						if (!cut) {
							image->getISISImage()->voxel<TYPE>(i,j,k) = value;
							image->getChunkVector()[0].voxel<InternalImageType>(i,j,k) = value;
						} else {
							image->getISISImage()->voxel<TYPE>(i,j,k) = std::numeric_limits<TYPE>::min();
							image->getChunkVector()[0].voxel<InternalImageType>(i,j,k) = std::numeric_limits<InternalImageType>::min();
						}
					}
				}
			}
		}
			
	}
	
	template<typename TYPE>
	boost::shared_ptr<ImageHolder> _createEmptyMask( boost::shared_ptr<ImageHolder> refImage ){
		boost::shared_ptr< ImageHolder > retImage;
		util::ivector4 size = refImage->getImageSize();
		size[0] /= m_Interface.xRes->value();
		size[1] /= m_Interface.yRes->value();
		size[2] /= m_Interface.zRes->value();				
		isis::data::MemChunk<TYPE> ch( size[0], size[1], size[2] );
		ch.join( static_cast<isis::util::PropertyMap&>( *refImage->getISISImage() ) );		
		isis::data::Image mask ( ch);
		const util::fvector4 voxelSize = util::fvector4( m_Interface.xRes->value(), m_Interface.yRes->value(), m_Interface.zRes->value() );
		mask.setPropertyAs<util::fvector4>("voxelSize", voxelSize );
		
		mask.updateOrientationMatrices();
		if( m_Interface.maskName->text().size() ) {
			mask.setPropertyAs<std::string>("source", m_Interface.maskName->text().toStdString() );
		} else {
			mask.setPropertyAs<std::string>("source", "mask" );
		}
		retImage = m_ViewerCore->addImage( mask, ImageHolder::anatomical_image );
		retImage->minMax.first = isis::util::Value<TYPE>( std::numeric_limits<TYPE>::min() );
		retImage->minMax.second = isis::util::Value<TYPE>( std::numeric_limits<TYPE>::max() );
		retImage->internMinMax.first = isis::util::Value<TYPE>( std::numeric_limits<TYPE>::min() );		
		retImage->internMinMax.second = isis::util::Value<TYPE>( std::numeric_limits<TYPE>::max() );

		return retImage;
	}
};
	
	
}}}





#endif