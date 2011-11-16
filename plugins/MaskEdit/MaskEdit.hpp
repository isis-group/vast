#ifndef MASKEDIT_HPP
#define MASKEDIT_HPP

#include "ui_maskEdit.h"
#include "qviewercore.hpp"

namespace isis {
namespace viewer {
namespace plugin {

class MaskEditDialog : public QDialog
{
	Q_OBJECT
public:
	MaskEditDialog( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	void physicalCoordChanged( util::fvector4 physCoord );
	void createEmptyMask();
	virtual void closeEvent( QCloseEvent * );
	
private:
	Ui::maskEditDialog m_Interface;
	QViewerCore *m_ViewerCore;
	boost::shared_ptr<ImageHolder> m_CurrentMask;
	unsigned short m_Radius;
};
	
	
}}}





#endif