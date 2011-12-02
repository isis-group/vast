#include "PythonStdIORedirect.hpp"

#include <sstream>

PythonStdIoRedirect::ContainerType PythonStdIoRedirect::m_outputs;

void PythonStdIoRedirect::Write(const std::string& str)
{
	if (m_outputs.capacity()<100) {
		m_outputs.resize(100);
	}
	m_outputs.push_back(str);
}

std::string PythonStdIoRedirect::GetOutput()
{
	std::string ret;
	std::stringstream ss;
	for(boost::circular_buffer<std::string>::const_iterator it=m_outputs.begin();
		it!=m_outputs.end();
		it++)
	{
		ss << *it;
	}
	m_outputs.clear();
	ret =  ss.str();
	return ret;
}
