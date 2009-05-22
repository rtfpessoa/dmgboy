#ifndef __CARTRIDGE_H__
#define __CARTRIDGE_H__

#include <string>
using namespace std;

class Cartridge
{
private:
	char * mem_cartridge;
	unsigned int size;
	bool _isLoaded;
public:
	Cartridge(string path);
	~Cartridge();
	void Print(int beg, int end);
	char *GetData();
	unsigned int GetSize();
	bool isLoaded();
};

#endif
