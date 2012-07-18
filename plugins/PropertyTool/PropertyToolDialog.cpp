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
 * PropertyToolDialog.cpp
 *
 * Description:
 *
 *  Created on: Jan 05, 2012
 *      Author: tuerke
 ******************************************************************/

#include "PropertyToolDialog.hpp"

namespace isis
{
namespace viewer
{
namespace plugin
{


PropertyToolDialog::PropertyToolDialog( QWidget *parent, QViewerCore *core )
	: QDialog( parent ),
	  m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	m_Interface.tabWidget->setCurrentIndex( 0 );
	setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Maximum );

	connect( m_Interface.selection, SIGNAL( currentIndexChanged( int ) ), this, SLOT( selectionChanged( int ) ) );
	connect( m_Interface.propertyTree, SIGNAL( itemSelectionChanged() ) , this, SLOT( onPropertyTreeClicked() ) );
	connect( m_Interface.propertyTree, SIGNAL( itemClicked( QTreeWidgetItem *, int ) ), SLOT( onPropertyTreeClicked() ) );
	connect( m_Interface.editButton, SIGNAL( clicked( bool ) ), this, SLOT( editRequested() ) );
	connect( m_Interface.propertyTree, SIGNAL( doubleClicked( QModelIndex ) ), this, SLOT( editRequested() ) );
	m_fillChunkListThread = new _internal::FillChunkListThread( this, &m_Interface );
}

void PropertyToolDialog::showEvent( QShowEvent *e )
{
	updateProperties();
	connect( m_ViewerCore, SIGNAL( emitUpdateScene() ), this, SLOT( updateProperties() ) );
	m_ViewerCore->emitImageContentChanged.connect( boost::bind( &PropertyToolDialog::updateProperties, this ) );
	QDialog::showEvent( e );
}

void PropertyToolDialog::closeEvent ( QCloseEvent *e )
{
	disconnect( m_ViewerCore, SIGNAL( emitUpdateScene() ), this, SLOT( updateProperties() ) );
	m_ViewerCore->emitImageContentChanged.disconnect( boost::bind( &PropertyToolDialog::updateProperties, this ) );
	QDialog::closeEvent( e );

}


void PropertyToolDialog::setIfHas( const std::string &name, QLabel *nameLabel, QLabel *propLabel, const boost::shared_ptr<data::Image> image )
{
	if( image->hasProperty( name.c_str() ) ) {
		nameLabel->setVisible( true );
		propLabel->setVisible( true );
		propLabel->setText( image->propertyValue( name.c_str() ).toString().c_str() );
	} else {
		nameLabel->setVisible( false );
		propLabel->setVisible( false );
	}
}

void PropertyToolDialog::updateProperties()
{
	if( m_ViewerCore->hasImage() && isVisible() ) {

		const boost::shared_ptr<data::Image> isisImage = m_ViewerCore->getCurrentImage()->getISISImage();
		//orientation
		const util::fvector3 rowVec = isisImage->getPropertyAs<util::fvector3>( "rowVec" );
		const util::fvector3 columnVec = isisImage->getPropertyAs<util::fvector3>( "columnVec" );
		const util::fvector3 sliceVec = isisImage->getPropertyAs<util::fvector3>( "sliceVec" );
		const util::fvector3 indexOrigin = isisImage->getPropertyAs<util::fvector3>( "indexOrigin" );
		m_Interface.rowVec0->setText( QString::number( rowVec[0] ) );
		m_Interface.rowVec1->setText( QString::number( rowVec[1] ) );
		m_Interface.rowVec2->setText( QString::number( rowVec[2] ) );
		m_Interface.columnVec0->setText( QString::number( columnVec[0] ) );
		m_Interface.columnVec1->setText( QString::number( columnVec[1] ) );
		m_Interface.columnVec2->setText( QString::number( columnVec[2] ) );
		m_Interface.sliceVec0->setText( QString::number( sliceVec[0] ) );
		m_Interface.sliceVec1->setText( QString::number( sliceVec[1] ) );
		m_Interface.sliceVec2->setText( QString::number( sliceVec[2] ) );
		m_Interface.indexOrigin0->setText( QString::number( indexOrigin[0] ) );
		m_Interface.indexOrigin1->setText( QString::number( indexOrigin[1] ) );
		m_Interface.indexOrigin2->setText( QString::number( indexOrigin[2] ) );


		m_Interface.dataType->setText( m_ViewerCore->getCurrentImage()->getImageProperties().majorTypeName.c_str() );
		m_Interface.imageSize->setText( m_ViewerCore->getCurrentImage()->getISISImage()->getSizeAsString().c_str() );
		setIfHas( std::string( "sequenceStart" ), m_Interface.L_sequenceStart, m_Interface.sequenceStart, isisImage );
		setIfHas( std::string( "sequenceNumber" ), m_Interface.L_sequenceNumber, m_Interface.sequenceNumber, isisImage );
		setIfHas( std::string( "sequenceDescription" ), m_Interface.L_sequenceDescription, m_Interface.sequenceDescription, isisImage );
		setIfHas( std::string( "subjectName" ), m_Interface.L_subjectName, m_Interface.subjectName, isisImage );
		setIfHas( std::string( "subjectAge" ), m_Interface.L_subjectAge, m_Interface.subjectAge, isisImage );
		setIfHas( std::string( "subjectBirth" ), m_Interface.L_subjectBirth, m_Interface.subjectBirth, isisImage );
		setIfHas( std::string( "subjectGender" ), m_Interface.L_subjectGender, m_Interface.subjectGender, isisImage );
		setIfHas( std::string( "subjectWeigth" ), m_Interface.L_subjectWeight, m_Interface.subjectWeight, isisImage );
		setIfHas( std::string( "voxelSize" ), m_Interface.L_voxelSize, m_Interface.voxelSize, isisImage );
		setIfHas( std::string( "voxelGap" ), m_Interface.L_voxelGap, m_Interface.voxelGap, isisImage );
		setIfHas( std::string( "transmitCoil" ), m_Interface.L_transmitCoil, m_Interface.transmitCoil, isisImage );
		setIfHas( std::string( "flipAngle" ), m_Interface.L_flipAngle, m_Interface.flipAngle, isisImage );
		setIfHas( std::string( "performingPhysician" ), m_Interface.L_performingPhysician, m_Interface.performingPhysician, isisImage );
		setIfHas( std::string( "numberOfAverages" ), m_Interface.L_numberOfAverages, m_Interface.numberOfAverages, isisImage );
		setIfHas( std::string( "caPos" ), m_Interface.L_positionCA, m_Interface.positionCA, isisImage );
		setIfHas( std::string( "cpPos" ), m_Interface.L_positionCP, m_Interface.positionCP, isisImage );
		setIfHas( std::string( "repetitionTime" ), m_Interface.L_repetitionTime, m_Interface.repetitionTime, isisImage );
		setIfHas( std::string( "numberOfAverages" ), m_Interface.L_numberOfAverages, m_Interface.numberOfAverages, isisImage );
		setIfHas( std::string( "echoTime" ), m_Interface.L_echoTime, m_Interface.echoTime, isisImage );

		m_Interface.subjectGroup->setVisible( isisImage->hasProperty( "subjectName" ) || isisImage->hasProperty( "subjectAge" )
											  || isisImage->hasProperty( "subjectBirth" ) || isisImage->hasProperty( "subjectGender" )
											  || isisImage->hasProperty( "subjectWeigth" ) );
		m_Interface.selection->clear();
		m_Interface.selection->addItem( "Image" );

		//filling the list with all chunks can take a while so we create a thread for that
		m_fillChunkListThread->setISISImage( isisImage );
		m_fillChunkListThread->start();

		buildUpTree( static_cast<util::PropertyMap &>( *isisImage ) );
	}

	m_Interface.propertyName->clear();
	m_Interface.propertyValue->clear();
	m_Interface.propertyTree->resizeColumnToContents( 0 );
	adjustSize();
}

void PropertyToolDialog::selectionChanged( int select )
{
	if( m_ViewerCore->hasImage() ) {
		if( select > 0 ) {
			const std::vector<data::Chunk>& chunks = m_ViewerCore->getCurrentImage()->getChunkVector();

			if( select < static_cast<int>( chunks.size() ) ) {
				buildUpTree( chunks[select-1] );
			}
		}
	}
}


void PropertyToolDialog::buildUpTree( const util::PropertyMap &image )
{
	TreePropMap propMap = image;
	propMap.fillTreeWidget( m_Interface.propertyTree );

}

TreePropMap::TreePropMap ( const util::PropertyMap &propMap )
{
	static_cast<util::PropertyMap &>( *this ) = propMap;
}


void TreePropMap::walkTree( QTreeWidgetItem *item, const TreePropMap &propMap, bool topLevel )
{
	for ( const_iterator i = propMap.begin(); i != propMap.end(); i++ ) {
		QTreeWidgetItem *newItem;

		if( i->second.is_leaf() ) {
			if( topLevel ) {
				newItem = new QTreeWidgetItem( m_TreeWidget );
			} else {
				newItem = new QTreeWidgetItem( item );
			}

			newItem->setText( 0, i->first.c_str() );
			newItem->setText( 1, i->second.toString().c_str() );
		} else {
			item->setText( 0, i->first.c_str() );
			walkTree( item, i->second.getBranch(), false );
		}
	}
}


void TreePropMap::fillTreeWidget( QTreeWidget *treeWidget )
{
	m_TreeWidget = treeWidget;
	m_TreeWidget->clear();
	walkTree( new QTreeWidgetItem( m_TreeWidget ), *this, true );
}

QString PropertyToolDialog::getItemName ( QTreeWidgetItem *item )
{
	QString retName;
	retName.append( item->text( item->columnCount() > 1 ? item->columnCount() - 2 : 0 ) );

	if( item->parent() ) {
		retName.prepend( "/" );
		retName.prepend( getItemName( item->parent() ) );
	}

	return retName;
}


void PropertyToolDialog::onPropertyTreeClicked()
{
	if( m_ViewerCore->hasImage() ) {
		QTreeWidgetItem *item = m_Interface.propertyTree->currentItem();

		if( item->columnCount() > 1 ) {
			QString name = getItemName( item );
			m_Interface.propertyName->setText( name );
			m_Interface.propertyValue->setText( static_cast<util::PropertyMap &>( *m_ViewerCore->getCurrentImage()->getISISImage() ).propertyValue( name.toStdString().c_str() ).toString().c_str() );
		}
	}

}

void PropertyToolDialog::editRequested()
{
	const QString propName = m_Interface.propertyName->text();
	const QString propValue = m_Interface.propertyValue->text();

	if( !propName.isEmpty() ) {
		bool ok;
		QString text = QInputDialog::getText( this, tr( "Edit property" ),
											  propName, QLineEdit::Normal,
											  propValue, &ok );

		if( ok && !text.isEmpty() ) {
			const util::PropertyMap::PropPath propNameStr = propName.toStdString().c_str();
			util::PropertyMap &propMap = static_cast<util::PropertyMap &>( *m_ViewerCore->getCurrentImage()->getISISImage() );

			switch( propMap.propertyValue( propNameStr ).getTypeID() ) {
			case util::Value<std::string>::staticID:
				checkAndSet<std::string>( propMap, propNameStr, text );
				break;
			case util::Value<int8_t>::staticID:
				checkAndSet<int8_t>( propMap, propNameStr, text );
				break;
			case util::Value<uint8_t>::staticID:
				checkAndSet<uint8_t>( propMap, propNameStr, text );
				break;
			case util::Value<int16_t>::staticID:
				checkAndSet<int16_t>( propMap, propNameStr, text );
				break;
			case util::Value<uint16_t>::staticID:
				checkAndSet<uint16_t>( propMap, propNameStr, text );
				break;
			case util::Value<int32_t>::staticID:
				checkAndSet<int32_t>( propMap, propNameStr, text );
				break;
			case util::Value<uint32_t>::staticID:
				checkAndSet<uint32_t>( propMap, propNameStr, text );
				break;
			case util::Value<int64_t>::staticID:
				checkAndSet<int64_t>( propMap, propNameStr, text );
				break;
			case util::Value<uint64_t>::staticID:
				checkAndSet<uint64_t>( propMap, propNameStr, text );
				break;
			case util::Value<float>::staticID:
				checkAndSet<float>( propMap, propNameStr, text );
				break;
			case util::Value<double>::staticID:
				checkAndSet<double>( propMap, propNameStr, text );
				break;
			case util::Value<util::fvector3>::staticID:
				checkAndSet<util::fvector3>( propMap, propNameStr, text );
				break;
			case util::Value<util::fvector4>::staticID:
				checkAndSet<util::fvector4>( propMap, propNameStr, text );
				break;
			case util::Value<util::dvector3>::staticID:
				checkAndSet<util::dvector3>( propMap, propNameStr, text );
				break;	
			case util::Value<util::dvector4>::staticID:
				checkAndSet<util::dvector4>( propMap, propNameStr, text );
				break;
			case util::Value<util::ivector4>::staticID:
				checkAndSet<util::ivector4>( propMap, propNameStr, text );
				break;
			}

			updateProperties();
			m_ViewerCore->getCurrentImage()->updateOrientation();
			m_ViewerCore->updateScene();
			m_ViewerCore->getUICore()->refreshUI();
			m_ViewerCore->physicalCoordsChanged( m_ViewerCore->getCurrentImage()->getImageProperties().physicalCoords );
			std::stringstream ss;
			ss << propName.toStdString() << " changed to " << text.toStdString();
			m_ViewerCore->getCurrentImage()->addChangedAttribute( ss.str() );

		}


	}

}



}
}
}