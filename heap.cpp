/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

#include "heap.h"

/////////////////////////////////////////////////
/// Create heap.
///
/// Create a new heap with the given cell and block sizes.\n
/// No blocks are allocated on construction, the first cell allocation request will acquire the first memory block.
/////////////////////////////////////////////////

Heap::Heap(unsigned int csize, unsigned int bsize)
: _freelist(0)
, _blocklist(0)
, _cellsize((csize + 3) & - 4)
, _blocksize(bsize)
{}

/////////////////////////////////////////////////
/// Destroy heap.
///
/// All memory blocks will be freed.
/////////////////////////////////////////////////

Heap::~Heap()
{
	//free all blocks
	Heapblock *node = _blocklist;
	while (node)
	{
		Heapblock *next = node->next;
		delete[] (char*)node;
		node = next;
	}
}

/////////////////////////////////////////////////
/// Free heap.
///
/// All memory blocks currently allocated are split into cells and those cells placed on the free cell list.\n
/// All current cells are therefore invalidated and returned to the free pool.
/////////////////////////////////////////////////

void Heap::free_heap()
{
	//enumerate all blocks and add to _freelist
	Heapcell *first = 0;
	Heapblock *node = _blocklist;
	while (node)
	{
		Heapcell *cell = (Heapcell*)((char*)node + sizeof(Heapblock));
		unsigned int i;
		for (i = 0; i < (_blocksize / _cellsize); i++)
		{
			cell->next = first;
			first = cell;
			cell = (Heapcell*)((char*)cell + _cellsize);
		}
		node = node->next;
	}
	_freelist = first;
}

/////////////////////////////////////////////////
/// Allocate cell.
///
/// Allocate a single cell, from the free cell list, fallback to allocating a new block if needed.
/////////////////////////////////////////////////

void *Heap::alloc_cell()
{
	//try allocating from _freelist
	Heapcell *cell = _freelist;
	if (cell)
	{
		_freelist = cell->next;
		return cell;
	}
	
	//need new block
	Heapblock *node = (Heapblock*)new char [_blocksize + sizeof(Heapblock)];
	
	//add block to block list
	node->next = _blocklist;
	_blocklist = node;
	
	//add cells to _freelist, reserve one for allocating
	Heapcell *first = _freelist;
	cell = (Heapcell*)((char*)node + sizeof(Heapblock));
	unsigned int i;
	for (i = 0; i < ((_blocksize / _cellsize) - 1); i++)
	{
		cell->next = first;
		first = cell;
		cell = (Heapcell*)((char*)cell + _cellsize);
	}
	_freelist = first;
	return cell;
}
