//3350
//Modified by: Jose Zamora
//Date: 02/21/2022  
//program: walk.cpp
//author:  Gordon Griesel
//date:    summer 2017 - 2018
//
//Walk cycle using a sprite sheet.
//images courtesy: http://games.ucla.edu/resource/walk-cycles/
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"

//defined types
typedef double Flt;
typedef double Vec[3];
typedef Flt Matrix[4][4];

//macros
#define rnd() (((double)rand())/(double)RAND_MAX)
#define random(a) (rand()%a)
#define MakeVector(x, y, z, v) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
                      (c)[1]=(a)[1]-(b)[1]; \
                      (c)[2]=(a)[2]-(b)[2]
//constants
const float timeslice = 1.0f;
const float gravity = -0.2f;
const float pos1 = 1920/2, pos2 = 1080/2;
#define ALPHA 1


class Image {
public:
	int width, height;
	unsigned char *data;
	~Image() { delete [] data; }
	Image(const char *fname) {
		if (fname[0] == '\0')
			return;
		//printf("fname **%s**\n", fname);
		char name[40];
		strcpy(name, fname);
		int slen = strlen(name);
		name[slen-4] = '\0';
		//printf("name **%s**\n", name);
		char ppmname[80];
		sprintf(ppmname,"%s.ppm", name);
		//printf("ppmname **%s**\n", ppmname);
		char ts[100];
		//system("convert eball.jpg eball.ppm");
		sprintf(ts, "convert %s %s", fname, ppmname);
		system(ts);
		//sprintf(ts, "%s", name);
		FILE *fpi = fopen(ppmname, "r");
		if (fpi) {
			char line[200];
			fgets(line, 200, fpi);
			fgets(line, 200, fpi);
			while (line[0] == '#')
				fgets(line, 200, fpi);
			sscanf(line, "%i %i", &width, &height);
			fgets(line, 200, fpi);
			//get pixel data
			int n = width * height * 3;			
			data = new unsigned char[n];			
			for (int i=0; i<n; i++)
				data[i] = fgetc(fpi);
			fclose(fpi);
		} else {
			printf("ERROR opening image: %s\n",ppmname);
			exit(0);
		}
		unlink(ppmname);
	}
};
//JOSE: THIS IS WHERE ANY IMAGES WE USE GO
Image img[2] = {"images/walk.gif",
		"images/map1.gif"};

//JOSE: sets up variables used for creating the background.
class Texture {
public:
	Image *backimage;
	GLuint backTexture;
	float xc[2];
	float yc[2];
        //TESTING:
        //Player 1
        float p1lr;
        float p1ud;
        int p1punch;
        //Player2 2
        float p2lr;
        float p2ud;
        int p2punch;
};

class Player_1 {
    public:
        Vec pos;
        Vec vel;
        float w = 20.f;
        float h = 100.0f;
        float pw1 = 20.f;
        float pw2 = 100.0f;
        float ph = 10.0f;
        int punch;
        float punchcooldown = 20.0f;
        int health = 100;
} player1;

class Player_2 {
    public:
        Vec pos;
        Vec vel;
        float w = 20.f;
        float h = 100.0f;
        float pw1 = 20.f;
        float pw2 = 100.0f;
        float ph = 10.0f;
        int punch;
        float punchcooldown = 20.0f;
        int health = 100;
} player2;
//-----------------------------------------------------------------------------
//Setup timers
class Timers {
public:
	double physicsRate;
	double oobillion;
	struct timespec timeStart, timeEnd, timeCurrent;
	struct timespec walkTime;
	Timers() {
		physicsRate = 1.0 / 30.0;
		oobillion = 1.0 / 1e9;
	}
	double timeDiff(struct timespec *start, struct timespec *end) {
		return (double)(end->tv_sec - start->tv_sec ) +
				(double)(end->tv_nsec - start->tv_nsec) * oobillion;
	}
	void timeCopy(struct timespec *dest, struct timespec *source) {
		memcpy(dest, source, sizeof(struct timespec));
	}
	void recordTime(struct timespec *t) {
		clock_gettime(CLOCK_REALTIME, t);
	}
} timers;
//-----------------------------------------------------------------------------

class Global {
public:
	int done;
	int xres, yres;
	int walk;
	int walkFrame;
    int gflag, bflag;
	int jeflag, joflag;
	double delay;
	int feature_mode;
	GLuint walkTexture;
	//GLuint map1Texture;
    int mapCenter;
	Texture tex;
    char keyStates[65536];
	Vec box[20];
	Global() {
		done=0;
		xres=1920;		// 800
		yres=1080;		// 600
		walk=0;
        mapCenter = 1;
        gflag = bflag = jeflag = joflag = 0;
        memset(keyStates, 0, 65536);
		walkFrame=0;
		delay = 0.1;
		for (int i=0; i<20; i++) {
			box[i][0] = rnd() * xres;
			box[i][1] = rnd() * (yres-220) + 220.0;
			box[i][2] = 0.0;
		}
	}
} g;

class X11_wrapper {
private:
	Display *dpy;
	Window win;
public:
	X11_wrapper() {
		GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
		//GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
		XSetWindowAttributes swa;
		setupScreenRes(g.xres, g.yres);
		dpy = XOpenDisplay(NULL);
		if (dpy == NULL) {
			printf("\n\tcannot connect to X server\n\n");
			exit(EXIT_FAILURE);
		}
		Window root = DefaultRootWindow(dpy);
		XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
		if (vi == NULL) {
			printf("\n\tno appropriate visual found\n\n");
			exit(EXIT_FAILURE);
		} 
		Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
		swa.colormap = cmap;
		swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
							StructureNotifyMask | SubstructureNotifyMask;
		win = XCreateWindow(dpy, root, 0, 0, g.xres, g.yres, 0,
								vi->depth, InputOutput, vi->visual,
								CWColormap | CWEventMask, &swa);
		GLXContext glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
		glXMakeCurrent(dpy, win, glc);
		setTitle();
	}
	~X11_wrapper() {
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);
	}
	void setTitle() {
		//Set the window title bar.
		XMapWindow(dpy, win);
		XStoreName(dpy, win, "Square Up!");
	}
	void setupScreenRes(const int w, const int h) {
		g.xres = w;
		g.yres = h;
	}
	void reshapeWindow(int width, int height) {
		//window has been resized.
		setupScreenRes(width, height);
		glViewport(0, 0, (GLint)width, (GLint)height);
		glMatrixMode(GL_PROJECTION); glLoadIdentity();
		glMatrixMode(GL_MODELVIEW); glLoadIdentity();
		glOrtho(0, g.xres, 0, g.yres, -1, 1);
		setTitle();
	}
	void checkResize(XEvent *e) {
		//The ConfigureNotify is sent by the
		//server if the window is resized.
		if (e->type != ConfigureNotify)
			return;
		XConfigureEvent xce = e->xconfigure;
		if (xce.width != g.xres || xce.height != g.yres) {
			//Window size did change.
			reshapeWindow(xce.width, xce.height);
		}
	}
	bool getXPending() {
		return XPending(dpy);
	}
	XEvent getXNextEvent() {
		XEvent e;
		XNextEvent(dpy, &e);
		return e;
	}
	void swapBuffers() {
		glXSwapBuffers(dpy, win);
	}

} x11;

//function prototypes
void initOpengl(void);
void checkMouse(XEvent *e);
int checkKeys(XEvent *e);
void init();
void physics(void);
void render(void);


int main(void)
{
	initOpengl();
	init();
	int done = 0;
	while (!done) {
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.checkResize(&e);
			checkMouse(&e);
			done = checkKeys(&e);
		}
		physics();
		render();
		x11.swapBuffers();
	}
	cleanup_fonts();
	return 0;
}

unsigned char *buildAlphaData(Image *img)
{
	//add 4th component to RGB stream...
	int i;
	unsigned char *newdata, *ptr;
	unsigned char *data = (unsigned char *)img->data;
	newdata = (unsigned char *)malloc(img->width * img->height * 4);
	ptr = newdata;
	unsigned char a,b,c;
	//use the first pixel in the image as the transparent color.
	unsigned char t0 = *(data+0);
	unsigned char t1 = *(data+1);
	unsigned char t2 = *(data+2);
	for (i=0; i<img->width * img->height * 3; i+=3) {
		a = *(data+0);
		b = *(data+1);
		c = *(data+2);
		*(ptr+0) = a;
		*(ptr+1) = b;
		*(ptr+2) = c;
		*(ptr+3) = 1;
		if (a==t0 && b==t1 && c==t2)
			*(ptr+3) = 0;
		//-----------------------------------------------
		ptr += 4;
		data += 3;
	}
	return newdata;
}

void initOpengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, g.xres, g.yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
	//
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
	//
	//Clear the screen
	glClearColor(1.0, 1.0, 1.0, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT);
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
	//
	//load the images file into a ppm structure.
	//
	int w = img[0].width;
	int h = img[0].height;
	//
	//create opengl texture elements
	glGenTextures(1, &g.walkTexture);
	//-------------------------------------------------------------------------
	//silhouette
	//this is similar to a sprite graphic
	//
	glBindTexture(GL_TEXTURE_2D, g.walkTexture);
	//
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//
	//must build a new set of data...
	unsigned char *walkData = buildAlphaData(&img[0]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, walkData);
	//free(walkData);
	//unlink("./images/walk.ppm");
	//-------------------------------------------------------------------------
    //JOSE: Part of displaying map 1; will try and see if i can add to own source file
	int w1 = img[1].width;
	int h1 = img[1].height;
	glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w1, h1, 0,
                GL_RGB, GL_UNSIGNED_BYTE, img[1].data);
    g.tex.xc[0] = 0.0f;
    g.tex.xc[1] = 1.0f;
    g.tex.yc[0] = 0.0f;
    g.tex.yc[1] = 1.0f;
}

void init() {
    // Initialize Player 1 stats
    MakeVector((float)g.xres/4,100.0f,0.0, player1.pos);
    MakeVector(10.0f,0.0,0.0, player1.vel);

    // Initialize Player 2 stats
    MakeVector((float)g.xres/2,100.0f,0.0, player2.pos);
    MakeVector(10.0f,0.0,0.0, player2.vel);
}

void checkMouse(XEvent *e)
{
	//Did the mouse move?
	//Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;
	//
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button is down
		}
		if (e->xbutton.button==3) {
			//Right button is down
		}
	}
	if (savex != e->xbutton.x || savey != e->xbutton.y) {
		//Mouse moved
		savex = e->xbutton.x;
		savey = e->xbutton.y;
	}
}

int checkKeys(XEvent *e)
{
    //keyboard input?
    static int shift=0;
    if (e->type != KeyRelease && e->type != KeyPress) {
        return 0;
    }
    int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
        //
    if (e->type == KeyRelease) {
        g.keyStates[key] = 0;
            if (key == XK_Shift_L || key == XK_Shift_R)
                shift = 0;
        return 0;
    }
    if (e->type == KeyPress) {
        g.keyStates[key] = 1;
        if (key == XK_Shift_L || key == XK_Shift_R) {
            shift = 1;
            return 0;
        }
    }

    (void)shift;
    switch (key) {
	case XK_w:
	    //timers.recordTime(&timers.walkTime);
	    //g.walk ^= 1;
	    break;
	case XK_f:
	    if (shift) {
	        if (g.feature_mode == 1) {
		    g.feature_mode = 0;
                    std::cout << g.tex.p1lr << std::endl;
		}
		else {
		    g.feature_mode = 1;
		}
	    }
	    break;
        case XK_g:
		if (g.gflag == 1) {
		    g.gflag = 0;
		} 
                else {
		    g.gflag = 1;
		}
            break;
	case XK_1:
		std::cout << "START" << std::endl;
		    g.joflag = 1;
		break;
	case XK_Left:
            //
            break;
	case XK_Right:    
	    break;
	case XK_Up:
	    break;
	case XK_Down:
	    break;
	case XK_equal:
	    g.delay -= 0.005;
		if (g.delay < 0.005)
	    		g.delay = 0.005;
	    break;
	case XK_minus:
	    g.delay += 0.005;
	    break;
	case XK_Escape:
	    return 1;
	    break;
        case XK_a:
            
            break;
        case XK_d:
            break;
        
	}
	return 0;
}

Flt VecNormalize(Vec vec)
{
	Flt len, tlen;
	Flt xlen = vec[0];
	Flt ylen = vec[1];
	Flt zlen = vec[2];
	len = xlen*xlen + ylen*ylen + zlen*zlen;
	if (len == 0.0) {
		MakeVector(0.0,0.0,1.0,vec);
		return 1.0;
	}
	len = sqrt(len);
	tlen = 1.0 / len;
	vec[0] = xlen * tlen;
	vec[1] = ylen * tlen;
	vec[2] = zlen * tlen;
	return(len);
}

void physics(void)
{
    int addgrav = 1;
	if (g.walk) {
	    //man is walking...
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
		if (g.box[i][0] < -10.0)
		    g.box[i][0] += g.xres + 10.0;
	    }
    }

    //--Player 1 Movement & Abilites--

    // Move left
    if(g.keyStates[XK_a]) {
        if (player1.pos[0] > 20) {
            player1.pos[0] -= player1.vel[0];
            std::cout << player1.pos[0] << std::endl;
        }
        else if (player1.pos[0] <= 20.0f && g.tex.xc[0] >= 0) {
            g.tex.xc[0] -= 0.001;
            g.tex.xc[1] -= 0.001;
        }
    }

    // Move right
    if (g.keyStates[XK_d]) {
        if (player1.pos[0] < 1420.0f) {
            player1.pos[0] += player1.vel[0];
            std::cout << player1.pos[0] << std::endl;
        }
        else if (player1.pos[0] >= 1420.0f && g.tex.xc[0] <= 0.166) {
            g.tex.xc[0] += 0.001;
            g.tex.xc[1] += 0.001;
        }

    }

    // Jump
    if (g.keyStates[XK_w] && player1.vel[1] == 0) {
        player1.vel[1] = 200.0f;
        player1.pos[1] += 500.0f;
        std::cout << "W key pressed" << std::endl;
    }

    // Gravity
    if (player1.vel[1] != 0) {
        std::cout << "yo1" << std::endl;
        player1.vel[1] -= 100.0f;
        player1.pos[1] -= 250.0f;
    }

    //JOSE: apparently after a jump the box doesn't go exactly back to original spot; this would fix it if need be
    /*if (player1.pos[1] < 100.0f) {
        player1.vel[1] = 0;
        player1.pos[1] = 100.0f;
    }*/

    // Punch
    if (g.keyStates[XK_b] && player1.punch == 0) {
        player1.punch = 1;

        // Punch Detection
        if (player1.pos[0] + player1.pw2 >= player2.pos[0]) {
            std::cout << "Player 1 hits Player 2!" << std::endl;
            player2.health -= 10;
        }
    }
    
    // Punch cooldown
    if (player1.punch == 1) {
        if (player1.punchcooldown == 0) {
            player1.punch = 0;
            player1.punchcooldown = 20.0f;
        }
        else {
            player1.punchcooldown -= 10.0f;
        }
    }

    // --Player 2 Movement & Abilites--
 
    // Move left   
    if (g.keyStates[XK_Left]) {
        if (player2.pos[0] > 20) {
            player2.pos[0] -= player2.vel[0];
        }
        else if (player2.pos[0] <= 20.0f && g.tex.xc[0] >= 0) {
            g.tex.xc[0] -= 0.001;
            g.tex.xc[1] -= 0.001;
        }
    }

    // Move right
    if (g.keyStates[XK_Right]) {
        if (player2.pos[0] < 1420.0f) {
            player2.pos[0] += player2.vel[0];
        }
        else if (player2.pos[0] >= 1420.0f && g.tex.xc[0] <= 0.166) {
            g.tex.xc[0] += 0.001;
            g.tex.xc[1] += 0.001;
        }
    }

    // Jump
    if (g.keyStates[XK_Up] && player2.vel[1] == 0) {
        player2.vel[1] = 200.0f;
        player2.pos[1] += 500.0f;
        std::cout << "Up Arrow key pressed" << std::endl;
    }

    // Gravity
    if (player2.vel[1] != 0) {
        std::cout << "Down" << std::endl;
        player2.vel[1] -= 100.0f;
        player2.pos[1] -= 250.0f;
    }


    // Punch
    if (g.keyStates[XK_m] && player2.punch == 0) {
        player2.punch = 1;

        // Punch detection
        if (player2.pos[0] + player2.pw1 >= player1.pos[0]) {
            std::cout << "Player 2 hits Player 1!" << std::endl;
            player1.health -= 10;
        }
    }

    // Punch cooldown
    if (player2.punch == 1) {
        if (player2.punchcooldown == 0) {
            player2.punch = 0;
            player2.punchcooldown = 20.0f;
        }
        else {
            player2.punchcooldown -= 10.0f;
        }
    }

}

void render(void)
{
    //Clear the screen
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    
    float cx = g.xres/3.0;
    float cy = g.yres/2.0;

    //Display Background
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
    extern void display_map_one(float x0, float x1, float y0, float y1, int xres, int yres);
    display_map_one(g.tex.xc[0], g.tex.xc[1],
	    		g.tex.yc[0], g.tex.yc[1],
	            	g.xres, 	 g.yres);

    //Centers Background; will combine with ^this later
    if (g.mapCenter != 0) {
        g.tex.xc[0] = 0.083;
        g.tex.xc[1] = 1.083;
        g.mapCenter = 0;
    }

    //glClear(GL_COLOR_BUFFER_BIT);
    //static float pos[2] = {g.xres/2.0f, g.yres/2.0f};
    glPushMatrix();
    glTranslatef(player1.pos[0], player1.pos[1], 0.0f);
    glBegin(GL_QUADS);
		glColor3ub(150, 160, 220);
		glVertex2f(-player1.w,  -player1.h);
		glVertex2f(-player1.w,   player1.h);
		glVertex2f( player1.w,   player1.h);
		glVertex2f( player1.w,  -player1.h);
    glEnd();
    glPopMatrix();
    // Player 1 punch box
    if (player1.punch == 1) {
    glPushMatrix();
    glTranslatef(player1.pos[0], player1.pos[1]+40.0f, 0.0f);
    glBegin(GL_QUADS);
		glColor3ub(0, 150, 0);
		glVertex2f(-player1.pw1,  -player1.ph);
		glVertex2f(-player1.pw1,   player1.ph);
		glVertex2f( player1.pw2,   player1.ph);
		glVertex2f( player1.pw2,  -player1.ph);
    glEnd();
    glPopMatrix();
    }
    //Player2:
    glPushMatrix();
    glTranslatef(player2.pos[0], player2.pos[1], 0.0f);
    glBegin(GL_QUADS);
		glColor3ub(0, 0, 0);
		glVertex2f(-player2.w,  -player2.h);
		glVertex2f(-player2.w,   player2.h);
		glVertex2f( player2.w,   player2.h);
		glVertex2f( player2.w,  -player2.h);
    glEnd();
    glPopMatrix();
    // Player 2 punch box
    if (player2.punch == 1) {
    glPushMatrix();
    glTranslatef(player2.pos[0], player2.pos[1]+40.0f, 0.0f);
    glBegin(GL_QUADS);
		glColor3ub(0, 150, 0);
		glVertex2f(-player2.pw2,  -player2.ph);
		glVertex2f(-player2.pw2,   player2.ph);
		glVertex2f( player2.pw1,   player2.ph);
		glVertex2f( player2.pw1,  -player2.ph);
	glEnd();
    glPopMatrix();
    }


    //JOSE: I think this is part of Sprite stuff
    //float h = 250.0;
	//float w = h * 0.5;
	//glPushMatrix();
	//glColor3f(1.0, 1.0, 1.0);
	//glBindTexture(GL_TEXTURE_2D, g.walkTexture);
	
    //JOSE: Sprite stuff; can use later
	/*glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glColor4ub(255,255,255,255);
	int ix = g.walkFrame % 8;
	int iy = 0;
	if (g.walkFrame >= 8)
		iy = 1;
	float tx = (float)ix / 10.0;
	float ty = (float)iy / 10.0;
	glBegin(GL_QUADS);
		glTexCoord2f(tx,      ty+.5); 	glVertex2i(cx-w, cy-h-150);
		glTexCoord2f(tx,      ty );    	glVertex2i(cx-w, cy+h-150);
		glTexCoord2f(tx+.125, ty);    	glVertex2i(cx+w, cy+h-150);
		glTexCoord2f(tx+.125, ty+.5); 	glVertex2i(cx+w, cy-h-150);
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);*/
	
    //JOSE: I JUST HAVE TO KEEP THIS; OTHERWISE, MAP MOVING DOENS'T WORK
    if (!g.feature_mode) {
	extern void display_controls(int wf, int yres);
        display_controls(g.walkFrame, g.yres);
    }
    //
    
    //
	if(g.gflag == 1){	
		//Genos functions
		extern void newText(int yres, int xres);
		newText(g.yres, g.xres);
	}

	if(g.jeflag == 1){
		//Jesses function
		extern void greenBoxes();
		greenBoxes();
	}
    //
    
    //
    if(g.joflag == 1){
		//Joses function
        extern void fmBorder(int xres, int yres);
        fmBorder(g.xres, g.yres);

		extern void test_text (int xres, int yres);
		test_text(g.xres, g.yres);
	}
    //
    Rect r;
    unsigned int c = 0x0000ff00;
    r.bot = g.yres - 20;
    r.left = g.xres/2;
    r.center = 50;
    ggprint8b(&r, 16, c, "Player 1 Health: %i", player1.health);
    ggprint8b(&r, 16, c, "Player 2 Health: %i", player2.health);
   
}








