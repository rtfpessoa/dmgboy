/*
 This file is part of gbpablog.
 
 gbpablog is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 gbpablog is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with gbpablog.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Log.h"
#include <fstream>
#include <string>
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
		if (item->regs)
			delete item->regs;
		delete item;
		item = auxItem;
	}
}

void QueueLog::Enqueue(string prefix, Registers * regs, string suffix)
{
	ItemLog * newItem = new ItemLog;
	newItem->prefix = prefix;
	newItem->regs = NULL;
	if (regs)
	{
		newItem->regs = new Registers();
		*newItem->regs = *regs;
	}
	newItem->suffix = suffix;
	newItem->next = NULL;
	newItem->prev = NULL;
	
	if (numItems >= maxItems)
	{
		first = first->next;
		//Borrar el más viejo
		if (first->prev->regs)
			delete first->prev->regs;
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
			if (item->regs)
				file << item->regs->ToString();
			file << item->suffix << endl;
			item = item->next;
		}
		file.close();
	}
}
