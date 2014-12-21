/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

//#define WANT_SAVEDEMO

#include "game.h"
#include "engine.h"
#include "sprites.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

//platform provided external functions

extern FILE *openbundlefile(const char *, const char *);
extern FILE *openappdatafile(const char *, const char *, const char *);
extern void loadsound(int num, const char *);
extern Pixmap *loadimage(const char *, int sw, int sh);

//engine functions

extern void sprite_draw_list(Listhead *);
extern void sprite_kill_list(Listhead *);
extern void sprite_kill_list_id(Listhead *, int);
extern Sprite *sprite_find_list_types(Listhead *, int);
extern void sprite_free_list(Listhead *);
extern void sprite_proc_list(Listhead *);
extern Sprite *sprite_collide(Listhead *, int, int, int, int, int);
extern void sprite_draw(Sprite *);
extern int random(int);

//prototypes

void game_free_dlists();
void game_proc_dlists();
void game_setstate_title();
void game_frame_title();
void game_setstate_menu();
void game_frame_menu();
void game_setstate_demo();
void game_frame_demo();

//globals

float SCREEN_WIDTH = 640;
float SCREEN_HEIGHT = 960;

Pixmap *GLOBALS_PIXMAP_MAN;

Listhead GLOBAL_DLIST_MAN;
Listhead GLOBAL_DLIST_CELLS;
Listhead GLOBAL_DLIST_FX;

int GLOBALS_GAME_FRAME_COUNT = 0;
int GLOBALS_GAME_STATE = 0;
int GLOBALS_GAME_STATE_LAST = 0;
int GLOBALS_GAME_STATE_NEXT = 0;

//sprite types

enum
{
	BFTP_MAN,
	BFTP_CELL,
};
#define FTP_MAN  (1 << BFTP_MAN)
#define FTP_CELL (1 << BFTP_CELL)

//game states

enum
{
	GAME_STATE_TITLE,
	GAME_STATE_MENU,
	GAME_STATE_DEMO
};

//death handlers

void dt_man(Sprite *sprite)
{}

void dt_enemy(Sprite *sprite)
{}

//collision handlers

void cl_man_hits_cell(Sprite *s, Sprite *s1)
{
	Sprite_Cell *cell = (Sprite_Cell *)s1;

	cell->_sprite_x = random(SCREEN_WIDTH - cell->_sprite_w);
	cell->_sprite_y = random(SCREEN_HEIGHT - cell->_sprite_h);
	cell->mv1._mv_xvel = 0;
	cell->mv1._mv_yvel = 0;
	cell->mv1._mv_xacc = 0;
	cell->mv1._mv_yacc = 0;
}

//collision tables

collision_function collision_table_man[] =
{
	0,
	cl_man_hits_cell,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

//animation tables

int at_man[] =
{
	0, 1, 2
};

//sprite class methods

Sprite_Man::Sprite_Man()
	: Sprite(16, 16, 0, FTP_MAN, 0, dt_man, GLOBALS_PIXMAP_MAN, 0, 0, 0)
	, cp1(this)
	, at1(this, 3, at_man, sizeof(at_man))
	, ml1(this, 0.25)
	, cl1(this, &GLOBAL_DLIST_CELLS, -1, collision_table_man)
{}

void CP_Man::cp_proc(Sprite *sprite)
{
	Sprite_Man *man = (Sprite_Man *)sprite;

	if (man->ml1._ml_count <= 0)
	{
		int x = random(SCREEN_WIDTH - man->_sprite_w);
		int y = random(SCREEN_HEIGHT - man->_sprite_h);
		man->ml1.ml_setline(man, man->_sprite_x, man->_sprite_y, x, y);
	}
}

Sprite_Cell::Sprite_Cell()
	: Sprite(16, 16, 1, FTP_CELL, 0, dt_enemy, GLOBALS_PIXMAP_MAN, 1, 0, 0)
	, cp1(this)
	, mv1(this, 0, 0, 0, 0, 16, 16)
{}

#define GRAV_CONST   0.5 //(6.67428E-11)
#define MIN_DISTANCE 0.000001
#define EDGE_DAMPING 0.25

void CP_Cell::cp_proc(Sprite *sprite)
{
	Sprite_Cell *cell = (Sprite_Cell *)sprite;
	Sprite_Cell *node, *next = cell;

	for (;;)
	{
		node = next;
		next = (Sprite_Cell *)next->get_succ();
		if (!next) { break; }
		if (node->_sprite_frame == SPRITE_IS_DEAD) { continue; }

		float xd = cell->_sprite_x - node->_sprite_x;
		float yd = cell->_sprite_y - node->_sprite_y;
		float d2 = (xd * xd) + (yd * yd);
		if (d2 < MIN_DISTANCE) { d2 = MIN_DISTANCE; }
		float d = sqrt(d2);
		xd /= d;
		yd /= d;
		if (d < (cell->_sprite_radius + node->_sprite_radius)) { d2 *= -1.5; }
		float force = (((cell->_sprite_user1 * node->_sprite_user1) / d2) * GRAV_CONST);
		xd *= force;
		yd *= force;
		cell->mv1._mv_xacc -= xd;
		cell->mv1._mv_yacc -= yd;
		node->mv1._mv_xacc += xd;
		node->mv1._mv_yacc += yd;
	}
	cell->mv1._mv_xacc /= cell->_sprite_user1;
	cell->mv1._mv_yacc /= cell->_sprite_user1;
}

//cell reseting

Listnode *cell_reset_cb(Listhead *list, Listnode *node, void *user)
{
	Sprite_Cell *cell = (Sprite_Cell *)node;

	cell->mv1._mv_xacc = 0;
	cell->mv1._mv_yacc = 0;

	if (cell->_sprite_x < 0)
	{
		cell->_sprite_x = 0;
		cell->mv1._mv_xvel = -cell->mv1._mv_xvel * EDGE_DAMPING;
	}
	else if (cell->_sprite_x > (SCREEN_WIDTH - cell->_sprite_w))
	{
		cell->_sprite_x = (SCREEN_WIDTH - cell->_sprite_w);
		cell->mv1._mv_xvel = -cell->mv1._mv_xvel * EDGE_DAMPING;
	}
	if (cell->_sprite_y < 0)
	{
		cell->_sprite_y = 0;
		cell->mv1._mv_yvel = -cell->mv1._mv_yvel * EDGE_DAMPING;
	}
	else if (cell->_sprite_y > (SCREEN_HEIGHT - cell->_sprite_h))
	{
		cell->_sprite_y = (SCREEN_HEIGHT - cell->_sprite_h);
		cell->mv1._mv_yvel = -cell->mv1._mv_yvel * EDGE_DAMPING;
	}
	return 0;
}

void cell_reset_list()
{
	GLOBAL_DLIST_CELLS.enumerate_forwards(cell_reset_cb, 0);
}

//sprite list processing

void game_proc_dlists()
{
	//process sprites
	sprite_proc_list(&GLOBAL_DLIST_MAN);
	sprite_proc_list(&GLOBAL_DLIST_CELLS);
	sprite_proc_list(&GLOBAL_DLIST_FX);
}

void game_free_dlists()
{
	//process sprites
	sprite_free_list(&GLOBAL_DLIST_MAN);
	sprite_free_list(&GLOBAL_DLIST_CELLS);
	sprite_free_list(&GLOBAL_DLIST_FX);
}

//game state setting and frame functions

void game_setstate_title()
{
	/*	Sprite_Man *man = new Sprite_Man();
		if (man)
		{
			int x = random(SCREEN_WIDTH - man->_sprite_w);
			int y = random(SCREEN_HEIGHT - man->_sprite_h);
			int x1 = random(SCREEN_WIDTH - man->_sprite_w);
			int y1 = random(SCREEN_HEIGHT - man->_sprite_h);
			man->ml1.ml_setline(man, x, y, x1, y1);
			GLOBAL_DLIST_MAN.add_at_head(man);
		}
	 */
	for (int i = 0; i < 10; i++)
	{
		Sprite_Cell *cell = new Sprite_Cell();
		if (cell)
		{
			cell->_sprite_x = random(SCREEN_WIDTH - cell->_sprite_w);
			cell->_sprite_y = random(SCREEN_HEIGHT - cell->_sprite_h);
			GLOBAL_DLIST_CELLS.add_at_tail(cell);
		}
	}

	for (int i = 0; i < 5; i++)
	{
		Sprite_Cell *cell = new Sprite_Cell();
		if (cell)
		{
			cell->_sprite_x = random(SCREEN_WIDTH - cell->_sprite_w);
			cell->_sprite_y = random(SCREEN_HEIGHT - cell->_sprite_h);
			cell->_sprite_frame = 2;
			cell->_sprite_user1 = 25;
			GLOBAL_DLIST_CELLS.add_at_tail(cell);
		}
	}

	for (int i = 0; i < 1; i++)
	{
		Sprite_Cell *cell = new Sprite_Cell();
		if (cell)
		{
			cell->_sprite_x = random(SCREEN_WIDTH - cell->_sprite_w);
			cell->_sprite_y = random(SCREEN_HEIGHT - cell->_sprite_h);
			cell->_sprite_frame = 0;
			cell->_sprite_user1 = 100;
			GLOBAL_DLIST_CELLS.add_at_tail(cell);
		}
	}
}

void game_frame_title()
{}

void game_setstate_menu()
{}

void game_frame_menu()
{}

void game_setstate_demo()
{}

void game_frame_demo()
{}

//game state function tables
typedef void (*frame_function)();
frame_function init_state_table[] =
{
	game_setstate_title,
	game_setstate_menu,
	game_setstate_demo
};

frame_function frame_state_table[] =
{
	game_frame_title,
	game_frame_menu,
	game_frame_demo
};

//game methods

bool Game::gm_init(float w, float h)
{
    SCREEN_WIDTH = w;
    SCREEN_HEIGHT = h;

	//Initialize OpenGL states
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	//Set up screen projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1, 1);

	GLOBALS_GAME_STATE = GAME_STATE_TITLE;
	GLOBALS_GAME_STATE_LAST = -1;
	GLOBALS_GAME_STATE_NEXT = GAME_STATE_MENU;

	//load sound buffers
	//loadsound(SOUND_EXPLODE,"explode");

	//load images
	GLOBALS_PIXMAP_MAN = loadimage("16x16", 16, 16);

	return true;
}

void Game::gm_deinit()
{
	game_free_dlists();

	//release images
	delete GLOBALS_PIXMAP_MAN;
}

void Game::gm_reset()
{
	//reset back to menu
	GLOBALS_GAME_STATE_NEXT = GAME_STATE_MENU;
	GLOBALS_GAME_STATE = GAME_STATE_TITLE;
}

void Game::gm_frame()
{
	//game state management
	do
	{
		if (GLOBALS_GAME_STATE != GLOBALS_GAME_STATE_LAST)
		{
			GLOBALS_GAME_STATE_LAST = GLOBALS_GAME_STATE;
			//init this state
			(*init_state_table[GLOBALS_GAME_STATE])();
		}
		//frame of this state
		(*frame_state_table[GLOBALS_GAME_STATE])();
	} while (GLOBALS_GAME_STATE != GLOBALS_GAME_STATE_LAST);

	//reset accelearations
	cell_reset_list();

	//process sprites
	game_proc_dlists();

	//clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw sprites
	sprite_draw_list(&GLOBAL_DLIST_CELLS);
	sprite_draw_list(&GLOBAL_DLIST_MAN);
	sprite_draw_list(&GLOBAL_DLIST_FX);

	//next frame
	GLOBALS_GAME_FRAME_COUNT++;
}

void Game::gm_save()
{}

void Game::gm_load()
{}

void Game::gm_touch_up(int x, int y)
{
	Sprite_Cell *cell = new Sprite_Cell();
	if (cell)
	{
		cell->_sprite_x = x - cell->_sprite_radius;
		cell->_sprite_y = y - cell->_sprite_radius;
		GLOBAL_DLIST_CELLS.add_at_head(cell);
	}
}
