#ifndef QPROGRESSFEEDBACK_HPP
#define QPROGRESSFEEDBACK_HPP

#include "CoreUtils/progressfeedback.hpp"
#include <QProgressBar>

namespace isis {
namespace viewer {
	
class QProgressFeedback : public util::ProgressFeedback
{
public:
	virtual void show( size_t max, std::string header = "" );
	virtual size_t progress( const std::string message = "", size_t step = 1 );
	virtual void close();
	virtual size_t getMax();
	
	QProgressFeedback();
	
	QProgressBar *getProgressBar() const { return m_ProgressBar; }
private:
	QProgressBar *m_ProgressBar;
	size_t m_CurrentVal;
};
	
	
}}



#endif