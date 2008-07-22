/*
Copyright (C) 2007 <SWGEmu>
 
This File is part of Core3.
 
This program is free software; you can redistribute 
it and/or modify it under the terms of the GNU Lesser 
General Public License as published by the Free Software
Foundation; either version 2 of the License, 
or (at your option) any later version.
 
This program is distributed in the hope that it will be useful, 
but WITHOUT ANY WARRANTY; without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU Lesser General Public License for
more details.
 
You should have received a copy of the GNU Lesser General 
Public License along with this program; if not, write to
the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 
Linking Engine3 statically or dynamically with other modules 
is making a combined work based on Engine3. 
Thus, the terms and conditions of the GNU Lesser General Public License 
cover the whole combination.
 
In addition, as a special exception, the copyright holders of Engine3 
give you permission to combine Engine3 program with free software 
programs or libraries that are released under the GNU LGPL and with 
code included in the standard release of Core3 under the GNU LGPL 
license (or modified versions of such code, with unchanged license). 
You may copy and distribute such a system following the terms of the 
GNU LGPL for Engine3 and the licenses of the other code concerned, 
provided that you include the source code of that other code when 
and as the GNU LGPL requires distribution of source code.
 
Note that people who make modified versions of Engine3 are not obligated 
to grant this special exception for their modified versions; 
it is their choice whether to do so. The GNU Lesser General Public License 
gives permission to release a modified version without this exception; 
this exception also makes it possible to release a modified version 
which carries forward this exception.
*/
#ifndef BLUEFROGTREE_H_
#define BLUEFROGTREE_H_

#include "engine/engine.h"

class BlueFrogTree {
	
private:
	BlueFrogTree * parent;
	string element;
	LinkedList<BlueFrogTree *> children;
	
public:
	
	BlueFrogTree(string item, BlueFrogTree * par = NULL) {
		element = item;
		parent = par;
	}
	
	void add(string item) {
		if (element.empty())
			element = item;
		else {
			BlueFrogTree * child = new BlueFrogTree(item, this);
			children.add(child);	
		}	
	}
	
	void set(string item) {
		element = item;
	}
	
	string getElement() {
		return element;
	}
	
	LinkedList<BlueFrogTree *> getChildren() {
		return children;
	}
	
	int size() {
		if (element.empty() && children.isEmpty())
			return 0;
		else if (!this->isGroup())
			return 1;
		else {
			int count = 0;
			
			for (int i = 0; i < children.size(); i++)
				count += children.get(i)->size();
			
			return 1 + count;
		}
	}
	
	bool isEmpty() {
		return this->size() == 0;
	}
	
	bool isGroup() {
		return !children.isEmpty();
	}
	
	BlueFrogTree * find(string obj) {
		if (element.compare(obj) == 0)
			return this;
		
		for (int i = 0; i < children.size(); i++) {
			BlueFrogTree * t = children.get(i)->find(obj);
			if (t != NULL)
				return t;
		}
		
		return NULL;
	}
	
};

#endif /*BLUEFROGTREE_H_*/
