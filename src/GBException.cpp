#include "GBException.h"

using namespace std;

GBException::GBException(string desc): exception(), description(desc)
{

}

GBException::~GBException() throw()
{

}

const char * GBException::what() const throw()
{
	return description.c_str();
}
