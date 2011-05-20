#include "GLCommon.hpp"

namespace isis {
namespace viewer {
namespace GL {
	
bool checkAndReportGLError( std::string context, LogLevel level )
{
	GLenum glErrorCode = glGetError();
	if( glErrorCode ) {
		LOG( Runtime, level ) << "Error in context " << context << ". Error code is " << glErrorCode
								<< " ( " << gluErrorString(glErrorCode) << " )";
	}
	
}
	
}}}