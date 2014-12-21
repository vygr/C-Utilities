/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

#ifndef __heap_1234_
#define __heap_1234_

/////////////////////////////////////////////////
/// Heap class.
///
/// Provides very fast fixed size object allocation and deallocation pools.
/////////////////////////////////////////////////

class Heap
{
private:
	struct Heapcell
	{
		Heapcell *next;
	};
	
	struct Heapblock
	{
		Heapblock *next;
	};
	
	Heapcell *_freelist;
	Heapblock *_blocklist;
	unsigned int _cellsize;
	unsigned int _blocksize;
public:
	explicit Heap(unsigned int csize, unsigned int bsize);
	~Heap();
	
	void free_heap();
	void *alloc_cell();
	void free_cell(void *cell);
};

/////////////////////////////////////////////////
/// Free cell.
///
/// Return cell to the free list.
/////////////////////////////////////////////////

inline void Heap::free_cell(void *cell)
{
	//add_child_at_tail cell to _freelist
	Heapcell *first = _freelist;
	_freelist = (Heapcell *)cell;
	((Heapcell *)cell)->next = first;
}

#endif
