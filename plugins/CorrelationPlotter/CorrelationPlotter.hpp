#ifndef CORRELATIONPLOTTER_HPP
#define CORRELATIONPLOTTER_HPP

#include "ui_correlationPlotter.h"
#include <QWidget>
#include "qviewercore.hpp"
#include <cmath>

#ifdef _OPENMP
#include <omp.h>
#endif /*_OPENMP*/

namespace isis {
namespace viewer {
namespace plugin {

class CorrelationPlotterDialog : public QDialog
{
	Q_OBJECT
	typedef float MapImageType;
	typedef uint16_t InternalFunctionalImageType;
public:
	CorrelationPlotterDialog( QWidget *parent, QViewerCore *core );
	
public Q_SLOTS:
	virtual void showEvent( QShowEvent* );
	virtual void closeEvent( QCloseEvent* );
	void physicalCoordsChanged( util::fvector4 );
	bool createCorrelationMap();
	void calculateCorrelation(bool all = false);
	void lockClicked();
	
private:
	ViewerCoreBase::Mode m_OrigMode;
	Ui::correlationPlotterDialog m_Interface;
	QViewerCore *m_ViewerCore;
	boost::shared_ptr<ImageHolder> m_CurrentCorrelationMap;
	boost::shared_ptr<ImageHolder> m_CurrentFunctionalImage;
	boost::scoped_ptr< isis::data::Chunk > m_InternalChunk;
	util::ivector4 m_CurrentVoxelPos;
	
	
	void _internCalculateCorrelation( const util::ivector4 &vec, const double &s_x, const double &_x, const InternalFunctionalImageType *vx, const size_t &n, const size_t &vol  );	

};
	

}}}

#endif