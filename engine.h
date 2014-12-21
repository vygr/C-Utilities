/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

#ifndef __engine__
#define __engine__

#include "list.h"
#include "pixmap.h"

//colours

#define RGBBLACK   0x000000
#define RGBWHITE   0xffffff
#define RGBRED     0xff0000
#define RGBGREEN   0x00ff00
#define RGBBLUE    0x0000ff
#define RGBYELLOW  0xffff00
#define RGBMAGENTA 0xff00ff
#define RGBCYAN    0x00ffff

#define RGBDWHITE   0x7f7f7f
#define RGBDRED     0x7f0000
#define RGBDGREEN   0x007f00
#define RGBDBLUE    0x00007f
#define RGBDYELLOW  0x7f7f00
#define RGBDMAGENTA 0x7f007f
#define RGBDCYAN    0x007f7f

#define RGBPWHITE   0xffffff
#define RGBPRED     0xff8080
#define RGBPGREEN   0x80ff80
#define RGBPBLUE    0x8080ff
#define RGBPYELLOW  0xffff80
#define RGBPMAGENTA 0xff80ff
#define RGBPCYAN    0x80ffff

#define RGBGREY1 0x222222
#define RGBGREY2 0x444444
#define RGBGREY3 0x666666
#define RGBGREY4 0x888888
#define RGBGREY5 0xAAAAAA
#define RGBGREY6 0xCCCCCC

#define RGBMASK 0xFFFFFF

//sprite class

#define SPRITE_IS_DEAD -1
#define killsprite(a) a->_sprite_frame = SPRITE_IS_DEAD

class Sprite;
typedef void (*death_function)(Sprite *);

class Sprite
	: public Listnode
{
public:
	float _sprite_x;
	float _sprite_y;
	float _sprite_w;
	float _sprite_h;
	float _sprite_radius;
	int _sprite_frame;
	int _sprite_id;
	int _sprite_type;
	int _sprite_hitpnt;
	void (*_sprite_death)(Sprite *);
	Pixmap *_sprite_pixmap;
	int _sprite_user1;
	int _sprite_user2;
	int _sprite_user3;
	Listhead _sprite_cp_list;

	Sprite(float w, float h, int frame, int type, int hitpt, death_function death, Pixmap *pix, int u1, int u2, int u3);
	void sp_add_cp(Listnode *cp);
	virtual void sp_draw();
};

//sprite component

class CP
	: public Listnode
{
public:
	CP(Sprite *sprite);
	virtual void cp_proc(Sprite *) = 0;
};

//animation table component

class AT
	: public CP
{
public:
	int _at_speed;
	int _at_count;
	int _at_index;
	int _at_length;
	int *_at_table;

	AT(Sprite *sprite, int speed, int *table, int size);
	void cp_proc(Sprite *sprite);
};

//movement table component

class MT
	: public CP
{
public:
	int _mt_speed;
	int _mt_count;
	int _mt_index;
	int _mt_length;
	int *_mt_table;
	Sprite *_mt_track;

	MT(Sprite *sprite, int speed, int *table, int size);
	void cp_proc(Sprite *sprite);
};

//movement vectors component

class MV
	: public CP
{
public:
	float _mv_xvel;
	float _mv_yvel;
	float _mv_xacc;
	float _mv_yacc;
	float _mv_maxpx;
	float _mv_maxnx;
	float _mv_maxpy;
	float _mv_maxny;

	MV(Sprite *sprite, float xv, float yv, float xacc, float yacc, float maxxv, float maxyv);
	void cp_proc(Sprite *sprite);
};

//movement line component

class ML
	: public CP
{
public:
	float _ml_speed;
	float _ml_dx;
	float _ml_dy;
	int _ml_count;

	ML(Sprite *sprite, float speed);
	void ml_setline(Sprite *sprite, float x, float y, float x1, float y1);
	void cp_proc(Sprite *sprite);
};

//collision component

typedef void (*collision_function)(Sprite *, Sprite *);

class CL
	: public CP
{
public:
	Listhead *_cl_list;
	int _cl_type;
	collision_function *_cl_table;

	CL(Sprite *sprite, Listhead *list, int type, collision_function *table);
	void cp_proc(Sprite *sprite);
};

#endif //__engine__
