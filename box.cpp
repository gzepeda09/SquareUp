#include "box.h"
#include <cstring>


Box::Box(float yres, float xres) {
    w = 80.0f;
    h = 20.5f;
    pos[0] = xres/2;
    pos[1] = yres/2;
}

void  Box::set_color(unsigned char col[3]) {
	
	memcpy(color, col, sizeof(unsigned char) * 3);
	        
}
