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

extern void Player_1(int cx, int cy, int walkFrame, GLuint walkTexture) 
{
	float h = 140.0;
	float w = h * 0.6;
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, walkTexture);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glColor4ub(255,255,255,255);
	int numColumns = 8;
	int numRows = 5;
	int ix = walkFrame % numColumns;
	int iy = walkFrame / numColumns;

	float tx = (float)ix / (float)numColumns;
	float ty = (float)iy / (float)numRows;
	float tw = 1.0f / (float)numColumns;
	float th = 1.0f / (float)numRows;

	glBegin(GL_QUADS);
	glTexCoord2f(tx, ty + th); 	glVertex2i(cx - w, cy - h - 70);
	glTexCoord2f(tx, ty); 		glVertex2i(cx - w, cy + h - 70);
	glTexCoord2f(tx + tw, ty); 	glVertex2i(cx + w, cy + h - 70);
	glTexCoord2f(tx + tw, ty + th); glVertex2i(cx + w, cy - h - 70);
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);

}

extern void Player_2(int cx, int cy, int walkFrame, GLuint walkTexture) 
{
	float h = 140.0;
	float w = h * 0.6;
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, walkTexture);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glColor4ub(255,255,255,255);
	int numColumns = 8;
	int numRows = 5;
	int ix = walkFrame % numColumns;
	int iy = walkFrame / numColumns;

	float tx = (float)ix / (float)numColumns;
	float ty = (float)iy / (float)numRows;
	float tw = 1.0f / (float)numColumns;
	float th = 1.0f / (float)numRows;

	glBegin(GL_QUADS);
	glTexCoord2f(tx + tw, ty + th); 	glVertex2i(cx - w, cy - h - 70);
	glTexCoord2f(tx + tw, ty); 		glVertex2i(cx - w, cy + h - 70);
	glTexCoord2f(tx, ty); 	glVertex2i(cx + w, cy + h - 70);
	glTexCoord2f(tx, ty + th); glVertex2i(cx + w, cy - h - 70);
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
