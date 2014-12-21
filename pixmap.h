/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

#ifndef __pixmap__
#define __pixmap__

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

class Pixmap
{
private:
	GLuint _name;
	int _width;
	int _height;
	int _spwidth;
	int _spheight;
	int _maxv;
public:
	Pixmap(void *data, int w, int h, int sw, int sh);
	~Pixmap();
	void blit(float x, float y, float w, float h, int sx, int sy);
	void blitr(float x, float y, float w, float h, int sx, int sy);
	void iblit(float x, float y, float w, float h, int sx, int sy, int col);
	void getframe(int f, int &x, int &y);
};

#endif //__pixmap__
