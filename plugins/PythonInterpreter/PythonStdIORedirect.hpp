#ifndef PYTHONSTDIOREDIRECT_HPP
#define PYTHONSTDIOREDIRECT_HPP


#include <iostream>
#include <boost/circular_buffer.hpp>

class PythonStdIoRedirect {
public:
    typedef boost::circular_buffer<std::string> ContainerType;
    void Write( std::string const& str );
    static std::string GetOutput();

private:
    static ContainerType m_outputs; // must be static, otherwise output is missing
};



#endif