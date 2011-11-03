#include "qprogressfeedback.hpp"



namespace isis {
namespace viewer {
	
QProgressFeedback::QProgressFeedback()
	: m_ProgressBar( new QProgressBar() ),
	m_CurrentVal(0)
{
	m_ProgressBar->setMaximumHeight(20);
}

		
void QProgressFeedback::show(size_t max, std::string header)
{
	m_ProgressBar->setMaximum( max );
	m_ProgressBar->setMinimum( 0 );
	m_ProgressBar->show();
}

size_t QProgressFeedback::progress(const std::string message, size_t step)
{
	m_CurrentVal+=step;
	m_ProgressBar->setValue( m_CurrentVal );
	return m_CurrentVal;
}

void QProgressFeedback::close()
{
	m_CurrentVal = 0;
	m_ProgressBar->setVisible(false);
}

size_t QProgressFeedback::getMax()
{
	return m_ProgressBar->maximum();
}

	
}}