#ifndef __GBEXCEPTION_H__
#define __GBEXCEPTION_H__

#include <string>

enum ExceptionType { Unknown, Error, Exit };

class GBException: public std::exception
{
private:
	std::string description;
	ExceptionType type;
	void newException(std::string desc, ExceptionType type);

public:
	GBException();
	GBException(std::string desc);
	GBException(std::string desc, ExceptionType type);
	const char* what() const throw();
	ExceptionType GetType();
	~GBException() throw();
};

#endif
