#include "GLLookupTable.hpp"


namespace isis
{
namespace viewer
{
namespace GL
{

GLuint GLLookUpTable::getLookUpTableAsTexture( const Color::LookUpTableType &lutType ) const
{
	size_t extent = std::numeric_limits<GLubyte>::max() + 1;
	Color colorHandler;
	colorHandler.setLutType( lutType );
	colorHandler.setNumberOfElements( extent );
	colorHandler.update();
	GLfloat *colorTable = ( GLfloat * ) calloc( extent * 3, sizeof( GLfloat ) );
	size_t index = 0;
	BOOST_FOREACH( QVector<QRgb>::const_reference color, colorHandler.getColorTable() ) {
		colorTable[index++] = ( float )QColor( color ).red() / float( extent - 1 );
		colorTable[index++] = ( float )QColor( color ).green() / float( extent - 1 );
		colorTable[index++] = ( float )QColor( color ).blue() / float( extent - 1 );
	}
	GLuint id;
	glEnable( GL_TEXTURE_1D );
	glGenTextures( 1, &id );
	glBindTexture( GL_TEXTURE_1D, id );
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	glTexImage1D( GL_TEXTURE_1D, 0, GL_RGB8, extent, 0, GL_RGB, GL_FLOAT, colorTable );
	glDisable( GL_TEXTURE_1D );
	return id;

}

}
}
}