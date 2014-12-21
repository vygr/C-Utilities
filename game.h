/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

#ifndef __game__
#define __game__

class Game
{
public:
	//Framework callbacks
	bool gm_init(float w, float h);
	void gm_deinit();
	void gm_reset();
	void gm_frame();
	void gm_save();
	void gm_load();
	void gm_touch_up(int x, int y);
};

#endif //__game__
