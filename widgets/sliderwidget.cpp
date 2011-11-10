#include "sliderwidget.hpp"
#include <boost/concept_check.hpp>


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
	m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<float>( "opacity", norm( 0.0, 1.0, sliderPos ) ) ;
	m_ViewerCore->updateScene();
}

void SliderWidget::lowerThresholdChanged( int sliderPos )
{
	m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<double>( "lowerThreshold",
			norm( m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() , 0.0, 1000 - sliderPos ) * -1 );
	m_ViewerCore->updateScene();
}

void SliderWidget::upperThresholdChanged( int sliderPos )
{
	m_ViewerCore->getCurrentImage()->getPropMap().setPropertyAs<double>( "upperThreshold",
			norm( 0.0, m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>(), 1000 - sliderPos ) );
	m_ViewerCore->updateScene();
}

void SliderWidget::synchronize()
{
	if( m_ViewerCore->getCurrentImage().get() ) {
		if( m_ViewerCore->getCurrentImage()->getImageProperties().imageType == ImageHolder::z_map ) {
			setVisible( LowerThreshold, true );
			setVisible( UpperThreshold, true );
			setVisible( Opacity, true );
		} else if ( m_ViewerCore->getCurrentImage()->getImageProperties().imageType == ImageHolder::anatomical_image ) {
			setVisible( LowerThreshold, false );
			setVisible( UpperThreshold, false );
			setVisible( Opacity, true );
		}

		const unsigned short lowerThreshold = 1000 - abs( ( 1000 / m_ViewerCore->getCurrentImage()->getMinMax().first->as<double>() )
											  * m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<double>( "lowerThreshold" ) );

		const unsigned short upperThreshold = 1000 - abs( ( 1000 / m_ViewerCore->getCurrentImage()->getMinMax().second->as<double>() )
											  * m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<double>( "upperThreshold" ) );

		m_Interface.opacitySlider->setSliderPosition( m_ViewerCore->getCurrentImage()->getPropMap().getPropertyAs<float>( "opacity" ) * 1000 );

		m_Interface.minSlider->setSliderPosition( lowerThreshold );

		m_Interface.maxSlider->setSliderPosition( upperThreshold );
	}
}



}
}
}
