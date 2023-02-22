//program: walk.cpp
//author: Jose Zamora //
//date: 02/21/2022

#ifndef _JOSE_H_
#define _JOSE_H_

#include "fonts.h"

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

#endif