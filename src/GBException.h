#ifndef __GBEXCEPTION_H__
#define __GBEXCEPTION_H__

#include <string>

class GBException: public std::exception
{
private:
	std::string description;

public:
	GBException(std::string desc);
	const char* what() const throw();
	~GBException() throw();
};

#endif
