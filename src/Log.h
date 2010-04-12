#ifndef __LOGQUEUE_H__
#define __LOGQUEUE_H__

#include "Registers.h"

struct ItemLog {
	string prefix;
	Registers * regs;
	string suffix;
	ItemLog * next;
	ItemLog * prev;
};

//Esta clase se encarga de guardar los valores de los registros hasta un
//número máximo. Cuando se alcanza dicho máximo y antes de insertar uno
//nuevo, se borra el más antiguo. Es decir, tiene el funcionamiento de un cola.
//El más nuevo será el apuntado por last y el más viejo por first.
//	item1 -> item2 -> item3
//	  ^					^
//  first			  last
class QueueLog
{
private:
	int maxItems;
	int numItems;
	ItemLog * first;
	ItemLog * last;
public:
	QueueLog(int maxItems);
	~QueueLog();
	void Enqueue(std::string prefix, Registers * regs, std::string suffix);
	void Save(std::string path);
};

#endif