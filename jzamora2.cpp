//program: walk.cpp
//author: Jose Zamora //
//date: 02/21/2022

#ifndef _JOSE_H_
#define _JOSE_H_
#include "fonts.h"

/*extern void show_map()
{
	glColor3f(0.5f, 0.5f, 0.5f);
	glBindTexture(GL_TEXTURE_2D, g.map_one)
	glBegin(GL_QUADS); 
        glTexCoord2f(0.0f, 0.0f); glVertex2i(0,      0); 
        glTexCoord2f(0.0f, 1.0f); glVertex2i(0,      g.yres); 
        glTexCoord2f(1.0f, 1.0f); glVertex2i(g.xres, g.yres); 
    glTexCoord2f(1.0f, 0.0f); glVertex2i(g.xres, 0); 
    glEnd(); 
    glBindTexture(GL_TEXTURE_2D, 0); 
} */

extern void test_text (int yres)
{
    Rect r;
    unsigned int c = 0x00ffff44;
	r.bot = yres - 50;
	r.left = 100;
	r.center = 1;
	ggprint8b(&r, 16, c, "Some Text");
    ggprint8b(&r, 16, c, "As a text");

}

// Add a background

// Add title screen

#endif