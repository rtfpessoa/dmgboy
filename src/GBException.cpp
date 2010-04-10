#include "GBException.h"

using namespace std;

GBException::GBException(): exception()
{
	newException("", Unknown);
}

GBException::GBException(string description): exception()
{
	newException(description, Unknown);
}

GBException::GBException(string description, ExceptionType type): exception()
{
	newException(description, type);
}

GBException::~GBException() throw()
{

}

ExceptionType GBException::GetType()
{
	return type;
}

void GBException::newException(string description, ExceptionType type)
{
	this->description = description;
	this->type = type;
}

const char * GBException::what() const throw()
{
	return description.c_str();
}
