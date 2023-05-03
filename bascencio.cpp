//Brian Ascencio
// 2/22/23
//walk.cpp

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <cstring>
//#include "image.cpp"

extern void draw_power_ups(int w, int h, float x, float y)
{
    glPushMatrix(); 
    glTranslatef(x/2, y/2, 0.0f);
    glBegin(GL_QUADS);
    glColor3ub(0, 0, 0);
    glVertex2f(w, -h);
    glVertex2f(w,  h);
    glVertex2f(w,  h);
    glVertex2f(w, -h);
    glEnd();
    glPopMatrix();
}

extern void power_up_effects()
{
}

extern void sprite(int cx, int cy, int walkFrame, GLuint walkTexture) 
{
    float h = 150.0;
    float w = h * 0.25;
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, walkTexture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255,255,255,255);
    int ix = walkFrame % 8;
    int iy = 0;
    if (walkFrame >= 8)
	iy = 1;
    float tx = (float)ix / 10.0;
    float ty = (float)iy / 10.0;
    glBegin(GL_QUADS);
    glTexCoord2f(tx,      ty+.5);   glVertex2i(cx-w, cy-h-150);
    glTexCoord2f(tx,      ty );     glVertex2i(cx-w, cy+h-150);
    glTexCoord2f(tx+.125, ty);      glVertex2i(cx+w, cy+h-150);
    glTexCoord2f(tx+.125, ty+.5);   glVertex2i(cx+w, cy-h-150);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);

}
/*
  man is walking...
        //when time is up, advance the frame.
        timers.recordTime(&timers.timeCurrent);
        double timeSpan = timers.timeDiff(&timers.walkTime, &timers.timeCurrent);
        if (timeSpan > g.delay) {
            //advance
            ++g.walkFrame;
            if (g.walkFrame >= 16)
                g.walkFrame -= 16;
            timers.recordTime(&timers.walkTime);
        }
        for (int i=0; i<20; i++) {
            g.box[i][0] -= 2.0 * (0.05 / g.delay);
            if (g.box[i][0] < -16.0)
                g.box[i][0] += g.xres + 16.0;
        }
*/
