#include "sliderwidget.hpp"
#include <boost/concept_check.hpp>
#include "imageholder.hpp"
#include "qviewercore.hpp"


namespace isis
{
namespace viewer
{
namespace widget
{

SliderWidget::SliderWidget( QWidget *parent, isis::viewer::QViewerCore *core )
	: QWidget( parent ),
	  m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	m_Interface.opacitySlider->setToolTip( "Opacity" );
	m_Interface.maxSlider->setToolTip( "Upper threshold" );
	m_Interface.minSlider->setToolTip( "Lower threshold" );
	m_Interface.opacitySlider->setMinimum( 0 );
	m_Interface.opacitySlider->setMaximum( 1000 );
	m_Interface.opacitySlider->setSliderPosition( 1000 );
	m_Interface.minSlider->setMinimum( 0 );
	m_Interface.minSlider->setMaximum( 1000 );
	m_Interface.minSlider->setSliderPosition( 1000 );
	m_Interface.maxSlider->setMinimum( 0 );
	m_Interface.maxSlider->setMaximum( 1000 );
	m_Interface.maxSlider->setSliderPosition( 1000 );

	connect( m_Interface.opacitySlider, SIGNAL( sliderMoved( int ) ), this, SLOT( opacityChanged( int ) ) );
	connect( m_Interface.maxSlider, SIGNAL( sliderMoved( int ) ), this, SLOT( upperThresholdChanged( int ) ) );
	connect( m_Interface.minSlider, SIGNAL( sliderMoved( int ) ), this, SLOT( lowerThresholdChanged( int ) ) );
	m_Interface.checkGlobal->setChecked(true);

}


void SliderWidget::setVisible( SliderWidget::SliderType slider , bool visible )
{
	switch( slider ) {
	case Opacity:
		m_Interface.opacitySlider->setVisible( visible );
		break;
	case UpperThreshold:
		m_Interface.maxSlider->setVisible( visible );
		break;
	case LowerThreshold:
		m_Interface.minSlider->setVisible( visible );
		break;
	}
}

double SliderWidget::norm( const double &min, const double &max, const int &pos )
{
	const double range = fabs( min ) + fabs( max );
	return ( range / 1000.0 ) * pos;

}

void SliderWidget::opacityChanged( int sliderPos )
{
	if( !m_Interface.checkGlobal->isChecked() ) {
		m_ViewerCore->getCurrentImage()->opacity = norm( 0.0, 1.0, sliderPos )  ;
	} else {
		BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) 
		{
			if( image.second->imageType == ImageHolder::z_map ) {
				image.second->opacity = norm( 0.0, 1.0, sliderPos )  ;
			}
		}		
	}
	m_ViewerCore->updateScene();
}

void SliderWidget::lowerThresholdChanged( int sliderPos )
{
	if ( !m_Interface.checkGlobal->isChecked() ) {
		m_ViewerCore->getCurrentImage()->lowerThreshold = norm( m_ViewerCore->getCurrentImage()->minMax.first->as<double>() , 0.0, 1000 - sliderPos ) * -1 ;
	} else {
		BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) 
		{
			if( image.second->imageType == ImageHolder::z_map ) {
				const double lT = norm( m_maxMin, 0.0, 1000 - sliderPos ) * -1;
				image.second->lowerThreshold = lT > image.second->minMax.first->as<double>() ? lT : image.second->minMax.first->as<double>();
			}
		}		
	}
	m_ViewerCore->updateScene();
}

void SliderWidget::upperThresholdChanged( int sliderPos )
{
	if( !m_Interface.checkGlobal->isChecked() ) {
		m_ViewerCore->getCurrentImage()->upperThreshold = norm( 0.0, m_ViewerCore->getCurrentImage()->minMax.second->as<double>(), 1000 - sliderPos ) ;
	} else {
		BOOST_FOREACH( DataContainer::reference image, m_ViewerCore->getDataContainer() ) 
		{
			if( image.second->imageType == ImageHolder::z_map ) {
				const double uT = norm( 0.0, m_maxMax, 1000 - sliderPos );
				image.second->upperThreshold = uT < image.second->minMax.second->as<double>() ? uT : image.second->minMax.second->as<double>() - std::numeric_limits<double>::round_error();
			}
		}
	}
	m_ViewerCore->updateScene();
}

void SliderWidget::synchronize()
{
	if( m_ViewerCore->getMode() == ViewerCoreBase::zmap ) {
		m_Interface.zmapModeFrame->setVisible(true);
		BOOST_FOREACH( DataContainer::const_reference image, m_ViewerCore->getDataContainer() ) 
		{
			if( image.second->imageType == ImageHolder::z_map ) {
				if( image.second->minMax.first->as<double>() < m_maxMin ) {
					m_maxMin = image.second->minMax.first->as<double>();
				}
				if( image.second->minMax.second->as<double>() > m_maxMax ) {
					m_maxMax = image.second->minMax.second->as<double>();
				}
			}
		}		
	} else {
		m_Interface.zmapModeFrame->setVisible( false );
		m_Interface.checkGlobal->setChecked(false);
	}	
	if( m_ViewerCore->hasImage() ) {
		if( m_ViewerCore->getCurrentImage()->imageType == ImageHolder::z_map ) {
			setVisible( LowerThreshold, true );
			setVisible( UpperThreshold, true );
			setVisible( Opacity, true );
		} else if ( m_ViewerCore->getCurrentImage()->imageType == ImageHolder::anatomical_image ) {
			setVisible( LowerThreshold, false );
			setVisible( UpperThreshold, false );
			setVisible( Opacity, true );
		}

		const unsigned short lowerThreshold = 1000 - abs( ( 1000 / m_ViewerCore->getCurrentImage()->minMax.first->as<double>() )
											  * m_ViewerCore->getCurrentImage()->lowerThreshold );

		const unsigned short upperThreshold = 1000 - abs( ( 1000 / m_ViewerCore->getCurrentImage()->minMax.second->as<double>() )
											  * m_ViewerCore->getCurrentImage()->upperThreshold );

		m_Interface.opacitySlider->setSliderPosition( m_ViewerCore->getCurrentImage()->opacity * 1000 );

		m_Interface.minSlider->setSliderPosition( lowerThreshold );

		m_Interface.maxSlider->setSliderPosition( upperThreshold );
	}
}



}
}
}
