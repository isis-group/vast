#include "loggingDialog.hpp"


isis::viewer::widget::LoggingDialog::LoggingDialog(QWidget* parent, isis::viewer::QViewerCore* core)
	: QDialog( parent ),
	m_ViewerCore( core)
{
	m_Interface.setupUi(this);
	m_Interface.errorCheck->setCheckState( Qt::Checked );
	m_Interface.warningCheck->setCheckState( Qt::Unchecked );
	m_Interface.infoCheck->setCheckState( Qt::Unchecked );
	m_Interface.verboseCheck->setCheckState( Qt::Unchecked );
	m_Interface.noticeCheck->setCheckState( Qt::Unchecked );
	connect( m_Interface.verboseCheck, SIGNAL(clicked()), this, SLOT( synchronize()));
	connect( m_Interface.infoCheck, SIGNAL(clicked()), this, SLOT( synchronize()));
	connect( m_Interface.warningCheck, SIGNAL(clicked()), this, SLOT( synchronize()));
	connect( m_Interface.errorCheck, SIGNAL(clicked()), this, SLOT( synchronize()));
	connect( m_Interface.noticeCheck, SIGNAL( clicked()), this, SLOT( synchronize()));
	
}


void isis::viewer::widget::LoggingDialog::synchronize()
{
	m_Interface.logList->clear();
	BOOST_FOREACH( std::list< qt4::QMessage>::const_reference message, m_ViewerCore->getMessageLog() ) 
	{
		QListWidgetItem *item = new QListWidgetItem();
		std::stringstream logStream;
		logStream << message.m_module << "(" << message.time_str << ") -> " << message.message ;
		item->setText( logStream.str().c_str() );
		switch (message.m_level) {
			case error:
				if( m_Interface.errorCheck->checkState() == Qt::Checked ) {
					item->setTextColor( Qt::red );
					m_Interface.logList->addItem( item );
				}
				break;
			case warning:
				if( m_Interface.warningCheck->checkState() == Qt::Checked ) {
					item->setTextColor( QColor(218,165,32 ) );
					m_Interface.logList->addItem( item );
				}
				break;
			case info:
				if( m_Interface.infoCheck->checkState() == Qt::Checked ) {
					item->setTextColor( Qt::black );
					m_Interface.logList->addItem( item );
				}
				break;
			case verbose_info:
				if( m_Interface.verboseCheck->checkState() == Qt::Checked ) {
					item->setTextColor( Qt::black );
					m_Interface.logList->addItem( item );
				}
				break;
			case notice:
				if( m_Interface.noticeCheck->checkState() == Qt::Checked ) {
					item->setTextColor( QColor(34,139,34) );
					m_Interface.logList->addItem( item );
				}
				break;
		}
		
	}
}
