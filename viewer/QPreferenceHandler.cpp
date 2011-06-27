#include "QPreferenceHandler.hpp"


namespace isis {
namespace viewer {
namespace pref {
	
QPreferenceHandler::QPreferenceHandler(const std::string& appName, const std::string& orgName)
{
	m_Settings = new QSettings( appName.c_str(), orgName.c_str() );
}

	
	
}}}
