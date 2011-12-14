#ifndef SCALINGWIDGET_HPP
#define SCALINGWIDGET_HPP

#include <QDialog>
#include "ui_scalingDialog.h"
#include "qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{


class ScalingWidget : public QDialog
{
	Q_OBJECT
public:
	ScalingWidget( QWidget *parent, QViewerCore *core );
	virtual void keyPressEvent( QKeyEvent *event );

public Q_SLOTS:
	void synchronize();
	void minChanged( double );
	void maxChanged( double );
	void scalingChanged( double );
	void offsetChanged( double );
	void reset();
	void autoScale();
	void showMe( bool visible );
	void applyScalingOffset( const double &scaling, const double &offset, bool global );


private:
	Ui::scalingOptionDialog m_Interface;
	QViewerCore *m_ViewerCore;

	std::pair<double, double> getScalingOffsetFromMinMax( const std::pair<double, double> &minMax, boost::shared_ptr<ImageHolder> image );
	std::pair<double, double> getMinMaxFromScalingOffset( const std::pair<double, double> &scalingOffset,  boost::shared_ptr<ImageHolder> image );
	void setMinMax( std::pair<double, double> minMax, boost::shared_ptr<ImageHolder> image ) ;
	void setScalingOffset( std::pair<double, double> scalingOffset ) ;
};

}
}
}

#endif