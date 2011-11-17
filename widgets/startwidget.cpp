#include "startwidget.hpp"
#include "uicore.hpp"
#include "filedialog.hpp"

namespace isis {
namespace viewer {
namespace widget {
	
	
StartWidget::StartWidget(QWidget* parent, QViewerCore* core)
	: QDialog(parent),
	m_ViewerCore( core )
{
	m_Interface.setupUi(this);
	connect( m_Interface.openImageButton, SIGNAL( clicked()), this, SLOT( openImageButtonClicked()));
	connect( m_ViewerCore, SIGNAL( emitStatus(QString)), this, SLOT( statusChanged(QString)));
	connect( m_Interface.showMeCheck, SIGNAL( clicked(bool)), this, SLOT( showMeChecked(bool)));
	connect( m_Interface.favList, SIGNAL( doubleClicked(QModelIndex)), this, SLOT( openPath()));

}

void StartWidget::showMeChecked(bool checked )
{
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>("showStartWidget", checked );
}

void StartWidget::keyPressEvent(QKeyEvent *e )
{
	if( e->key() == Qt::Key_Escape ) {
		close();
	}
}


void StartWidget::closeEvent(QCloseEvent* )
{
	m_ViewerCore->getUI()->getMainWindow()->setEnabled(true);
}


void StartWidget::openPath()
{
	close();
	QStringList fileList;
	fileList.push_back( m_Interface.favList->currentItem()->text() );
	m_ViewerCore->openPath( fileList, ImageHolder::anatomical_image, "", "", true );
}

void StartWidget::showEvent(QShowEvent* )
{
	uint16_t width = m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("startWidgetWidth");
	uint16_t height = m_ViewerCore->getOptionMap()->getPropertyAs<uint16_t>("startWidgetHeight");
	const QRect screen = QApplication::desktop()->screenGeometry();

	setMaximumHeight(height);
	setMaximumWidth(width);
	setMinimumHeight(height);
	setMinimumWidth(width);
	QPixmap pixMap( ":/common/vast_big.jpg" );
	float ratio = pixMap.height() / (float)pixMap.width();
	m_Interface.imageLabel->setPixmap( pixMap.scaled(width, width * ratio) );
	m_Interface.buttonFrame->setMaximumHeight( height - width * ratio );
	move( m_ViewerCore->getUI()->getMainWindow()->rect().center().x() - this->width() / 2,  m_ViewerCore->getUI()->getMainWindow()->rect().center().y() - this->height() / 2 );
	
	
	m_ViewerCore->getUI()->getMainWindow()->setEnabled(false);
	setEnabled( true );
}


void StartWidget::showMe(bool asStartDialog)
{

	if( asStartDialog ) {
		m_Interface.buttonFrame->setVisible(true);
		m_Interface.statusLabel->setText( m_ViewerCore->getVersion().c_str() );
		m_Interface.showMeCheck->setChecked( m_ViewerCore->getOptionMap()->getPropertyAs<bool>("showStartWidget"));
		m_ViewerCore->getSettings()->beginGroup("UserProfile" );
		QList<QVariant> favFiles = m_ViewerCore->getSettings()->value( "favoriteFiles" ).toList();
		m_ViewerCore->getSettings()->endGroup();
		m_Interface.favList->clear();
		if( favFiles.size() ) {
			BOOST_FOREACH( QList<QVariant>::const_reference path, favFiles ) {
				unsigned short validFiles;
				QListWidgetItem *item = new QListWidgetItem( path.toString() );
				if( FileDialog::checkIfPathIsValid( path.toString(), validFiles, "" ) ) {
					item->setTextColor(QColor( 34, 139, 34 ));
				} else {
					item->setTextColor( Qt::red );
				}
				m_Interface.favList->addItem( item );
			}
		} else {
			m_Interface.favList->setVisible(false);
		}
		
		adjustSize();
		show();
	} else {
		m_Interface.buttonFrame->setVisible(false);
		show();
	}
}

void StartWidget::statusChanged(QString status)
{
	
	m_Interface.statusLabel->setText( status );
	QCoreApplication::processEvents();
}


void StartWidget::openImageButtonClicked()
{
	close();
	m_ViewerCore->getUI()->getMainWindow()->openImage();
}


	
}}}