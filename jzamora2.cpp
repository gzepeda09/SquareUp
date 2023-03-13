//program: walk.cpp
//author: Jose Zamora //
//date: 02/21/2022

#ifndef _JOSE_H_
#define _JOSE_H_
#include "fonts.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <cstring>

/*class Texture {
	public:
		Image *backimage;
		GLuint backTexture;
		float xc[2];
		float yc[2];
}; */

const int numbox=5;
const int MAX_PARTICLES=150;

class player1 {
   public:
   float w, h;
   float pos[2];
   float vel[2];
   unsigned char color[3];
   void set_color(unsigned char col[3]) {
       memcpy(color, col, sizeof(unsigned char) * 3);
   }

   player1() {
       w = 50.0f;       // 80.0f
       h = 12.0f;       // 20.0f
       pos[0] = 1920/2;
       pos[1] = 1080/2;
       vel[0] = 0;
       vel[1] = 0;
   }

   player1(float wid, float hgt, int x, int y, float v0, float v1) {
       w = wid;
       h = hgt;
       pos[0] = x;
       pos[1] = y;
       vel[0] = v0;
       vel[1] = v1;
  }

} player[numbox], playerparticle[MAX_PARTICLES];

extern void test_text (int gres, int yres)
{
    Rect r;
    unsigned int c = 0x00ffff44;
	r.bot = yres - 50;
	r.left = 100;
	r.center = 1;
	ggprint8b(&r, 16, c, "Some Text");
    ggprint8b(&r, 16, c, "As a text");

}

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

extern void display_controls(int wf, int yres) {
    Rect r;
    unsigned int c = 0x00ffff44;
	r.bot = yres - 20;
	r.left = 10;
	r.center = 0;
	ggprint8b(&r, 16, c, "Player 1: ");
	ggprint8b(&r, 16, c, "a - move left");
	ggprint8b(&r, 16, c, "d - move rightr");
	ggprint8b(&r, 16, c, "Player 2: ");
	ggprint8b(&r, 16, c, "left arrow - move left");
	ggprint8b(&r, 16, c, "right arrow - move right");
	ggprint8b(&r, 16, c, "frame: %i", wf);
}

// Add a background
extern void display_map_one(float x0, float x1,
                            float y0, float y1,
                            int xres, int yres) {
    glBegin(GL_QUADS);
        glTexCoord2f(x0, y1); 	glVertex2i(0,       0);
        glTexCoord2f(x0, y0); 	glVertex2i(0,       yres);
        glTexCoord2f(x1, y0); 	glVertex2i(xres*1.2,    yres);
        glTexCoord2f(x1, y1); 	glVertex2i(xres*1.2,    0);
    glEnd();
}

extern void player_1(int x, int y) {
    //
    glPushMatrix();
    glColor3ub(150, 160, 220);
    //glColor3ubv(box[i].color);
    glTranslatef(player[0].pos[0], player[0].pos[1], 0.0f);   // 200 ; 100
    glBegin(GL_QUADS);
        glVertex2f(-player[0].w, -player[0].h);
        glVertex2f(-player[0].w,  player[0].h);
        glVertex2f( player[0].w,  player[0].h);
        glVertex2f( player[0].w, -player[0].h);
    glEnd();
    glPopMatrix();
}

// Add title screen

#endif
