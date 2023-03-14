/*
NAME: Geno Zepeda
FILE: gzepeda.cpp
ORGN: CSUB - CMPS 3350

Purpose: Draws text 


*/
#include "fonts.h"
#include <GL/glx.h>
#include <GL/glut.h>
#include <cstring>



extern void newText(int yres, int xres){




	class Box {
	    public:
	        float w;
	        float h;
	        float pos[2];
	        float vel[2];
	        unsigned char color[3];
	        void set_color(unsigned char col[3]) {
	            memcpy(color, col, sizeof(unsigned char) * 3);
	        }

	        Box(float yres, float xres) {
	            w = 80.0f;
	            h = 20.5f;
	            pos[0] = xres/2;
	            pos[1] = yres/2;
	            vel[0] = 0.0;
	            vel[1] = 0.0;
	        }

	};

	float sxres = xres - 0.5f;

	Box box(yres , sxres);
	Box box2(yres + 1.0f, sxres);
	Box box3(yres - 0.5f, sxres);


	Box border(yres - 0.5f, sxres);

	unsigned char c[3] = {176, 172, 172};
	unsigned char c2[3] = {33, 156, 14};
	unsigned char c3[3] = {184, 2, 2};


	unsigned char bc[3] = {255, 255, 255};


	Rect r;
    glClear(GL_COLOR_BUFFER_BIT);


    r.bot = yres - box3.pos[1];
    r.left = box3.pos[0];
    r.center = 0;





    box.set_color(c);
	box2.set_color(c2);
	box3.set_color(c3);
	border.set_color(bc);

    
	   //Draw box.
	glPushMatrix();
	glColor3ubv(box.color);
	glTranslatef(box.pos[0], box.pos[1], 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(-box.w, -box.h);
	glVertex2f(-box.w,  box.h);
	glVertex2f( box.w,  box.h);
	glVertex2f( box.w, -box.h);
	glEnd();
	glPopMatrix();




	//Draw border.
	glPushMatrix();
	glColor3ubv(border.color);
	glTranslatef(border.pos[0], border.pos[1], 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(-border.w + 20, -border.h + 20);
	glVertex2f(-border.w + 20,  border.h + 20);
	glVertex2f( border.w + 20,  border.h + 20);
	glVertex2f( border.w + 20, -border.h + 20);
	glEnd();
	glPopMatrix();



	//Draw box.
	glPushMatrix();
	glColor3ubv(box2.color);
	glTranslatef(box2.pos[0], box2.pos[1], 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(-box2.w, -box2.h);
	glVertex2f(-box2.w,  box2.h);
	glVertex2f( box2.w,  box2.h);
	glVertex2f( box2.w, -box2.h);
	glEnd();
	glPopMatrix();


	ggprint8b(&r, 16, 0x000000, "Start");

	//Draw box.
	glPushMatrix();
	glColor3ubv(box3.color);
	glTranslatef(box3.pos[0], box3.pos[1], 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(-box3.w, -box3.h);
	glVertex2f(-box3.w,  box3.h);
	glVertex2f( box3.w,  box3.h);
	glVertex2f( box3.w, -box3.h);
	glEnd();
	glPopMatrix();


}