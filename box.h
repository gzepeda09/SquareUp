
#ifndef _BOX
#define _BOX

class Box {
	    
	    public:
	    	Box(float yres, float xres);
	        float w;
	        float h;
	        float pos[2];
	        unsigned char color[3];
	        void set_color(unsigned char col[3]);

};

#endif