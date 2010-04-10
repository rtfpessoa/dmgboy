#include "Log.h"
#include <fstream>
using namespace std;

QueueLog::QueueLog(int maxItems)
{
	if (maxItems <= 0)
		maxItems = 100;
	
	this->maxItems = maxItems;
	this->numItems = 0;
	this->first = NULL;
	this->last = NULL;
}

QueueLog::~QueueLog()
{
	ItemLog * item;
	ItemLog * auxItem;
	
	item = first;
	while (item) {
		auxItem = item->next;
		delete item;
		item = auxItem;
	}
}

void QueueLog::Enqueue(string prefix, Registers * regs, string suffix)
{
	ItemLog * newItem = new ItemLog;
	newItem->prefix = prefix;
	newItem->regs = *regs;
	newItem->suffix = suffix;
	newItem->next = NULL;
	newItem->prev = NULL;
	
	if (numItems >= maxItems)
	{
		first = first->next;
		//Borrar el más viejo
		delete first->prev;
		first->prev = NULL;
		numItems--;
	}
	
	if (first == NULL)
	{
		first = newItem;
		last = newItem;
	}
	else
	{
		last->next = newItem;
		newItem->prev = last;
		last = newItem;
	}
	
	numItems++;

}

void QueueLog::Save(string path)
{
	ofstream file(path.c_str());
	
	if (file)
	{
		ItemLog * item;
		
		item = first;
		while (item) {
			file << item->prefix;
			file << item->regs.ToString() << endl;
			file << item->suffix;
			item = item->next;
		}
		file.close();
	}
}