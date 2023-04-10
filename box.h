
#ifndef _BOX
#define _BOX

class Box{
	    
	    public:
	    	Box();
	        float w;
	        float yres; 
	        float xres;
	        float h;
	        float pos[2];
	        unsigned char color[3];
	        void set_color(unsigned char col[3]);
	        void set_xres(float xres);
          	void set_yres(float yres);
            void set_width(float w);
            void set_height(float h);

};

#endif