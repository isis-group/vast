#ifndef GLSHADER_HPP
#define GLSHADER_HPP

#include "GLCommon.hpp"
#include <iostream>
#include <map>
#include "common.hpp"


namespace isis
{
namespace viewer
{
namespace GL
{

class GLShader
{
public:
	enum ShaderType { fragment, vertex };
	void setSourceCode( const std::string &source ) { m_SourceCode = source; }
	void setShaderID ( const GLhandleARB id ) { m_ShaderID = id; }
	void setShaderType ( const ShaderType &type ) { m_ShaderType = type; }

	GLhandleARB getShaderID() const { return m_ShaderID; }
private:
	std::string m_SourceCode;
	GLhandleARB m_ShaderID;
	ShaderType m_ShaderType;
};


class GLShaderHandler
{
public:
	GLShaderHandler();
	typedef std::map<std::string, GLShader> ShaderMapType;

	ShaderMapType getShaderMap() const { return m_ShaderMap; }

	void addShader( const std::string &name, const std::string &source, const GLShader::ShaderType &shaderType );
	void addShader( const std::string &name, const GLShader &shader );
	void removeShader( const std::string &name );
	bool isEnabled() const { return m_isEnabled; }
	void setEnabled( bool enable );
	void createContext () {
		m_ProgramID = glCreateProgramObjectARB();
		m_Context = true;
		checkAndReportGLError( "creating shader context " );
	}
	template <typename TYPE>
	bool addVariable( const std::string &name, TYPE var, bool integer = false ) {
		if( m_Context ) {
			LOG( Debug, verbose_info ) << "Setting shader value " << name << " to " << var;
			GLint location = glGetUniformLocationARB( m_ProgramID, name.c_str() );

			if( integer ) {
				glUniform1iARB( location, var );
			} else {
				glUniform1fARB( location, var );
			}

			checkAndReportGLError( "setting shader value " + name );
			return true;
		} else {
			LOG( Runtime, error ) << "You first have to set a shader context before adding a variable!";
			return false;
		}


	}

private:
	ShaderMapType m_ShaderMap;
	GLhandleARB m_ProgramID;
	bool m_isEnabled;
	bool m_Context;
};

}
}
} //end namespace




#endif