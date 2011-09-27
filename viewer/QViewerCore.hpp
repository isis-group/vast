#ifndef QVIEWERCORE_HPP
#define QVIEWERCORE_HPP


#include "ViewerCoreBase.hpp"
#include "QWidgetImplementationBase.hpp"
#include <QtGui>
#include "Color.hpp"

namespace isis
{
namespace viewer
{
class QViewerCore : public QObject, public ViewerCoreBase
{
	Q_OBJECT
public:
	enum Actions {not_specified, timestep_changed, show_labels};
	typedef std::map<std::string, QWidgetImplementationBase * > WidgetMap;
	QViewerCore( const std::string &appName = std::string(), const std::string &orgName = std::string() );


	bool registerWidget( std::string key, QWidgetImplementationBase *widget, Actions = not_specified );

	virtual void addImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType, bool passToWidgets );
	virtual void setImageList( const std::list< data::Image > imageList, const ImageHolder::ImageType &imageType, bool passToWidgets );

	const WidgetMap &getWidgets() const { return m_WidgetMap; }
	WidgetMap &getWidgets() { return m_WidgetMap; }
	const QSettings *getSettings() const { return m_Settings; }
	QSettings *getSettings() { return m_Settings; }

	//  bool widgetsAreIntitialized() const;

	std::vector< util::fvector4 > getRGBColorGradient() const { return m_RGBColorGradient; }

	template<typename T>
	T *getWidgetAs( std::string key ) {
		if( m_WidgetMap.find( key ) == m_WidgetMap.end() ) {
			LOG( Runtime, error ) << "A widget with the name " << key << " is not registered!";
			return 0;
		}

		if( dynamic_cast<T *>( m_WidgetMap[key] ) != 0 ) {
			return dynamic_cast<T *>( m_WidgetMap[key] );
		} else {
			LOG( Runtime, error ) << "Error while converting widget " << key << " !";
		}
	};


public Q_SLOTS:
	virtual void zoomChanged( float zoomFactor );
	virtual void voxelCoordsChanged( util::ivector4 );
	virtual void physicalCoordsChanged( util::fvector4 );
	virtual void timestepChanged( int );
	virtual void setShowLabels( bool );
	virtual void updateScene( bool center = false );
	virtual void setAutomaticScaling( bool );

Q_SIGNALS:
	void emitZoomChanged( float zoom );
	void emitVoxelCoordChanged( util::ivector4 );
	void emitPhysicalCoordsChanged( util::fvector4 );
	void emitTimeStepChange( unsigned int );
	void emitImagesChanged( DataContainer );
	void emitShowLabels( bool );
	void emitUpdateScene( bool center );

private:
	//this map holds the widgets associated with a given name
	WidgetMap m_WidgetMap;
	std::vector< util::fvector4 > m_RGBColorGradient;
	QSettings *m_Settings;

};


}
} // end namespace






#endif
