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

extern void fmBorder(int xres, int yres) {
    int t = 40;
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(0,           0);
        glVertex2f(t,           t);
        glVertex2f(0,           yres);
        glVertex2f(t,           yres - t);
        glVertex2f(xres,     	yres);
        glVertex2f(xres - t, 	yres - t);
        glVertex2f(xres,     	0);
        glVertex2f(xres - t, 	t);
        glVertex2f(0,           0);
        glVertex2f(t,           t);
    glEnd();
}

extern void test_text (int xres, int yres)
{
    Rect r;
    //unsigned int c = 0x00ffff44;
	r.bot = yres - 20;
    r.left = xres / 2;
    r.center = 1;
    ggprint8b(&r, 16, 0x00ff0000, "Jose's Feature Mode");

}

extern void display_controls(int wf, int yres) {
    Rect r;
    unsigned int c = 0x00ffff44;
	r.bot = yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, c, "W   Walk cycle");
	ggprint8b(&r, 16, c, "+   faster");
	ggprint8b(&r, 16, c, "-   slower");
	ggprint8b(&r, 16, c, "right arrow -> walk right");
	ggprint8b(&r, 16, c, "left arrow  <- walk left");
	ggprint8b(&r, 16, c, "frame: %i", wf);
}

// Add a background
extern void display_map_one(float x0, float x1, 
                            float y0, float y1, 
                            int xres, int yres) {
    glBegin(GL_QUADS);
        glTexCoord2f(x0, y1+1.5); 		glVertex2i(0,       -357);
        glTexCoord2f(x0, y0); 			glVertex2i(0,       yres*2);
        glTexCoord2f(x1+.375, y0); 		glVertex2i(xres,    yres*2);
        glTexCoord2f(x1+.375, y1+1.5); 	glVertex2i(xres,    -357);
    glEnd(); 
}

// Add title screen

#endif