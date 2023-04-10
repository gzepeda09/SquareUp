//program: jzamora2.cpp || Source file for project
//author: Jose Zamora //
//date: 02/21/2022

#ifndef _JOSE_H_
#define _JOSE_H_
#include "fonts.h"
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <cstring>

extern void test_text (int gres, int yres)
{
    Rect r;
    unsigned int c = 0x00ffff44;
        r.bot = yres - 50;
        r.left = 100;
        r.center = 1;
        ggprint8b(&r, 16, c, "Jose's Feature Mode");
    ggprint8b(&r, 16, c, "As a text");
}

extern void fmBorder(int xres, int yres) 
{
    int t = 10;
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(0,           0);
        glVertex2f(t,           t);
        glVertex2f(0,           yres);
        glVertex2f(t,           yres - t);
        glVertex2f(xres,        yres);
        glVertex2f(xres - t,    yres - t);
        glVertex2f(xres,        0);
        glVertex2f(xres - t,    t);
        glVertex2f(0,           0);
        glVertex2f(t,           t);
    glEnd();
}

extern void display_controls(int wf, int yres) 
{
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
extern void display_map(float x0, float x1, float y0, float y1, int xres, int yres) 
{
    glBegin(GL_QUADS);
        glTexCoord2f(x0, y1);   glVertex2i(0,           0);
        glTexCoord2f(x0, y0);   glVertex2i(0,           yres);
        glTexCoord2f(x1, y0);   glVertex2i(xres*1.2,    yres);
        glTexCoord2f(x1, y1);   glVertex2i(xres*1.2,    0);
    glEnd();
}

// Makes boxes for players
extern void player_hitbox(int w, int h, float x, float y) 
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glBegin(GL_QUADS);
        glColor3ub(0, 0, 0);
        glVertex2f(-w,  -h);
        glVertex2f(-w,   h);
        glVertex2f( w,   h);
        glVertex2f( w,  -h);
    glEnd();
    glPopMatrix();
}

extern void punch_hitbox(int w1, int w2, int h, float x, float y) 
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glBegin(GL_QUADS);
        glColor3ub(0, 150, 0);
        glVertex2f(-w2,  -h);
        glVertex2f(-w2,   h);
        glVertex2f( w1,   h);
        glVertex2f( w1,  -h);
    glEnd();
    glPopMatrix();
}

#endif
