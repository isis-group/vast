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
 * qviewercore.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef QVIEWERCORE_HPP
#define QVIEWERCORE_HPP


#include "viewercorebase.hpp"
#include "widgetinterface.hpp"
#include <QtGui>
#include "qprogressfeedback.hpp"
#include <Adapter/qtapplication.hpp>


namespace isis
{
namespace viewer
{
class UICore;

class QViewerCore : public QObject, public ViewerCoreBase
{
	Q_OBJECT
public:

	QViewerCore( const std::string &appName = std::string(), const std::string &orgName = std::string(), QWidget *parent = 0 );

	virtual std::list<boost::shared_ptr<ImageHolder> > addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );
	virtual void setImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType );


	const QSettings *getSettings() const { return m_Settings; }
	QSettings *getSettings() { return m_Settings; }


	void addPlugin( boost::shared_ptr< plugin::PluginInterface > plugin );
	void addPlugins( plugin::PluginLoader::PluginListType plugins );
	PluginListType getPlugins() const { return m_PluginList; }

	virtual bool attachImageToWidget( boost::shared_ptr<ImageHolder> image, WidgetInterface *widget );

	void setParentWidget( QWidget *parent ) { m_Parent = parent; }

	const std::string &getCurrentPath() const  { return m_CurrentPath; }
	void setCurrentPath( const std::string &path ) { m_CurrentPath = path; }

	const UICore *getUICore() const { return m_UI; }
	UICore *getUICore() { return m_UI; }

	const boost::shared_ptr< QProgressFeedback > getProgressFeedback() const { return m_ProgressFeedback; }

	void addMessageHandler( qt4::QDefaultMessagePrint * );

	std::list< qt4::QMessage> getMessageLog() const { return m_MessageLog; }


public Q_SLOTS:
	virtual void settingsChanged();
	virtual void zoomChanged( float zoomFactor );
	virtual void physicalCoordsChanged( util::fvector4 );
	virtual void timestepChanged( int );
	virtual void setShowLabels( bool );
	virtual void setShowCrosshair( bool );
	virtual void updateScene( );
	virtual bool callPlugin( QString name );
	virtual void receiveMessage( qt4::QMessage message );
	virtual void receiveMessage( std::string message );
	virtual void openPath( QStringList fileList, ImageHolder::ImageType imageType, const std::string &rdialect = "", const std::string &rf = "", bool newWidget = false );
	virtual void centerImages( bool ca = false );
	virtual void closeImage( boost::shared_ptr<ImageHolder> image, bool refreshUI = true );
	virtual void saveSettings();
	virtual void loadSettings();

Q_SIGNALS:
	void emitZoomChanged( float zoom );
	void emitVoxelCoordChanged( util::ivector4 );
	void emitPhysicalCoordsChanged( util::fvector4 );
	void emitTimeStepChange( unsigned int );
	void emitImagesChanged( DataContainer );
	void emitShowLabels( bool );
	void emitUpdateScene( );
	void emitSetEnableCrosshair( bool enable );
	void emitStatus( QString );

private:

	QSettings *m_Settings;
	std::list< qt4::QMessage > m_MessageLog;

	QWidget *m_Parent;
	PluginListType m_PluginList;
	std::string m_CurrentPath;
	boost::shared_ptr< QProgressFeedback > m_ProgressFeedback;
	UICore *m_UI;

};


}
} // end namespace



#include "uicore.hpp"


#endif
