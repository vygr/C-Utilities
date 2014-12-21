/////////////////////////////////////////////////
//  Created by Chris Hinsley.
//  Copyright (C) 2008-2014. All rights reserved.
/////////////////////////////////////////////////

#include "pixmap.h"
#include <string.h>

GLuint pixmap_current_texture = -1;

Pixmap::Pixmap(void *data, int w, int h, int sw, int sh)
{
	_width = w;
	_height = h;
	_spwidth = sw;
	_spheight = sh;
	_maxv = h / sh;
	
	glGenTextures(1, &_name);
	pixmap_current_texture = _name;
	glBindTexture(GL_TEXTURE_2D, _name);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, data);
}

Pixmap::~Pixmap()
{
	glDeleteTextures(1, &_name);
}

void Pixmap::getframe(int f, int &x, int &y)
{
	x = (f / _maxv) * _spwidth;
	y = (f % _maxv) * _spheight;
}

void Pixmap::blit(float x, float y, float w, float h, int sx, int sy)
{
	GLfixed coordinates[8];
	GLfloat vertices[8];

	vertices[0] = x;
	vertices[1] = y;
	vertices[2] = x + w;
	vertices[3] = y;
	vertices[4] = x;
	vertices[5] = y + h;
	vertices[6] = vertices[2];
	vertices[7] = vertices[5];

	coordinates[0] = (sx * 0x10000) / _width;
	coordinates[1] = (sy * 0x10000) / _height;
	coordinates[2] = ((sx + w) * 0x10000) / _width;
	coordinates[3] = coordinates[1];
	coordinates[4] = coordinates[0];
	coordinates[5] = ((sy + h) * 0x10000) / _height;
	coordinates[6] = coordinates[2];
	coordinates[7] = coordinates[5];

	if (_name != pixmap_current_texture)
	{
		pixmap_current_texture = _name;
		glBindTexture(GL_TEXTURE_2D, _name);
	}
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FIXED, 0, coordinates);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Pixmap::blitr(float x, float y, float w, float h, int sx, int sy)
{
	GLfixed coordinates[8];
	GLfloat vertices[8];

	vertices[0] = x;
	vertices[1] = y;
	vertices[2] = x + w;
	vertices[3] = y;
	vertices[4] = x;
	vertices[5] = y + h;
	vertices[6] = vertices[2];
	vertices[7] = vertices[5];

	coordinates[0] = ((sx + w) * 0x10000) / _width;
	coordinates[1] = (sy * 0x10000) / _height;
	coordinates[2] = (sx * 0x10000) / _width;
	coordinates[3] = coordinates[1];
	coordinates[4] = coordinates[0];
	coordinates[5] = ((sy + h) * 0x10000) / _height;
	coordinates[6] = coordinates[2];
	coordinates[7] = coordinates[5];

	if (_name != pixmap_current_texture)
	{
		pixmap_current_texture = _name;
		glBindTexture(GL_TEXTURE_2D, _name);
	}
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FIXED, 0, coordinates);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Pixmap::iblit(float x, float y, float w, float h, int sx, int sy, int col)
{
	glColor4ub(((col & 0xff0000) >> 16), ((col & 0xff00) >> 8), (col & 0xff), ((col >> 24) & 0xff));
	blit(x, y, w, h, sx, sy);
	glColor4ub(0xff, 0xff, 0xff, 0xff);
}
