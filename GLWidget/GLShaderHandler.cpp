#include "GLShaderHandler.hpp"
#include <boost/foreach.hpp>


namespace isis
{
namespace viewer
{
namespace GL 
{
	
GLShaderHandler::GLShaderHandler()
	: m_isEnabled( false ),
	  m_Context( false )
{

}

void GLShaderHandler::addShader( const std::string &name, const std::string &source, const isis::viewer::GL::GLShader::ShaderType &shaderType )
{
	GLShader shader;
	shader.setShaderType( shaderType );
	shader.setSourceCode( source );

	switch( shaderType ) {
	case GLShader::fragment:
		shader.setShaderID( glCreateShaderObjectARB( GL_FRAGMENT_SHADER ) );
		break;
	case GLShader::vertex:
		shader.setShaderID( glCreateShaderObjectARB( GL_VERTEX_SHADER ) );
		break;
	}

	const GLcharARB *csource  = source.c_str();

	glShaderSourceARB( shader.getShaderID(), 1, &csource, NULL );

	glCompileShaderARB( shader.getShaderID() );

	GLint compileStatus;
#ifndef __APPLE__
	glGetShaderiv( static_cast<GLuint>(shader.getShaderID()), GL_COMPILE_STATUS, &compileStatus );

	if( compileStatus == GL_FALSE ) {
		LOG( Runtime, error ) << "Error during compilation of shader " << name << " !";
	} else {
		LOG( Debug, info ) << "Shader " << name << " with id " << shader.getShaderID() << " compiled successfully!";
	}
#else
	checkAndReportGLError( "compiling shader" + name );
#endif
	m_ShaderMap[name] = shader;
}

void GLShaderHandler::setEnabled( bool enable )
{
	if(!m_Context) {
		LOG(Runtime, error) << "You first have to create the shader context";
	}
	if( enable && !m_isEnabled) {
		BOOST_FOREACH( ShaderMapType::const_reference shader, m_ShaderMap ) {
			glAttachObjectARB(m_ProgramID, shader.second.getShaderID());
			checkAndReportGLError( "attaching shader"  );
		}
		glLinkProgramARB( m_ProgramID );
		glValidateProgramARB( m_ProgramID );
		glUseProgramObjectARB( m_ProgramID );
		m_isEnabled = true;
		checkAndReportGLError( "enabling shader" );
	} else if (!enable && m_isEnabled ){
		BOOST_FOREACH( ShaderMapType::const_reference shader, m_ShaderMap ) {
			glDetachObjectARB( m_ProgramID, shader.second.getShaderID() );
			checkAndReportGLError( "detaching shader" );
		}
		m_isEnabled = false;
	}

}

void GLShaderHandler::removeShader( const std::string &name )
{
	glDetachObjectARB( m_ProgramID, m_ShaderMap[name].getShaderID() );
	m_ShaderMap.erase( name );
}


void GLShaderHandler::addShader( const std::string &name, const isis::viewer::GL::GLShader &shader )
{
	m_ShaderMap[name] = shader;
}

}
}
} //end namespace