#include "box.h"
#include <cstring>


Box::Box() {

}

void Box::set_xres(float xres){
    this->xres = xres;
    pos[0] = xres/2;
}

void Box::set_yres(float yres){
    this->yres = yres;
    pos[1] = yres/2;
}

void Box::set_width(float w){
    this->w = w;
}

void Box::set_height(float h){
    this->h = h;
}

void  Box::set_color(unsigned char col[3]) {
	
	memcpy(color, col, sizeof(unsigned char) * 3);
	        
}
