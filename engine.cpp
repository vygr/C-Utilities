/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

#include "engine.h"
#include <math.h>

//sprite class

Sprite::Sprite(float w, float h, int frame, int type, int hitpt, death_function death, Pixmap *pix, int u1, int u2, int u3)
	: Listnode(this)
	, _sprite_x(0)
	, _sprite_y(0)
	, _sprite_w(w)
	, _sprite_h(h)
	, _sprite_radius((w + h) / 4)
	, _sprite_frame(frame)
	, _sprite_type(type)
	, _sprite_hitpnt(hitpt)
	, _sprite_death(death)
	, _sprite_pixmap(pix)
	, _sprite_user1(u1)
	, _sprite_user2(u2)
	, _sprite_user3(u3)
{
	static int next_id = 0;
	_sprite_id = next_id++;
}

void Sprite::sp_add_cp(Listnode *cp)
{
	_sprite_cp_list.add_at_tail(cp);
}

void Sprite::sp_draw()
{
	Pixmap *pix = this->_sprite_pixmap;
	if (pix)
	{
		int sx, sy;
		pix->getframe(this->_sprite_frame, sx, sy);
		pix->blit(this->_sprite_x, this->_sprite_y, this->_sprite_w, this->_sprite_h, sx, sy);
	}
}

//sprite component

CP::CP(Sprite *sprite)
	: Listnode(this)
{
	sprite->sp_add_cp(this);
}

//animation table component

AT::AT(Sprite *sprite, int speed, int *table, int size)
	: CP(sprite)
	, _at_speed(speed)
	, _at_count(speed)
	, _at_index(0)
	, _at_length(size / sizeof(int))
	, _at_table(table)
{}

void AT::cp_proc(Sprite *sprite)
{
	int *table;
	int speed, count, index;

	speed = _at_speed;
	if (speed)
	{
		count = _at_count;
		count++;
		if (count >= speed)
		{
			table = _at_table;
			index = _at_index;
			sprite->_sprite_frame = table[index];
			index++;
			if (index >= _at_length) { index = 0; }
			_at_index = index;
			count = 0;
		}
		_at_count = count;
	}
}

//movement table component

MT::MT(Sprite *sprite, int speed, int *table, int size)
	: CP(sprite)
	, _mt_speed(speed)
	, _mt_count(speed)
	, _mt_index(0)
	, _mt_length(size / sizeof(int))
	, _mt_table(table)
	, _mt_track(0)
{}

void MT::cp_proc(Sprite *sprite)
{
	int *table;
	Sprite *track;
	int speed, count, index, x, y;

	speed = _mt_speed;
	if (speed)
	{
		count = _mt_count;
		count++;
		if (count >= speed)
		{
			track = _mt_track;
			if (!track) { track = sprite; }
			table = _mt_table;
			index = _mt_index;
			x = track->_sprite_x;
			y = track->_sprite_y;
			x += table[index];
			y += table[index + 1];
			sprite->_sprite_x = x;
			sprite->_sprite_y = y;
			index += 2;
			if (index >= _mt_length) { index = 0; }
			_mt_index = index;
			count = 0;
		}
		_mt_count = count;
	}
}

//movement vectors component

MV::MV(Sprite *sprite, float xv, float yv, float xacc, float yacc, float maxxv, float maxyv)
	: CP(sprite)
	, _mv_xvel(xv)
	, _mv_yvel(yv)
	, _mv_xacc(xacc)
	, _mv_yacc(yacc)
	, _mv_maxpx(maxxv)
	, _mv_maxnx(-maxxv)
	, _mv_maxpy(maxyv)
	, _mv_maxny(-maxyv)
{}

void MV::cp_proc(Sprite *sprite)
{
	float xvel, xmax, yvel, ymax;

	xvel = _mv_xvel;
	xvel += _mv_xacc;
	if (xvel > 0)
	{
		xmax = _mv_maxpx;
		if (xvel > xmax) { xvel = xmax; }
	}
	else
	{
		xmax = _mv_maxnx;
		if (xvel < xmax) { xvel = xmax; }
	}
	_mv_xvel = xvel;
	sprite->_sprite_x += xvel;

	yvel = _mv_yvel;
	yvel += _mv_yacc;
	if (yvel > 0)
	{
		ymax = _mv_maxpy;
		if (yvel > ymax) { yvel = ymax; }
	}
	else
	{
		ymax = _mv_maxny;
		if (yvel < ymax) { yvel = ymax; }
	}
	_mv_yvel = yvel;
	sprite->_sprite_y += yvel;
}

//movement line component

ML::ML(Sprite *sprite, float speed)
	: CP(sprite)
	, _ml_speed(speed)
	, _ml_count(0)
	, _ml_dx(0)
	, _ml_dy(0)
{}

void ML::ml_setline(Sprite *sprite, float x, float y, float x1, float y1)
{
	sprite->_sprite_x = x;
	sprite->_sprite_y = y;
	float xd = (x1 - x);
	float yd = (y1 - y);
	float td = sqrt((xd * xd) + (yd * yd));
	_ml_count = (td / _ml_speed);
	if (!_ml_count) { _ml_count = 1; }
	_ml_dx = xd / _ml_count;
	_ml_dy = yd / _ml_count;
}

void ML::cp_proc(Sprite *sprite)
{
	if (_ml_count > 0)
	{
		sprite->_sprite_x += _ml_dx;
		sprite->_sprite_y += _ml_dy;
		_ml_count--;
	}
}

//colision component

CL::CL(Sprite *sprite, Listhead *list, int type, collision_function *table)
	: CP(sprite)
	, _cl_list(list)
	, _cl_type(type)
	, _cl_table(table)
{}

Sprite *sprite_collide(Listhead *list, float x, float y, float w, float h, int type)
{
	Sprite *node, *next = (Sprite *)list->get_tail();

	w += x;
	h += y;
	for (;;)
	{
		node = next;
		next = (Sprite *)next->get_pred();
		if (!next) { return 0; }
		if (node->_sprite_frame == SPRITE_IS_DEAD) { continue; }
		if (!(node->_sprite_type & type)) { continue; }
		if (node->_sprite_x >= w) { continue; }
		if (node->_sprite_y >= h) { continue; }
		if (node->_sprite_x + node->_sprite_w <= x) { continue; }
		if (node->_sprite_y + node->_sprite_h <= y) { continue; }
		return node;
	}
}

void CL::cp_proc(Sprite *sprite)
{
	collision_function *table;
	collision_function code;
	int type;
	Sprite *hit;

	type = _cl_type;
	hit = sprite_collide(_cl_list, sprite->_sprite_x, sprite->_sprite_y, sprite->_sprite_w, sprite->_sprite_h, type);
	if (hit)
	{
		//call collision code for type
		table = _cl_table;
		type = hit->_sprite_type;
		while (!(type & 1))
		{
			type = type >> 1;
			table++;
		}
		code = *table;
		if (code) { (*code)(sprite, hit); }
	}
}

//sprite list processing

Listnode *sprite_proc_cb(Listhead *list, Listnode *node, void *user)
{
	Sprite *sprite = (Sprite *)node;
	CP *cp, *next = (CP *)sprite->_sprite_cp_list.get_head();
	death_function death;

	for (;;)
	{
		if (sprite->_sprite_frame == SPRITE_IS_DEAD)
		{
			//death processing
			sprite->remove_node();
			death = sprite->_sprite_death;
			if (death) { (*death)(sprite); }
			delete sprite;
			break;
		}
		cp = next;
		next = (CP *)next->get_succ();
		if (!next) { break; }
		cp->cp_proc(sprite);
	}
	return 0;
}

void sprite_proc_list(Listhead *list)
{
	list->enumerate_forwards(sprite_proc_cb, 0);
}

//sprite list drawing

Listnode *sprite_draw_cb(Listhead *list, Listnode *node, void *user)
{
	Sprite *sprite = (Sprite *)node;

	if (sprite->_sprite_frame >= 0) { sprite->sp_draw(); }
	return 0;
}

void sprite_draw_list(Listhead *list)
{
	list->enumerate_forwards(sprite_draw_cb, 0);
}

//sprite freeing and killing

Listnode *sprite_free_cb(Listhead *list, Listnode *node, void *user)
{
	node->remove_node();
	delete node;
	return 0;
}

void sprite_free_list(Listhead *list)
{
	list->enumerate_forwards(sprite_free_cb, 0);
}

Listnode *sprite_kill_cb(Listhead *list, Listnode *node, void *user)
{
	Sprite *sprite = (Sprite *)node;
	if ((sprite->_sprite_id) == (int)user) { killsprite (sprite); }
	return 0;
}

void sprite_kill_list_id(Listhead *list, int sid)
{
	list->enumerate_forwards(sprite_kill_cb, (void *)sid);
}

Listnode *sprite_kill_cb1(Listhead *list, Listnode *node, void *user)
{
	Sprite *sprite = (Sprite *)node;
	killsprite (sprite);
	return 0;
}

void sprite_kill_list(Listhead *list)
{
	list->enumerate_forwards(sprite_kill_cb1, 0);
}

Listnode *sprite_find_list_types_cb(Listhead *list, Listnode *node, void *user)
{
	Sprite *sprite = (Sprite *)node;
	if (sprite->_sprite_type & (int)user) { return sprite; }
	return 0;
}

Sprite *sprite_find_list_types(Listhead *list, int types)
{
	return (Sprite *)list->enumerate_forwards(sprite_find_list_types_cb, (void *)types);
}

//text drawing

char *itoa(int val, int base)
{
	static char buf[32] =
	{
		0
	};
	int i = 30;

	if (!val)
	{
		buf [i--] = '0';
	}
	else
	{
		for (; val && i ; --i, val /= base)
		{
			buf[i] = "0123456789abcdef"[val % base];
		}
	}

	return &buf[i + 1];
}

//utils

unsigned int GLOBALS_GAME_SEED = 0;

int random(int range)
{
	unsigned int seed = GLOBALS_GAME_SEED;
	seed *= 17;
	seed ^= 0xa5a5a5a5;
	GLOBALS_GAME_SEED = seed;
	seed = seed >> 16;
	range *= seed;
	return range >> 16;
}
