#include "filedialog.hpp"
#include <sys/stat.h>


isis::viewer::widget::FileDialog::FileDialog(QWidget* parent, QViewerCore *core )
	: QDialog (parent),
	m_ViewerCore( core ),
	m_ImageType( ImageHolder::anatomical_image ),
	m_Completer( new QCompleter( this ))
	
{
	
	m_Interface.setupUi(this);
	setMinimumWidth(500);
	m_Completer->setModel( new QDirModel( m_Completer ) );
	m_Completer->setCompletionMode(QCompleter::PopupCompletion);
	m_Interface.fileDirEdit->setCompleter( m_Completer );
	
	std::stringstream fileFormats;
	fileFormats << "Image files (" << getFileFormatsAsString(isis::image_io::FileFormat::read_only, std::string( "*." ) ) << ")";
	m_FileDialog.setNameFilter( fileFormats.str().c_str());
	m_Interface.distributeCheck->setEnabled( false );
	m_Interface.typeComboBox->addItem( "structural image" );
	m_Interface.typeComboBox->addItem( "zmap" );
	connect( m_Interface.browseButton, SIGNAL( clicked()), this, SLOT( browse()));
	connect( m_Interface.fileDirEdit, SIGNAL(textChanged(QString)),this, SLOT( parsePath()));
	connect( m_Interface.advancedOptionsCheck, SIGNAL(clicked(bool)),this, SLOT( advancedChecked(bool)));
	connect( m_Interface.openSaveButton, SIGNAL( clicked()), this, SLOT( openPath()));
	connect( m_Interface.cancelButton, SIGNAL( clicked()), this, SLOT( close()));
	connect( m_Interface.imageRadio, SIGNAL(clicked()), this, SLOT( modeChanged())) ;
	connect( m_Interface.dirRadio, SIGNAL(clicked()), this, SLOT( modeChanged())) ;
	connect( m_Interface.typeComboBox, SIGNAL( currentIndexChanged(int)), this, SLOT(imageTypeChanged(int)));
	connect( m_Interface.rfComboBox, SIGNAL( currentIndexChanged(int)), this, SLOT( rfChanged(int)));
	
}

void isis::viewer::widget::FileDialog::showEvent(QShowEvent* )
{
	setup();
}

void isis::viewer::widget::FileDialog::imageTypeChanged(int imageType)
{
	m_ImageType = static_cast<ImageHolder::ImageType>(imageType);
}

void isis::viewer::widget::FileDialog::rfChanged(int rfIndex)
{
	m_Suffix = m_Interface.rfComboBox->itemText(rfIndex).toStdString();
	parsePath();
}


void isis::viewer::widget::FileDialog::setup()
{
	std::string fileDialogTitle;
	m_FileDialog.setViewMode( QFileDialog::Detail );
	m_FileDialog.setDirectory( m_ViewerCore->getCurrentPath().c_str() );
	switch( m_Mode ) {
		case OPEN_DIR:
			m_FileDialog.setWindowTitle( "Open Directory" );
			m_FileDialog.setFileMode( QFileDialog::Directory );
			m_FileDialog.setOption( QFileDialog::ShowDirsOnly, true );
			m_Interface.fileDirLabel->setText( "Directory: " );
			m_Interface.openSaveButton->setText( "Open Directory" );
			m_FileFormatList = getFileFormatsAsList( isis::image_io::FileFormat::read_only );
			m_Interface.dirRadio->setChecked(true);
			break;
		case OPEN_FILE:
			m_FileDialog.setWindowTitle( "Open File(s)" );
			m_FileDialog.setFileMode( QFileDialog::ExistingFiles );
			m_Interface.fileDirLabel->setText( "File(s): " );			
			m_Interface.openSaveButton->setText( "Open File(s)" );
			m_FileFormatList = getFileFormatsAsList( isis::image_io::FileFormat::read_only );
			m_Interface.imageRadio->setChecked(true);
			break;
	}
	
	
	if( m_ViewerCore->getOptionMap()->getPropertyAs<bool>("showAdvancedFileDialogOptions") ) {
		m_Interface.advancedOptionsCheck->setCheckState( Qt::Checked );
		m_Interface.advancedOptionsFrame->setVisible( true );
	} else {
		m_Interface.advancedOptionsCheck->setCheckState( Qt::Unchecked );
		m_Interface.advancedOptionsFrame->setVisible( false );
	}
	adjustSize();
	m_Interface.openSaveButton->setEnabled( false );
	m_PathList.clear();
	m_Interface.fileDirEdit->clear();
	m_Interface.rfComboBox->clear();
	m_Interface.rfComboBox->addItem( "" );
	BOOST_FOREACH( std::list<std::string>::const_reference suffix, m_FileFormatList ) {
		m_Interface.rfComboBox->addItem( suffix.c_str() );
	}
	m_Interface.rfComboBox->setCurrentIndex(0);
}


void isis::viewer::widget::FileDialog::parsePath()
{
	unsigned short validFiles = 0;
	QPalette pal;
	pal.setColor(QPalette::Text, Qt::black );
	if( m_Interface.fileDirEdit->count() > 1 ) {
		m_PathList.clear();
		for( unsigned short i = 0; i < m_Interface.fileDirEdit->count(); i++ ) {
			if( checkIfPathIsValid( m_Interface.fileDirEdit->itemText(i), validFiles ) ) {
				m_PathList.push_back( m_Interface.fileDirEdit->itemText(i) );
			}	
		}
	} else { 
		if( checkIfPathIsValid( m_Interface.fileDirEdit->currentText(), validFiles ) ) {
			m_PathList.clear();
			m_PathList.push_back( m_Interface.fileDirEdit->currentText() );
		}
	}
	if ( !validFiles ) {
		pal.setColor( QPalette::Text, Qt::red );
		m_Interface.openSaveButton->setEnabled( false );
	} else {
		pal.setColor(QPalette::Text, QColor(34,139,34) );
		m_Interface.openSaveButton->setEnabled( true );
	}
	std::stringstream ss;
	ss << "Open " << validFiles << " images";
	m_Interface.openSaveButton->setText( ss.str().c_str() );
	m_Interface.fileDirEdit->setPalette(pal);	
	if( validFiles > 1 ) {
		m_Interface.distributeCheck->setEnabled(true);
	}else {
		m_Interface.distributeCheck->setEnabled( false );
	}
}

bool isis::viewer::widget::FileDialog::checkIfPathIsValid(QString path, unsigned short &validFiles, bool acceptNoSuffix)
{
	boost::filesystem::path p(path.toStdString());
	//ok, path exists
	if( boost::filesystem::exists(p) ) {
		//is dir?
		if( boost::filesystem::is_directory(p) && m_Mode == OPEN_DIR ) {
			for ( boost::filesystem::directory_iterator itr( p ); itr != boost::filesystem::directory_iterator(); ++itr ) {
				if ( boost::filesystem::is_directory( *itr ) )continue;
				checkIfPathIsValid( itr->path().file_string().c_str(), validFiles );
			}
			if( validFiles ) {
				return true;
			}
			//
		} else {
			std::string extension = boost::filesystem::extension( p );
			//lstrip "."
			extension.erase(0,1);
			if( !m_Suffix.size() ) {
				if( std::find( m_FileFormatList.begin(), m_FileFormatList.end(), extension) != m_FileFormatList.end() ) {
					validFiles++;
					return true;
				} 
			} else {
				validFiles++;
				return true;
			}
		}
	}
	return false;
}


void isis::viewer::widget::FileDialog::browse()
{
	boost::filesystem::path p( m_Interface.fileDirEdit->currentText().toStdString() );
	if ( boost::filesystem::is_directory(  boost::filesystem::path(p.directory_string()) ) ) {
		m_FileDialog.setDirectory( p.directory_string().c_str() );
	}
	QStringList files;
	if( m_FileDialog.exec() ) {
		files = m_FileDialog.selectedFiles();
	}
	m_Interface.fileDirEdit->clear();
	m_PathList.clear();
	disconnect( m_Interface.fileDirEdit, SIGNAL(textChanged(QString)), this, SLOT( parsePath()));
	BOOST_FOREACH( QStringList::reference file, files ) {
		m_Interface.fileDirEdit->addItem( file );
	}
	connect( m_Interface.fileDirEdit, SIGNAL(textChanged(QString)),this, SLOT( parsePath()));
	parsePath();
}

void isis::viewer::widget::FileDialog::openPath()
{
//TODO check if valid
	close();
	m_ViewerCore->openPath( m_PathList, m_ImageType, m_Dialect, m_Suffix, m_Interface.distributeCheck->isChecked() );
}

void isis::viewer::widget::FileDialog::advancedChecked(bool advanced)
{
	m_ViewerCore->getOptionMap()->setPropertyAs<bool>("showAdvancedFileDialogOptions", advanced );
	m_Interface.advancedOptionsFrame->setVisible(advanced);
	adjustSize();
}


void isis::viewer::widget::FileDialog::closeEvent(QCloseEvent* )
{
	
}

void isis::viewer::widget::FileDialog::modeChanged()
{
	if( m_Interface.dirRadio->isChecked() ) {
		m_Mode = OPEN_DIR;
	} else {
		m_Mode = OPEN_FILE;
	}
	setup();
}

