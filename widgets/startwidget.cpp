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
 * startwidget.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "startwidget.hpp"
#include "../viewer/uicore.hpp"
#include "../viewer/qviewercore.hpp"
#include "../widgets/filedialog.hpp"
#include "../viewer/fileinformation.hpp"

namespace isis
{
namespace viewer
{
namespace ui
{


StartWidget::StartWidget( QWidget *parent, QViewerCore *core )
	: QDialog( parent ),
	  m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	connect( m_Interface.openImageButton, SIGNAL( clicked() ), this, SLOT( openImageButtonClicked() ) );
	connect( m_Interface.showMeCheck, SIGNAL( clicked( bool ) ), this, SLOT( showMeChecked( bool ) ) );
	connect( m_Interface.favList, SIGNAL( doubleClicked( QModelIndex ) ), this, SLOT( openFavPath() ) );
	connect( m_Interface.recentList, SIGNAL( doubleClicked( QModelIndex ) ), this, SLOT( openRecentPath() ) );

}

void StartWidget::showMeChecked( bool checked )
{
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "showStartWidget", checked );
}

void StartWidget::keyPressEvent( QKeyEvent *e )
{
	if( e->key() == Qt::Key_Escape ) {
		close();
	}
}


void StartWidget::closeEvent( QCloseEvent * )
{
	m_ViewerCore->getUICore()->getMainWindow()->setEnabled( true );
}


void StartWidget::openFavPath()
{
	close();
	m_ViewerCore->openFile( m_ViewerCore->getSettings()->getFavoriteFiles().at( m_Interface.favList->currentItem()->text().toStdString() ) );
}

void StartWidget::openRecentPath()
{
	close();
	m_ViewerCore->openFile( m_ViewerCore->getSettings()->getRecentFiles().at( m_Interface.recentList->currentItem()->text().toStdString() ) );
}

void StartWidget::showEvent( QShowEvent * )
{
	uint16_t width = m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "startWidgetWidth" );
	const QRect screen = QApplication::desktop()->screenGeometry();

	const float scale = 0.9;
	QPixmap pixMap( m_ViewerCore->getSettings()->getPropertyAs<std::string>( "vastSymbol" ).c_str() );
	float ratio = pixMap.height() / ( float )pixMap.width() * scale;
	m_Interface.imageLabel->setMinimumHeight( width * ratio );
	m_Interface.imageLabel->setMaximumHeight( width * ratio );
	m_Interface.imageLabel->setPixmap( pixMap.scaled( width, width * ratio ) );
	m_Interface.imageLabel->setAlignment( Qt::AlignCenter );
	move( m_ViewerCore->getUICore()->getMainWindow()->rect().center().x() - this->width() / 2,  m_ViewerCore->getUICore()->getMainWindow()->rect().center().y() - this->height() / 2 );


	m_ViewerCore->getUICore()->getMainWindow()->setEnabled( false );
	setEnabled( true );
	m_Interface.buttonFrame->setVisible( true );
	m_Interface.showMeCheck->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "showStartWidget" ) );

	m_Interface.favoritesLabel->setVisible( fillList( m_ViewerCore->getSettings()->getFavoriteFiles(), m_Interface.favList ) );
	m_Interface.recentLabel->setVisible( fillList( m_ViewerCore->getSettings()->getRecentFiles(), m_Interface.recentList ) );
	adjustSize();
}

bool StartWidget::fillList ( const FileInformationMap &fileInfoList, QListWidget *list )
{
	if( !fileInfoList.empty() ) {
		list->clear();
		list->setVisible( true );
		BOOST_FOREACH( FileInformationMap::const_reference fileInfo, fileInfoList ) {
			unsigned short validFiles;
			QListWidgetItem *item = new QListWidgetItem( fileInfo.first.c_str() );

			if( FileDialog::checkIfPathIsValid( fileInfo.second.getCompletePath().c_str(), validFiles, "" ) ) {
				item->setTextColor( QColor( 34, 139, 34 ) );
			} else {
				item->setTextColor( Qt::red );
			}

			list->addItem( item );
		}
		return true;
	} else {
		list->setVisible( false );
		return false;
	}

	return false; // supress warning
}



void StartWidget::openImageButtonClicked()
{
	close();
	m_ViewerCore->getUICore()->getMainWindow()->openImage();
}



}
}
}
