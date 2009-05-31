#ifndef __GBEXCEPTION_H__
#define __GBEXCEPTION_H__

#include <string>
using namespace std;

class GBException: public exception
{
private:
	string description;

public:
	GBException(string desc);
	const char* what() const throw();
	~GBException() throw();
};

#endif
