/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

#include "list.h"

Listnode *Listhead::enumerate_forwards(Listnode *(*callback)(Listhead *, Listnode *, void *), void *user)
{
	Listnode *next, *node, *status = 0;
	next = get_head();
	do
	{
		node = next;
		next = next->get_succ();
		if (!next) { break; }
		status = callback(this, node, user);
	} while (!status);
	return status;
}

Listnode *Listhead::enumerate_backwards(Listnode *(*callback)(Listhead *, Listnode *, void *), void *user)
{
	Listnode *next, *node, *status = 0;
	next = get_tail();
	do
	{
		node = next;
		next = next->get_pred();
		if (!next) { break; }
		status = callback(this, node, user);
	} while (!status);
	return status;
}

Listnode *Listhead::get_node_at_index(unsigned int i)
{
	Listnode *next = get_head();
	for (;;)
	{
		Listnode *node = next;
		next = next->get_succ();
		if (!next) return 0;
		if (!i) return node;
		i--;
	}
}

unsigned int Listhead::get_index_of_node(Listnode *node)
{
	Listnode *next = get_head();
	unsigned int i;
	for (i = 0; ; i++)
	{
		Listnode *n = next;
		next = next->get_succ();
		if (!next) return (unsigned int)-1;
		if (n == node) break;
	}
	return i;
}
