/*
NAME: Geno Zepeda
FILE: gzepeda.cpp
ORGN: CSUB - CMPS 3350

Purpose: Draws text 


*/
#include "fonts.h"
#include <GL/glx.h>

extern void newText(int yres){
	Rect r;
	//
	r.bot = yres - 20;
	r.left = 100;
	r.center = 0;

	glColor3f(1, 1, 1);
	glRasterPos2i(300, 500);
	ggprint8b(&r, 16, 0x00ff0000, "Test");

}