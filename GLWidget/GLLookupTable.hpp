#ifndef GLLOOKUPTABLE_HPP
#define GLLOOKUPTABLE_HPP

#include "color.hpp"
#include "GLCommon.hpp"

namespace isis
{
namespace viewer
{
namespace GL
{

class GLLookUpTable
{
public:


	GLuint getLookUpTableAsTexture( const Color::LookUpTableType &lutType ) const;


};

}
}
}

#endif