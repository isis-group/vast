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
 * PropertyToolDialog.cpp
 *
 * Description:
 *
 *  Created on: Jan 05, 2012
 *      Author: tuerke
 ******************************************************************/

#include "PropertyToolDialog.hpp"

namespace isis {
namespace viewer {
namespace plugin {
 

PropertyToolDialog::PropertyToolDialog(QWidget* parent, QViewerCore* core)
    : QDialog( parent ),
    m_ViewerCore( core )
{
    m_Interface.setupUi( this );
    connect( m_ViewerCore, SIGNAL( emitUpdateScene()), this, SLOT( updateProperties()));
}

void PropertyToolDialog::showEvent(QShowEvent* )
{
    updateProperties();
}

void PropertyToolDialog::setIfHas(const std::string& name, QLabel *nameLabel, QLabel *propLabel, const boost::shared_ptr<data::Image> image )
{
    if( image->hasProperty( name.c_str() ) ) {
        nameLabel->setVisible(true);
        propLabel->setVisible(true);
        propLabel->setText( image->propertyValue(name.c_str()).toString().c_str());
    } else {
        nameLabel->setVisible(false);
        propLabel->setVisible(false);
    }
}



void PropertyToolDialog::updateProperties()
{
    if( m_ViewerCore->hasImage() && isVisible() ) {
        boost::shared_ptr<data::Image> isisImage = m_ViewerCore->getCurrentImage()->getISISImage();
        //orientation
        const util::fvector4 rowVec = isisImage->getPropertyAs<util::fvector4>("rowVec");
        const util::fvector4 columnVec = isisImage->getPropertyAs<util::fvector4>("columnVec");
        const util::fvector4 sliceVec = isisImage->getPropertyAs<util::fvector4>("sliceVec");
        const util::fvector4 indexOrigin = isisImage->getPropertyAs<util::fvector4>("indexOrigin");
        m_Interface.rowVec0->setText( QString::number(rowVec[0]));
        m_Interface.rowVec1->setText( QString::number(rowVec[1]));
        m_Interface.rowVec2->setText( QString::number(rowVec[2]));
        m_Interface.columnVec0->setText( QString::number(columnVec[0] ) );
        m_Interface.columnVec1->setText( QString::number(columnVec[1] ) );
        m_Interface.columnVec2->setText( QString::number(columnVec[2] ) );
        m_Interface.sliceVec0->setText( QString::number(sliceVec[0]));
        m_Interface.sliceVec1->setText( QString::number(sliceVec[1]));
        m_Interface.sliceVec2->setText( QString::number(sliceVec[2]));
        m_Interface.indexOrigin0->setText( QString::number(indexOrigin[0]));
        m_Interface.indexOrigin1->setText( QString::number(indexOrigin[1]));
        m_Interface.indexOrigin2->setText( QString::number(indexOrigin[2]));
        
        m_Interface.fileName->setText( m_ViewerCore->getCurrentImage()->getFileNames().front().c_str() );
        m_Interface.dataType->setText( m_ViewerCore->getCurrentImage()->majorTypeName.c_str() );
        setIfHas( std::string("sequenceStart"), m_Interface.L_sequenceStart, m_Interface.sequenceStart, isisImage );
        setIfHas( std::string("sequenceNumber"), m_Interface.L_sequenceNumber, m_Interface.sequenceNumber, isisImage );
        setIfHas( std::string("sequenceDescription"), m_Interface.L_sequenceDescription, m_Interface.sequenceDescription, isisImage );
        setIfHas( std::string("subjectName"), m_Interface.L_subjectName, m_Interface.subjectName, isisImage );
        setIfHas( std::string("subjectAge"), m_Interface.L_subjectAge, m_Interface.subjectAge, isisImage );
        setIfHas( std::string("subjectBirth"), m_Interface.L_subjectBirth, m_Interface.subjectBirth, isisImage );
        setIfHas( std::string("subjectGender"), m_Interface.L_subjectGender, m_Interface.subjectGender, isisImage );
        setIfHas( std::string("subjectWeigth"), m_Interface.L_subjectWeight, m_Interface.subjectWeight, isisImage );
        setIfHas( std::string("voxelSize"), m_Interface.L_voxelSize, m_Interface.voxelSize, isisImage );
        setIfHas( std::string("voxelGap"), m_Interface.L_voxelGap, m_Interface.voxelGap, isisImage );
        setIfHas( std::string("transmitCoil"), m_Interface.L_transmitCoil, m_Interface.transmitCoil, isisImage );        
        setIfHas( std::string("flipAngle"), m_Interface.L_flipAngle, m_Interface.flipAngle, isisImage );   
        setIfHas( std::string("performingPhysician"), m_Interface.L_performingPhysician, m_Interface.performingPhysician, isisImage );   
        setIfHas( std::string("numberOfAverages"), m_Interface.L_numberOfAverages, m_Interface.numberOfAverages, isisImage );           
        setIfHas( std::string("caPos"), m_Interface.L_positionCA, m_Interface.positionCA, isisImage );           
        setIfHas( std::string("cpPos"), m_Interface.L_positionCP, m_Interface.positionCP, isisImage );           
        setIfHas( std::string("repetitionTime"), m_Interface.L_repetitionTime, m_Interface.repetitionTime, isisImage );           
        setIfHas( std::string("numberOfAverages"), m_Interface.L_numberOfAverages, m_Interface.numberOfAverages, isisImage );  
        setIfHas( std::string("echoTime"), m_Interface.L_echoTime, m_Interface.echoTime, isisImage );  
        m_Interface.subjectGroup->setVisible(isisImage->hasProperty("subjectName") || isisImage->hasProperty("subjectAge")
            || isisImage->hasProperty("subjectBirth") || isisImage->hasProperty("subjectGender")
            || isisImage->hasProperty("subjectWeigth"));
    }
    adjustSize();
}


 

}}}