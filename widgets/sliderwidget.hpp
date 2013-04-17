#ifndef SLIDERWIDGET_HPP
#define SLIDERWIDGET_HPP

#include "ui_sliderWidget.h"
#include "../viewer/qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace ui
{


class SliderWidget : public QWidget
{
	Q_OBJECT
public:
	enum SliderType { Opacity, UpperThreshold, LowerThreshold };
	SliderWidget( QWidget *parent, QViewerCore *core );

	void setSliderVisible( SliderType, bool );

public Q_SLOTS:
	void opacityChanged( int );
	void lowerThresholdChanged( int );
	void upperThresholdChanged( int );
	void synchronize();
	void toggleGlobal( bool );

private:
	double norm( const double &min, const double &max, const int &pos );
	Ui::sliderwidget m_Interface;
	QViewerCore *m_ViewerCore;
	double m_maxMax;
	double m_maxMin;

};

}
}
}


#endif
