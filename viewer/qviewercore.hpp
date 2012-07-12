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
 * qviewercore.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef VAST_QVIEWERCORE_HPP
#define VAST_QVIEWERCORE_HPP


#include "viewercorebase.hpp"
#include "qprogressfeedback.hpp"

#include <Adapter/qtapplication.hpp>

#include <QtGui>

namespace isis
{
namespace viewer
{
class UICore;

class QViewerCore : public QObject, public ViewerCoreBase
{
	Q_OBJECT
public:

	QViewerCore();

	virtual ImageHolder::Vector addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );

	void addPlugin( boost::shared_ptr< plugin::PluginInterface > plugin );
	void addPlugins( plugin::PluginLoader::PluginListType plugins );
	plugin::PluginLoader::PluginListType getPlugins() const { return m_PluginList; }

	void setParentWidget( QWidget *parent ) { m_Parent = parent; }

	const std::string &getCurrentPath() const  { return m_CurrentPath; }
	void setCurrentPath( const std::string &path ) { m_CurrentPath = path; }

	const UICore *getUICore() const { return m_UI; }
	UICore *getUICore() { return m_UI; }

	const boost::shared_ptr< QProgressFeedback > getProgressFeedback() const { return m_ProgressFeedback; }

	void addMessageHandler( qt4::QDefaultMessagePrint * );
	void addMessageHandlerDev( qt4::QDefaultMessagePrint * );

	std::list< qt4::QMessage> getMessageLog() const { return m_MessageLog; }
	std::list< qt4::QMessage> getMessageLogDev() const { return m_DevMessageLog; }

public Q_SLOTS:
	virtual void settingsChanged();
	virtual void close( );
	virtual void zoomChanged( float zoomFactor );
	virtual void physicalCoordsChanged( util::fvector3 );
	virtual void onWidgetClicked( widget::WidgetInterface *, util::fvector3, Qt::MouseButton );
	virtual void onWidgetMoved( widget::WidgetInterface *, util::fvector3, Qt::MouseButton );
	virtual void timestepChanged( int );
	virtual void setShowLabels( bool );
	virtual void setShowCrosshair( bool );
	virtual void updateScene( );
	virtual bool callPlugin( QString name );
	virtual void receiveMessage( qt4::QMessage  );
	virtual void receiveMessage( std::string  );
	virtual void receiveMessageDev( qt4::QMessage );
	virtual ImageHolder::Vector openFile( const FileInformation &fileInfo, bool show = true );
	virtual void openFileList( const std::list< FileInformation > fileInfoList );
	virtual void centerImages( bool ca = false );
	virtual void closeImage( boost::shared_ptr<ImageHolder> image, bool refreshUI = true );

Q_SIGNALS:
	void emitZoomChanged( float zoom );
	void emitVoxelCoordChanged( util::ivector4 );
	void emitOnWidgetClicked( util::fvector3, Qt::MouseButton );
	void emitOnWidgetMoved( util::fvector3, Qt::MouseButton );
	void emitPhysicalCoordsChanged( util::fvector3 );
	void emitTimeStepChange( unsigned int );
	void emitShowLabels( bool );
	void emitUpdateScene( );
	void emitSetEnableCrosshair( bool enable );

private:

	void checkForErrors();

	std::list< qt4::QMessage > m_MessageLog;
	std::list< qt4::QMessage > m_DevMessageLog;

	QWidget *m_Parent;
	plugin::PluginLoader::PluginListType m_PluginList;
	std::string m_CurrentPath;
	boost::shared_ptr< QProgressFeedback > m_ProgressFeedback;
	UICore *m_UI;

	std::list<FileInformation> m_OpenFileList;


};


}
} // end namespace



#include "uicore.hpp"


#endif
