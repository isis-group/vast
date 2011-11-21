#ifndef CREATEMASKDIALOG_HPP
#define CREATEMASKDIALOG_HPP


#include "ui_createMask.h"
#include "qviewercore.hpp"
#include <boost/assign.hpp>
#include "MaskEdit.hpp"


namespace isis {
namespace viewer {
namespace plugin {
	
class MaskEditDialog;	
	
class CreateMaskDialog : public QDialog
{
	Q_OBJECT
	
public:
	CreateMaskDialog( QWidget *parent, MaskEditDialog *maskEditDialog );	
	
public Q_SLOTS:
	void createMask();
	void showEvent( QShowEvent *);
	
private:
	MaskEditDialog *m_MaskEditDialog;
	Ui::createMaskDialog m_Interface;
	
	
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
		if( mask.hasProperty("Vista/ca") ) {
			std::list<double> ca = util::stringToList<double>(mask.getPropertyAs<std::string>("Vista/ca"));
			std::list<double>::const_iterator iter = ca.end();
			std::list<double> newCa = boost::assign::list_of<double>( *--iter / voxelSize[0])( *--iter / voxelSize[1])( *--iter / voxelSize[2] );
			mask.setPropertyAs<std::string>("Vista/ca", util::listToString<std::list<double>::iterator>(newCa.begin(), newCa.end(), " ", "", "") );
		}
		if( mask.hasProperty("Vista/cp") ) {
			std::list<double> cp = util::stringToList<double>(mask.getPropertyAs<std::string>("Vista/cp"));
			std::list<double>::const_iterator iter = cp.end();
			std::list<double> newCp = boost::assign::list_of<double>( *--iter / voxelSize[0])( *--iter / voxelSize[1])( *--iter / voxelSize[2] );
			mask.setPropertyAs<std::string>("Vista/cp", util::listToString<std::list<double>::iterator>(newCp.begin(), newCp.end(), " ", "", "") );
		}
		
		if( m_Interface.maskName->text().size() ) {
			mask.setPropertyAs<std::string>("source", m_Interface.maskName->text().toStdString() );
		} else {
			mask.setPropertyAs<std::string>("source", "mask" );
		}
		retImage = m_MaskEditDialog->m_ViewerCore->addImage( mask, ImageHolder::anatomical_image );
		retImage->minMax.first = isis::util::Value<TYPE>( std::numeric_limits<TYPE>::min() );
		retImage->minMax.second = isis::util::Value<TYPE>( std::numeric_limits<TYPE>::max() );
		retImage->internMinMax.first = isis::util::Value<TYPE>( std::numeric_limits<TYPE>::min() );		
		retImage->internMinMax.second = isis::util::Value<TYPE>( std::numeric_limits<TYPE>::max() );

		return retImage;
	}	
	
};
	
	
}}}



#endif