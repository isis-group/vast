#ifndef QVIEWERCORE_HPP
#define QVIEWERCORE_HPP


#include "viewercorebase.hpp"
#include "widgetImplementationBase.hpp"
#include <QtGui>
#include "color.hpp"
#include "uicore.hpp"

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

	virtual std::list<boost::shared_ptr<ImageHolder> > addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType);
	virtual void setImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType);


	const QSettings *getSettings() const { return m_Settings; }
	QSettings *getSettings() { return m_Settings; }

	//  bool widgetsAreIntitialized() const;

// 	std::vector< util::fvector4 > getRGBColorGradient() const { return m_RGBColorGradient; }

	//plugin stuff

	void addPlugin( boost::shared_ptr< plugin::PluginInterface > plugin );
	void addPlugins( plugin::PluginLoader::PluginListType plugins );
	PluginListType getPlugins() const { return m_PluginList; }

	virtual bool attachImageToWidget( boost::shared_ptr<ImageHolder> image, QWidgetImplementationBase * widget );
	
	void setParentWidget( QWidget *parent ) { m_Parent = parent; }
	
	const std::string &getCurrentPath() const  { return m_CurrentPath; }
	void setCurrentPath( const std::string &path) { m_CurrentPath = path; }
	
	const UICore *getUI() const { return m_UI; }
	UICore *getUI() { return m_UI; }

public Q_SLOTS:
	virtual void settingsChanged();
	virtual void zoomChanged( float zoomFactor );
	virtual void voxelCoordsChanged( util::ivector4 );
	virtual void physicalCoordsChanged( util::fvector4 );
	virtual void timestepChanged( int );
	virtual void setShowLabels( bool );
	virtual void updateScene( bool center = false );
	virtual void setAutomaticScaling( bool );
	virtual bool callPlugin( QString name );
	

Q_SIGNALS:
	void emitZoomChanged( float zoom );
	void emitVoxelCoordChanged( util::ivector4 );
	void emitPhysicalCoordsChanged( util::fvector4 );
	void emitTimeStepChange( unsigned int );
	void emitImagesChanged( DataContainer );
	void emitShowLabels( bool );
	void emitUpdateScene( bool center );

private:

// 	std::vector< util::fvector4 > m_RGBColorGradient;
	QSettings *m_Settings;

	QWidget *m_Parent;
	PluginListType m_PluginList;
	std::string m_CurrentPath;
	UICore *m_UI;
	
};


}
} // end namespace






#endif
