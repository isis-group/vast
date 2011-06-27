#ifndef QPREFERENCEHANDLER_HPP
#define QPREFERENCEHANDLER_HPP
#include <QSettings>


namespace isis {
namespace viewer {
namespace pref {
	
	
class QPreferenceHandler 
{
public:
	QPreferenceHandler( const std::string &appName, const std::string &orgName );
	
	QSettings* getQSettings() { return m_Settings; }
	const QSettings* getQSettings() const { return m_Settings; }
	
private:
	QSettings* m_Settings;
		
};

	
}}}




#endif
