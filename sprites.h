/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

#ifndef __sprites__
#define __sprites__

#include "engine.h"

//sprite class templates

class CP_Man
	: public CP
{
public:
	CP_Man(Sprite *sprite)
		: CP(sprite) {}
	void cp_proc(Sprite *);
};

class Sprite_Man
	: public Sprite
{
public:
	CP_Man cp1;
	AT at1;
	ML ml1;
	CL cl1;

	Sprite_Man();
};

class CP_Cell
	: public CP
{
public:
	CP_Cell(Sprite *sprite)
		: CP(sprite) {}
	void cp_proc(Sprite *);
};

class Sprite_Cell
	: public Sprite
{
public:
	CP_Cell cp1;
	MV mv1;

	Sprite_Cell();
};

#endif //__sprites__
