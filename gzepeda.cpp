/*
NAME: Geno Zepeda
FILE: gzepeda.cpp
ORGN: CSUB - CMPS 3350

Purpose: Draws text 


*/
#include "fonts.h"


extern void newText(int yres){
	Rect r;
	//
	r.bot = yres - 20;
	r.left = 100;
	r.center = 0;
	ggprint8b(&r, 16, 0x00ff0000, "Test");
}