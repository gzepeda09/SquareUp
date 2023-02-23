//program: walk.cpp
//author: Jose Zamora //
//date: 02/21/2022

#ifndef _JOSE_H_
#define _JOSE_H_
#include "fonts.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

/*class Texture {
	public:
		Image *backimage;
		GLuint backTexture;
		float xc[2];
		float yc[2];
}; */

/*extern void map_details() {
    int w1 = g.tex.backimage->width;
	int h1 = g.tex.backimage->height;
	glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w1, h1, 0,
                GL_RGB, GL_UNSIGNED_BYTE, g.tex.backimage->data);
    g.tex.xc[0] = 0.0;
    g.tex.xc[1] = 0.25;
    g.tex.yc[0] = 0.0;
    g.tex.yc[1] = 1.0;
} */

/*extern void show_mapv1()
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

/*extern void show_mapv2() {
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
    glBegin(GL_QUADS);
        glTexCoord2f(g.tex.xc[0], g.tex.yc[1]); glVertex2i(0,      0);
        glTexCoord2f(g.tex.xc[0], g.tex.yc[0]); glVertex2i(0,      g.yres);
        glTexCoord2f(g.tex.xc[1], g.tex.yc[0]); glVertex2i(g.xres, g.yres);
        glTexCoord2f(g.tex.xc[1], g.tex.yc[1]); glVertex2i(g.xres, 0);
    glEnd(); 
}*/

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

/*extern void display_map_one(float xc1[0], float xc2[0], float yc1[0], float yc2[0], int xres, int yres) {
    glBegin(GL_QUADS);
        glTexCoord2f(xc1[0], yc2[0]); glVertex2i(0,      -150);
        glTexCoord2f(xc1[0], yc1[0]); glVertex2i(0,      yres*2);
        glTexCoord2f(xc2[0], yc1[0]); glVertex2i(xres,   yres*2);
        glTexCoord2f(xc2[0], yc2[1]); glVertex2i(xres,   -150);
    glEnd(); 
}*/
// Add a background

// Add title screen

#endif