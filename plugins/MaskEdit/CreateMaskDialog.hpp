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
 * CreateMaskDialog.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef CREATEMASKDIALOG_HPP
#define CREATEMASKDIALOG_HPP


#include "ui_createMask.h"
#include "qviewercore.hpp"
#include <boost/assign.hpp>
#include "MaskEdit.hpp"


namespace isis
{
namespace viewer
{
namespace plugin
{

class MaskEditDialog;

class CreateMaskDialog : public QDialog
{
	Q_OBJECT

public:
	CreateMaskDialog( QWidget *parent, MaskEditDialog *maskEditDialog );

public Q_SLOTS:
	void createMask();
	void showEvent( QShowEvent * );

private:
	MaskEditDialog *m_MaskEditDialog;
	Ui::createMaskDialog m_Interface;


	template<typename TYPE>
	boost::shared_ptr<ImageHolder> _createEmptyMask( boost::shared_ptr<ImageHolder> refImage ) {
		const util::fvector3 &refVoxelSize = refImage->getISISImage()->getPropertyAs<util::fvector3>( "voxelSize" ) +
											 ( refImage->getISISImage()->hasProperty( "voxelGap" ) ? refImage->getISISImage()->getPropertyAs<util::fvector3>( "voxelGap" ) : util::fvector3() );
		boost::shared_ptr< ImageHolder > retImage;
		util::ivector4 size = refImage->getImageSize();
		size[0] /= ( m_Interface.xRes->value() / refVoxelSize[0] );
		size[1] /= ( m_Interface.yRes->value() / refVoxelSize[1] );
		size[2] /= ( m_Interface.zRes->value() / refVoxelSize[2] );
		isis::data::MemChunk<TYPE> ch( size[0], size[1], size[2], size[3] );
		ch.join( static_cast<isis::util::PropertyMap &>( *refImage->getISISImage() ) );
		isis::data::Image mask ( ch );
		const util::fvector3 voxelSize = util::fvector3( m_Interface.xRes->value(), m_Interface.yRes->value(), m_Interface.zRes->value() );
		mask.setPropertyAs<util::fvector3>( "voxelSize", voxelSize );
		mask.updateOrientationMatrices();

		if( mask.hasProperty( "Vista/ca" ) ) {
			std::list<double> ca = util::stringToList<double>( mask.getPropertyAs<std::string>( "Vista/ca" ) );
			std::list<double>::const_iterator iter = ca.end();
			std::list<double> newCa = boost::assign::list_of<double>( *--iter / voxelSize[0] )( *--iter / voxelSize[1] )( *--iter / voxelSize[2] );
			mask.setPropertyAs<std::string>( "Vista/ca", util::listToString<std::list<double>::iterator>( newCa.begin(), newCa.end(), " ", "", "" ) );
		}

		if( mask.hasProperty( "Vista/cp" ) ) {
			std::list<double> cp = util::stringToList<double>( mask.getPropertyAs<std::string>( "Vista/cp" ) );
			std::list<double>::const_iterator iter = cp.end();
			std::list<double> newCp = boost::assign::list_of<double>( *--iter / voxelSize[0] )( *--iter / voxelSize[1] )( *--iter / voxelSize[2] );
			mask.setPropertyAs<std::string>( "Vista/cp", util::listToString<std::list<double>::iterator>( newCp.begin(), newCp.end(), " ", "", "" ) );
		}

		mask.setPropertyAs<util::fvector3>( "indexOrigin", refImage->getISISImage()->getPropertyAs<util::fvector3>( "indexOrigin" ) );

		if( m_Interface.maskName->text().size() ) {
			mask.setPropertyAs<std::string>( "source", m_Interface.maskName->text().toStdString() );
		} else {
			mask.setPropertyAs<std::string>( "source", "mask" );
		}

		retImage = m_MaskEditDialog->m_ViewerCore->addImage( mask, ImageHolder::structural_image );
		retImage->getImageProperties().minMax.first = isis::util::Value<TYPE>( std::numeric_limits<TYPE>::min() );
		retImage->getImageProperties().minMax.second = isis::util::Value<TYPE>( std::numeric_limits<TYPE>::max() );
		retImage->getImageProperties().scalingMinMax.first = retImage->getImageProperties().minMax.first->as<double>();
		retImage->getImageProperties().scalingMinMax.second = retImage->getImageProperties().minMax.second->as<double>();

		return retImage;
	}

};


}
}
}



#endif