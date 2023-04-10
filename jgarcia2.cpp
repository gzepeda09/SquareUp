//Jesse Garcia
//walk.cpp

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"



void greenBoxes(int ywin, int xwin)
{
        glPushMatrix();
        glColor3f(0.0, 5.0, 0.0);
        glBegin(GL_QUADS);
            glVertex2i(0, 0);
            glVertex2i(0, ywin);
            glVertex2i(20, ywin);
            glVertex2i(20, 0);
            
            glVertex2i(xwin-20, 0);
            glVertex2i(xwin-20, ywin);
            glVertex2i(xwin, ywin);
            glVertex2i(xwin, 0);
            
            glVertex2i(0, 0);
            glVertex2i(0, 20);
            glVertex2i(xwin, 20);
            glVertex2i(xwin, 0);
            
            glVertex2i(0, ywin-20);
            glVertex2i(0, ywin);
            glVertex2i(xwin, ywin);
            glVertex2i(xwin, ywin-20);
        glEnd();
        glPopMatrix();
}

void restartScreen(int player, int ywin, int xwin)
{
    Rect r;
    unsigned int c = 0x00ffff44;
	r.bot = ywin - 20;
	r.left = 10;
	r.center = 0;

    /*glPushMatrix();
    glTranslatef(xwin/2, ywin/2, 0.0);
    glColor3ub(0, 1, 0);
    glBegin(GL_QUADS);
        glVertex2i(-(xwin/4)-20, -20);
        glVertex2i(-(xwin/4)-20, (ywin/4)+20);
        glVertex2i((xwin/4)+20, (ywin/4)+20);
        glVertex2i((xwin/4)+20, -20);
    glEnd();
    glPopMatrix();*/

    glPushMatrix();
    glTranslatef(xwin/2, ywin/2, 0.0);
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
        glVertex2i(-xwin/4, 0);
        glVertex2i(-xwin/4, ywin/4);
        glVertex2i(xwin/4, ywin/4);
        glVertex2i(xwin/4, 0);
    glEnd();
    glPopMatrix();

    r.left += 80;
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 50, c, "PLAYER%i WINS", player);
    ggprint8b(&r, 50, c, "PRESS r TO RESTART");
    r.left -= 80;
}

