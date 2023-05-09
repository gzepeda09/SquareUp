//3350
//Modified by: Jose Zamora
//Date: 02/21/2022  
//program: walk.cpp
//author:  Gordon Griesel
//date:    summer 2017 - 2018
//
//modified by: Geno Zepeda
//purpose: developed a function that renders random platforms for player to jump on
//Walk cycle using a sprite sheet.
//images courtesy: http://games.ucla.edu/resource/walk-cycles/
//
#include <iostream>
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
#include <thread>
#include "box.h"
#include <GL/glut.h>
#include "image.h"

using namespace std;
//#include <windows.h>
//#include <irrklang/irrKlang.h>
//using namespace irrklang;

//defined types
typedef double Flt;
typedef double Vec[3];
typedef Flt	Matrix[4][4];



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
//bool* keyStates = new bool[256];

//All external functions -
// --- Jose ---
#include "jzamora2.h"
//--- Jesse ---
extern void restartScreen(int player, int ywin, int xwin);
extern void playerBlocking(float w, float h, float x, float y, int flipped);
extern int mysteryBox(int x, int mbn);
extern void holdingWeapon(float w, float h, float x, float y, int flipped, int weapon);
extern void useWeapon(float w, float h, float x, float y, int flipped, int weapon);
extern void showPowerUp(int weapon, int ywin, int xwin);
extern void restartGame(int* p1Health, int* p2Health, double* p1Pos, double* p2Pos,
                		int* p1Dead, int* p2Dead, int* p1Weapon, int* p2Weapon,
                		time_t* p1TimeLimit, time_t* p2TimeLimit, int* gRestart,
                		int* nextMB, int* mBS, int xwin);
extern void player1PU (int* p1PUL, double* p1Vel, double* p2Vel, 
                int* p1Health, time_t* p1TL, int* weapon, time_t p2TL);
extern void player2PU (int* p2PUL, double* p1Vel, double* p2Vel, 
                int* p2Health, time_t* p2TL, int* weapon, time_t p1TL);
extern void spawnMysteryBox(int* mBS, int* nMB, time_t* p1TL,
                    int* weapon1, time_t* p2TL, int* weapon2,
                    int p1Health, int p2Health, int xwin,
                    int flag, double p1PosX, double p2PosX,
                    double p1PosY, double p2PosY);
//--- Brian ---
extern void Player_1(int cx, int cy, int walkFrame, GLuint walkTexture);
extern void Player_1_fliped(int cx, int cy, int walkFrame, GLuint walkTexture);
extern void Player_2(int cx, int cy, int walkFrame, GLuint walkTexture);
extern void Player_2_fliped(int cx, int cy, int walkFrame, GLuint walkTexture);
extern void Player_2_spe(int cx, int cy, int walkFrame, GLuint walkTexture);
extern void Player_2_spe_fliped(int cx, int cy, int walkFrame, GLuint walkTexture);


// Geno
extern void playSound();
extern void strMenu(int yres, int xres);
extern void powBar(float w, float h, unsigned char color[3], float pos0, 
		float pos1);
extern void superPunch(int w, int w2, int h, float x, float y, int flipped, int player);
extern void cntrlMenu(int yres, int xres);


//JOSE: THIS IS WHERE ANY IMAGES WE USE GO
Image img[4] = {"images/Freddy2.png",
	"images/map1-06.png",
	"images/Wolf.png",
	"images/Wolf_spe.png"};

//JOSE: sets up variables used for creating the background.
class Texture {
	public:
		Image *backimage;
		GLuint backTexture;
		GLuint map1Texture;
		GLuint map2Texture;
		float xc[2];
		float yc[2];
};

class Player_1 {
	public:
		Vec pos;
		Vec vel;
		int dead = 0;
		int block = 0;
		int sPunch = 0;
		int pBar = 0;
		int weapon = 0; 
		time_t timeLimit = 0;
		int puLimit = 0;
		float w = 20.0f;
		float h = 100.0f;
		float pw1 = 20.0f;
		float pw2 = 100.0f;
		float ph = 10.0f;
		int punch;
		int punchcooldown = 250;
		int health = 150;
} player1;

class Player_2 {
	public:
		Vec pos;
		Vec vel;
		int dead = 0;
		int block = 0;
		int pBar = 0;
		int sPunch = 0;
		int weapon = 0;
		time_t timeLimit = 0;
		int puLimit = 0;
		float w = 20.0f;
		float h = 100.0f;
		float pw1 = 20.0f;
		float pw2 = 100.0f;
		float ph = 10.0f;
		int punch;
		int punchcooldown = 250;
		int health = 150;
} player2;

class Power_up {
	public:
		Vec pos;
		float w = 20.0f;
		float h = 20.0f;
		float power_cooldown = 150.0f;
		float power_life_span = 100;
} Power_up;

//-----------------------------------------------------------------------------
//Setup timers
class Timers {
	public:
		double physicsRate;
		double oobillion;
		struct timespec timeStart, timeEnd, timeCurrent;
		struct timespec walkTime;
		struct timespec walk2Time;
		struct timespec speTime;
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
		int walk2Frame;
		int speFrame;
		int gflag, bflag;
		int start;
		int ctrls;
		int jeflag, joflag;
		double delay;
		int feature_mode;
		int restart;
		int mysteryBoxSpawn;
		int nextMysteryBox;
		GLuint walkTexture;
		GLuint walk2Texture;
		GLuint speTexture;
		int mapCenter;
		int punchflip = 1;
		Texture tex;
		char keyStates[65536];
		int loadMap = 0;
		int time_scroll = 0;
		float scroll = 0.0f;
		Vec box[20];
		Global() {
			done=0;
			xres=1920;
			yres=1080;
			walk=0;
			mapCenter = 1;
			bflag = jeflag = joflag = 0;
			gflag = 1;
			start = 0;
			ctrls = 0;
			restart = 0;
			mysteryBoxSpawn = 0;
			nextMysteryBox = 100;
			memset(keyStates, 0, 65536);
			walkFrame=0;
			walk2Frame=0;
			speFrame=0;
			delay = 0.1;
			for (int i=0; i<20; i++) {
				box[i][0] = rnd() * xres;
				box[i][1] = rnd() * (yres-220) + 220.0;
				box[i][2] = 0.0;
			}
		}
} g;


//Geno -  random platform for 
Box rplat[3];
Box hbar[2];
Box powerBar[2];

// Box rpl[3] = {Box(g.yres, g.xres), Box(g.yres, g.xres), Box(g.yres, g.xres)};


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

bool music = false;
int rNum = 0;
int main(void)
{
	initOpengl();
	init();

	std::srand(std::time(nullptr));

	//chooses a rand num between 1-100 to check against players health
	rNum = std::rand() % 61 + 30;

	std::cout << "Random number: " << rNum << std::endl;

	//GENOS SOUND FUNCTIONALITY
	extern void initSound();
	initSound();

	int done = 0;
	while (!done) {
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.checkResize(&e);
			checkMouse(&e);
			done = checkKeys(&e);
			//Geno - logic for main music
			if(!music){
				playSound();
				music = true;
			}
		}
		physics();
		render();
		x11.swapBuffers();
	}

	//GENOS SOUND FUNCTIONALITY
	extern void cleanupSound();
	cleanupSound();
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
	//g.tex.yc[1] = 1.0f;
	g.tex.yc[1] = 0.125f;
	//==========================================================================
	//load the images file into a ppm structure.
	//Brian used for displaying sprites
	//==========================================================================
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
	//============================================================================
	//free(walkData);
	//unlink("./images/walk.ppm");
	//-------------------------------------------------------------------------
	int w2 = img[2].width;
	int h2 = img[2].height;
	//
	//create opengl texture elements
	glGenTextures(1, &g.walk2Texture);
	//-------------------------------------------------------------------------
	//silhouette
	//this is similar to a sprite graphic
	//
	glBindTexture(GL_TEXTURE_2D, g.walk2Texture);
	//
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//
	//must build a new set of data...
	unsigned char *walk2Data = buildAlphaData(&img[2]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w2, h2, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, walk2Data);
	//-------------------------------------------------------------------------
	int w3 = img[3].width;
	int h3 = img[3].height;
	//
	//create opengl texture elements
	glGenTextures(1, &g.speTexture);
	//-------------------------------------------------------------------------
	//silhouette
	//this is similar to a sprite graphic
	//
	glBindTexture(GL_TEXTURE_2D, g.speTexture);
	//
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//
	//must build a new set of data...
	unsigned char *speData = buildAlphaData(&img[3]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w3, h3, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, speData);


}
void init() {
	// Initialize Player 1 stats
	MakeVector((float)g.xres/3,100.0f,0.0, player1.pos);
	MakeVector(10.0f,0.0,0.0, player1.vel);

	// Initialize Player 2 stats
	MakeVector((float)g.xres/1.5,100.0f,0.0, player2.pos);
	MakeVector(10.0f,0.0,0.0, player2.vel);
}

void checkMouse(XEvent *e)
{
	//Did the mouse move?
	//Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;

	if (e->type != ButtonPress
			&& e->type != ButtonRelease
			&& e->type != MotionNotify) {
		return;
	}

	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
		}
		if (e->xbutton.button==3) {
		}
	}

	if (e->type == MotionNotify) {
		if (savex != e->xbutton.x || savey != e->xbutton.y) {
			//Mouse moved
			savex = e->xbutton.x;
			savey = e->xbutton.y;
		}
	}
}

int checkKeys(XEvent *e)
{
    //keyboard input?
	static int shift=0;
    static int ctrl=0;
	if (e->type != KeyRelease && e->type != KeyPress) {
		return 0;
	}
	int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
	//
	if (e->type == KeyRelease) {
		g.keyStates[key] = 0;
		if (key == XK_Shift_L || key == XK_Shift_R)
			shift = 0;
		if (key == XK_Control_L || key == XK_Control_R)
			 ctrl = 0;
		return 0;
	}
	if (e->type == KeyPress) {
		g.keyStates[key] = 1;
		if (key == XK_Shift_L || key == XK_Shift_R) {
			shift = 1;
            return 0;
        }
		if (key == XK_Control_L || key == XK_Control_R) {
			ctrl = 1;
	        return 0;
		}
	}

	// checks keys
	(void)shift;
    (void)ctrl;
	switch (key) {
		case XK_w:
			timers.recordTime(&timers.walkTime);
			g.walk ^= 1;
			break;
		case XK_f:
			if (shift) {
				if (g.feature_mode == 1) {
					g.feature_mode = 0;
				}
				else {
					g.feature_mode = 1;
				}
			}
			break;
		case XK_1:
			//Brian: change the flags to "flag =! flag;" 
			//       make it toggle with less lines of code
			g.gflag =! g.gflag;
			break;
		case XK_2:
			g.bflag =! g.bflag;
			break;
		case XK_3:
			g.jeflag =! g.jeflag;
			break;
		case XK_Left:
			break;
		case XK_j:
			if (shift)
				//Brian: change the flags to "flag =! flag;" 
				//       make it toggle with less lines of code
				g.joflag =! g.joflag;
			break;
		case XK_Right:
			break;
        case XK_Up:
            if (shift && g.joflag == 1) {
                player1.vel[0] += 2.0f;
                player2.vel[0] += 2.0f;
            }
			break;
		case XK_Down:
            if (shift && g.joflag == 1) {
                player1.vel[0] -= 2.0f;
                player2.vel[0] -= 2.0f;
            }
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
		case XK_F1:
			if(g.start != 1){
				g.start = 1;
			}
			g.loadMap = 1;
			break;
		case XK_F2:
			if(g.start != 2){
				g.start = 2;
			} else {
				g.start = 0;
			}
			break;
		case XK_d:
			break;
		case XK_q:
			if(g.ctrls != 1){
				g.ctrls = 1;
			} else {
				g.ctrls = 0;
			}
			break;
		case XK_r:
			g.restart =! g.restart;
			break;
         case XK_7:
            if (shift && g.joflag == 1) {
                player1.w += 10;
            }
            else if (ctrl && g.joflag == 1) {
                player1.pw1 += 10;
                player1.pw2 += 10;
            }
            break;
        case XK_F7:
            if (shift && g.joflag == 1) {
                player1.w -= 10;
            }
            else if (ctrl && g.joflag == 1) {
                player1.pw1 -= 10;
                player1.pw2 -= 10;
            }
            break;
        case XK_8:
            if (shift && g.joflag == 1) {
                player1.h += 10;
            }
            else if (ctrl && g.joflag == 1) {
                player1.ph += 10;
            }
            break;
        case XK_F8:
            if (shift && g.joflag == 1) {
                player1.h -= 10;
            }
            else if (ctrl && g.joflag == 1) {
                player1.ph -= 10;
            }
            break;
        case XK_9:
            if (shift && g.joflag == 1) {
                player2.w += 10;
            }
            else if (ctrl && g.joflag == 1) {
                player2.pw1 += 10;
                player2.pw2 += 10;
            }
            break;
        case XK_F9:
            if (shift && g.joflag == 1) {
                player2.w -= 10;
            }
            else if (ctrl && g.joflag == 1) {
                player2.pw1 -= 10;
                player2.pw2 -= 10;
            }
            break;
        case XK_0:
            if (shift && g.joflag == 1) {
                player2.h += 10;
            }
            else if (ctrl && g.joflag == 1) {
                player2.ph += 10;
            }
            break;
        case XK_F10:
            if (shift && g.joflag == 1) {
                player2.h -= 10;
            }
            else if (ctrl && g.joflag == 1) {
                player2.ph -= 10;
            }
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
//Geno - this is a flag for platforms
bool stop = false;
bool top = false;
bool endIt = false;

bool stop2 = false;
bool top2 = false;
bool endIt2 = false;


//Geno - flag for physics
int activate = 0;
int activate2 = 0;


int prevI = 0;
int pltFlg = 0;


//Geno - flags for charge up
bool chargeUp = false;
bool chargeUp2 = false;

//Geno - for super punch
bool once = false;

//Brina - for idle animation
bool idle = true;
bool idle2 = true;

bool ePressed = false; 
bool kPressed = false;
bool rndrPbar = false;
bool rndrPbar2 = false;
void physics(void)
{
	//int addgrav = 1;
	/*
	   if (g.walk) {
	//man is walking...
	//when time is up, advance the frame.
	timers.recordTime(&timers.timeCurrent);
	double timeSpan = timers.timeDiff(&timers.walkTime, &timers.timeCurrent);
	if (timeSpan > g.delay) {
	//advance
	++g.walkFrame;
	if (g.walkFrame >= 7)
	g.walkFrame -= 7;
	timers.recordTime(&timers.walkTime);
	}
	for (int i=0; i<20; i++) {
	g.box[i][0] -= 2.0 * (0.05 / g.delay);
	if (g.box[i][0] < -16.0)
	g.box[i][0] += g.xres + 16.0;
	}
	}
	*/
	//Super-Punch

	extern void supPunchPhys1(bool *btnPress, int *pB, int *sPnch);
	extern void supPunchPhys2(bool *btnPress, int *sPnch);




	if (g.keyStates[XK_e] && !chargeUp) {
		supPunchPhys1(&ePressed, &player1.pBar, &player1.sPunch);
	} else {
		supPunchPhys2(&ePressed, &player1.sPunch);
	}


	if (g.keyStates[XK_k] && !chargeUp) {
		supPunchPhys1(&kPressed, &player2.pBar, &player2.sPunch);
	} else {
		supPunchPhys2(&kPressed, &player2.sPunch);
	}




	//--Player 1 Movement & Abilites--	
	if(idle == true) {
		//man is walking...
		//when time is up, advance the frame.
		timers.recordTime(&timers.timeCurrent);
		double timeSpan = timers.timeDiff(&timers.walkTime, 
				&timers.timeCurrent);

		//FIXME: this shouldn't be necessary
		//       our animations shouldn't be automaticate
		//       or based on a timer
		if (timeSpan > g.delay) {
			//advance
			++g.walkFrame;
			if(g.walkFrame > 7) {
				g.walkFrame = 0;
			}
			if(g.walkFrame >= 7)
				g.walkFrame -= 7;
			timers.recordTime(&timers.walkTime);
		}
		for (int i=0; i<20; i++) {
			g.box[i][0] -= 2.0 * (0.05 / g.delay);
			if (g.box[i][0] < -16.0)
				g.box[i][0] += g.xres + 16.0;
		}
	}
	if(g.keyStates[XK_a] || g.keyStates[XK_d]) {
		idle = false;
	}else if(g.keyStates[XK_Left] || g.keyStates[XK_Right]) {
		idle2 = false;
	}else {
		idle = true;
		idle2 = true;
	}
	// Move left player 1
	if(g.keyStates[XK_a] && player1.dead == 0 && !chargeUp) {
		//man is walking...
		//when time is up, advance the frame.
		timers.recordTime(&timers.timeCurrent);
		double timeSpan = timers.timeDiff(&timers.walkTime, 
				&timers.timeCurrent);

		//FIXME: this shouldn't be necessary
		//       our animations shouldn't be automaticate
		//       or based on a timer
		if (timeSpan > g.delay) {
			//advance
			++g.walkFrame;
			if(g.walkFrame <= 8 || g.walkFrame >= 16) {
				g.walkFrame = 9;
			}
			if (g.walkFrame >= 16)
				g.walkFrame -= 16;
			timers.recordTime(&timers.walkTime);
		}
		for (int i=0; i<20; i++) {
			g.box[i][0] -= 2.0 * (0.05 / g.delay);
			if (g.box[i][0] < -16.0)
				g.box[i][0] += g.xres + 16.0;
		}
		/*if (player1.pos[0] > player1.w) {
		  }
		  else if (player1.pos[0] <= 20.0f && g.tex.xc[0] >= 0) {
		  g.tex.xc[0] -= 0.001;
		  g.tex.xc[1] -= 0.001;
		  }*/

		movePlayerLeft(&player1.pos[0], &player2.pos[0], 
				&player1.pos[1], &player2.pos[1],
				player1.vel[0],  player2.w,
				player1.w,       &g.tex.xc[0],    &g.tex.xc[1]);

		cout << player1.pos[0] << endl;
		cout << player2.pos[0] << endl;
	}
	// Move right player 1
	if (g.keyStates[XK_d] && player1.dead == 0 && !chargeUp) {
		//idle = true;
		//man is walking...
		//when time is up, advance the frame.
		timers.recordTime(&timers.timeCurrent);
		double timeSpan = timers.timeDiff(&timers.walkTime, 
				&timers.timeCurrent);

		//FIXME: this shouldn't be necessary
		//       our animations shouldn't be automaticate
		//       or based on a timer
		if (timeSpan > g.delay) {
			//advance
			++g.walkFrame;
			if(g.walkFrame <= 8 || g.walkFrame >= 16) {
				g.walkFrame = 9;
			}
			if (g.walkFrame >= 16)
				g.walkFrame -= 16;
			timers.recordTime(&timers.walkTime);
		}
		for (int i=0; i<20; i++) {
			g.box[i][0] -= 2.0 * (0.05 / g.delay);
			if (g.box[i][0] < -16.0)
				g.box[i][0] += g.xres + 16.0;
		}
		/*if (player1.pos[0] < (float)g.xres - player1.w) {
		  player1.pos[0] += player1.vel[0];
		  std::cout << player1.pos[0] << std::endl;
		  }
		  else if (player1.pos[0] >= 1420.0f && g.tex.xc[0] <= 0.166) {
		  g.tex.xc[0] += 0.001;
		  g.tex.xc[1] += 0.001;
		  }*/

		movePlayerRight(&player1.pos[0], &player2.pos[0], 
				&player1.pos[1], &player2.pos[1],
				player1.vel[0],  player2.w,
				player1.w,       &g.tex.xc[0],    
				&g.tex.xc[1],    g.xres);
	}

	// Jump player 1
	if (g.keyStates[XK_w] && player1.vel[1] == 0 && 
			player1.pos[1] != 600.0f && player1.dead == 0 && !chargeUp) {
		//player1.vel[1] = 200.0f;
		//player1.vel[2] = 200.0f;
		//player1.pos[1] += 500.0ff;
		//std::cout << "W key pressed" << std::endl;
		jumpPlayer(&player1.vel[1], &player1.vel[2]);
		g.walkFrame = 24;
	}

	if (player1.vel[2] != 0) {
		//player1.vel[2] -= 5.0f;
		//player1.pos[1] += 12.5f;
		movePlayerUp(&player1.vel[2], &player1.pos[1]);
	}

	// Gravity player 1
	if (player1.vel[1] != 0 && player1.vel[2] == 0.0f) {
		//std::cout << "yo1" << std::endl;
		//player1.vel[1] -= 5.0f;
		//player1.pos[1] -= 12.5f;
		movePlayerDown(&player1.vel[1], &player1.pos[1]);
	}

	//JOSE: apparently after a jump the box doesn't go exactly back 
	//      to original spot; this would fix it if need be
	/*if (player1.pos[1] < 100.0f) {
	  player1.vel[1] = 0;
	  player1.pos[1] = 100.0f;
	  }*/

	// Punch player 1
	if ((g.keyStates[XK_b] && player1.punch == 0 && 
				player1.dead == 0 && !chargeUp) ||
			(g.keyStates[XK_e] && player1.punch == 0 && 
			 player1.dead == 0 && !chargeUp)) {

		//Functions can be found in gzepeda.cpp
		extern void playPunchSound();
		playPunchSound();

		g.walkFrame = 20;

		if(player1.sPunch == 1){
			player1.punch = 0;
		}
		else{
			player1.punch = 1;
		}

		punchAbilityPlayer1(&player1.punch, &player1.sPunch, g.jeflag,
				&player1.pos[0], &player1.pos[1], player1.pw2,
				&player2.pos[0], &player2.pos[1], player2.w,
				&player2.vel[1], &player2.dead,
				player2.block, &player2.health, player1.weapon);
		// Punch Detection player 1
		/*if (player1.pos[0] + player1.pw2 >= player2.pos[0] + (-player2.w) && 
		  player1.pos[0] < player2.pos[0] && player1.weapon==0) {
		  std::cout << "Player 1 hits Player 2!" << std::endl;
		//player2.health -= 10;
		//For testing. delete once testing is done
		if (g.jeflag == 1 && player2.dead == 0) {
		player2.health -= 50;
		} else if (player2.dead == 0){
		if (!player2.block) {    
		if(player1.sPunch == 1){
		player2.health -= 20;
		}   else {
		player2.health -= 10;
		}
		} else {
		if(player1.sPunch == 1){
		player2.health -=10;
		} else {
		player2.health -=5;
		}
		}
		//Jesse - reaction to punches
		player2.pos[1] += 50.0f;
		player2.vel[1] += 20.0f;
		player2.pos[0] += 35.0f;
		}
		}
		// Punch Detection (Flipped)
		else if (player1.pos[0] - player1.pw2 <= player2.pos[0] + (player2.w) && 
		player1.pos[0] > player2.pos[0] && player1.weapon==0) {
		std::cout << "Player 1 hits Player 2!" << std::endl;
		//player2.health -= 10;
		//For testing. delete once testing is done
		if (g.jeflag == 1 && player2.dead == 0) {
		player2.health -= 50;
		} else if (player2.dead == 0){
		if (!player2.block) {    
		if(player1.sPunch == 1){
		player2.health -= 20;
		}   else {
		player2.health -= 10;
		}
		} else {
		if(player1.sPunch == 1){
		player2.health -=10;
		} else {
		player2.health -=5;
		}
		}
		//Jesse - reaction to punches
		player2.pos[1] += 50.0f;
		player2.vel[1] += 20.0f;
		player2.pos[0] -= 35.0f;
		}
		}*/
		//Sword detected for jesse feature
		if (player1.weapon==1 && (player1.pos[0] < player2.pos[0]) && 
				((player2.pos[0] - player1.pos[0]) <= 230)) {
			if (player2.block and !player2.dead) { 
				//player 2 is blocking and is not dead
				player2.health -= 10;
			} else if (!player2.dead) { 
				//player 2 is not blocking and is not dead
				player2.health -= 20;
			}

			//Reaction to punches
			player2.pos[1] += 50.0f;
			player2.vel[1] += 20.0f;
			player2.pos[0] += 35.0f;

			printf("player 1 swings sword\n");
		}
		//Sword detected flipped
		else if (player1.weapon==1 && (player2.pos[0] < player1.pos[0]) && 
				((player1.pos[0] - player2.pos[0]) <= 230)) { 
			if (player2.block and !player2.dead) { 
				//player 2 is blocking and is not dead
				player2.health -= 10;
			} else if (!player2.dead) { 
				//player 2 is not blocking and is not dead
				player2.health -= 20;
			}

			//Reaction to punches
			player2.pos[1] += 50.0f;
			player2.vel[1] += 20.0f;
			player2.pos[0] -= 35.0f;

			printf("player 1 swings sword\n");
		}

		//check if player 2 has died
		if (player2.health <= 0) {
			player2.health = 0;
			player2.dead = 1;
			g.nextMysteryBox = -10;
			g.walkFrame = 37;
			g.walk2Frame = 30;
		}
	}

	// Block player 1
	if (g.keyStates[XK_t] && player1.dead == 0 && 
			player1.punch == 0 && !chargeUp) {
		//printf("player1 blocking");
		player1.block = 1;
		g.walkFrame = 26;
	} else {
		player1.block = 0;
	}

	//Player 1 powerups and time limit
	player1PU (&player1.puLimit, &player1.vel[0], &player2.vel[0], 
                &player1.health, &player1.timeLimit, &player1.weapon, 
				player2.timeLimit);

	// Punch cooldown player 1
	if (player1.punch == 1) {
		/*if (player1.punchcooldown == 0) {
		  player1.punch = 0;
		  player1.punchcooldown = 250.0f;
		  }
		  else {
		  player1.punchcooldown -= 10.0f;
		  }*/
		punchCooldownPlayer(&player1.punchcooldown, &player1.punch);
	}

	// --Player 2 Movement & Abilites--
	if(idle2 == true) {
		//man is walking...
		//when time is up, advance the frame.
		timers.recordTime(&timers.timeCurrent);
		double timeSpan = timers.timeDiff(&timers.walk2Time, 
				&timers.timeCurrent);

		//FIXME: this shouldn't be necessary
		//       our animations shouldn't be automaticate
		//       or based on a timer
		if (timeSpan > g.delay) {
			//advance
			++g.walk2Frame;
			if(g.walk2Frame > 7) {
				g.walk2Frame = 0;
			}
			if(g.walk2Frame >= 7)
				g.walk2Frame -= 7;
			timers.recordTime(&timers.walk2Time);
		}
		for (int i=0; i<20; i++) {
			g.box[i][0] -= 2.0 * (0.05 / g.delay);
			if (g.box[i][0] < -16.0)
				g.box[i][0] += g.xres + 16.0;
		}
	}

	// Move left player 2
	if (g.keyStates[XK_Left] && player2.dead == 0) {
		//man is walking...
		//when time is up, advance the frame.
		timers.recordTime(&timers.timeCurrent);
		double timeSpan = timers.timeDiff(&timers.walk2Time, 
				&timers.timeCurrent);
		if (timeSpan > g.delay) {
			//advance
			++g.walk2Frame;
			if(g.walk2Frame <= 8 || g.walk2Frame >=16) {
				g.walk2Frame = 9;
			}
			if (g.walk2Frame >= 16)
				g.walk2Frame -= 16;
			timers.recordTime(&timers.walk2Time);
		}
		for (int i=0; i<20; i++) {
			g.box[i][0] -= 2.0 * (0.05 / g.delay);
			if (g.box[i][0] < -16.0)
				g.box[i][0] += g.xres + 16.0;
		}
		/*if (player2.pos[0] > player2.w) {
		  player2.pos[0] -= player2.vel[0];
		  }
		  else if (player2.pos[0] <= 20.0f && g.tex.xc[0] >= 0) {
		  g.tex.xc[0] -= 0.001;
		  g.tex.xc[1] -= 0.001;
		  }*/

		movePlayerLeft(&player2.pos[0], &player1.pos[0],
				&player2.pos[1], &player1.pos[1],
				player2.vel[0], player1.w,
				player2.w, &g.tex.xc[0], &g.tex.xc[1]);
	}

	// Move right player 2
	if (g.keyStates[XK_Right] && player2.dead == 0) {
		//man is walking...
		//when time is up, advance the frame.
		timers.recordTime(&timers.timeCurrent);
		double timeSpan = timers.timeDiff(&timers.walk2Time, 
				&timers.timeCurrent);
		if (timeSpan > g.delay) {
			//advance
			++g.walk2Frame;
			if(g.walk2Frame <= 8 || g.walk2Frame >= 16) {
				g.walk2Frame = 9;
			}
			if (g.walk2Frame >= 16)
				g.walk2Frame -= 16;
			timers.recordTime(&timers.walk2Time);
		}
		for (int i=0; i<20; i++) {
			g.box[i][0] -= 2.0 * (0.05 / g.delay);
			if (g.box[i][0] < -16.0)
				g.box[i][0] += g.xres + 16.0;
		}
		/*if (player2.pos[0] < (float)g.xres - player2.w) {
		  player2.pos[0] += player2.vel[0];
		  }
		  else if (player2.pos[0] >= 1420.0f && g.tex.xc[0] <= 0.166) {
		  g.tex.xc[0] += 0.001;
		  g.tex.xc[1] += 0.001;
		  }*/

		movePlayerRight(&player2.pos[0], &player1.pos[0],
				&player2.pos[1], &player1.pos[1],
				player2.vel[0], player1.w,
				player2.w, &g.tex.xc[0],
				&g.tex.xc[1], g.xres);
	}

	// Jump player 2
	if (g.keyStates[XK_Up] && player2.vel[1] == 0 && 
			player2.pos[1] != 600.0f && player2.dead == 0) {
		//player2.vel[1] = 200.0f;
		//player2.vel[2] = 200.0f;
		//player2.pos[1] += 500.0f;
		//std::cout << "Up Arrow key pressed" << std::endl;

		jumpPlayer(&player2.vel[1], &player2.vel[2]);
		g.walk2Frame = 24;
	}

	if (player2.vel[2] != 0) {
		//player2.vel[2] -= 5.0f;
		//player2.pos[1] += 12.5f;
		movePlayerUp(&player2.vel[2], &player2.pos[1]);
	}

	// Gravity player 2
	if (player2.vel[1] != 0 && player2.vel[2] == 0.0f) {
		//std::cout << "Down" << std::endl;
		//player2.vel[1] -= 5.0f;
		//player2.pos[1] -= 12.5f;
		movePlayerDown(&player2.vel[1], &player2.pos[1]);
	}

	//if (g.keyStates[XK_K]) {
		//man is walking...
		//when time is up, advance the frame.
		timers.recordTime(&timers.timeCurrent);
		double timeSpan = timers.timeDiff(&timers.speTime, 
				&timers.timeCurrent);
		if (timeSpan > g.delay) {
			//advance
			++g.speFrame;
			if (g.speFrame >= 4)
				g.speFrame -= 4;
			timers.recordTime(&timers.speTime);
		}
		for (int i=0; i<20; i++) {
			g.box[i][0] -= 2.0 * (0.05 / g.delay);
			if (g.box[i][0] < -16.0)
				g.box[i][0] += g.xres + 16.0;
		}
	//}
	// Punch player 2
	if ((g.keyStates[XK_m] && player2.punch == 0 && 
				player2.dead == 0 && !chargeUp2) ||
			(g.keyStates[XK_k] && player2.punch == 0 && 
			 player2.dead == 0 && !chargeUp2)) {
		
		g.walk2Frame = 22;
		if(player2.sPunch == 1){
			player2.punch = 0;
		}
		else{
			player2.punch = 1;
		}

		punchAbilityPlayer2(&player2.punch, &player2.sPunch, g.jeflag,
				&player2.pos[0], &player2.pos[1], player2.pw2,
				&player1.pos[0], &player1.pos[1], player1.w,
				&player1.vel[1], &player1.dead,
				player1.block, &player1.health, player2.weapon);
		// Punch detection player 2
		/*if (player2.pos[0] - player2.pw2 <= player1.pos[0] + (player1.w) && 
		  player2.pos[0] > player1.pos[0] && player2.weapon == 0) {
		  std::cout << "Player 2 hits Player 1!" << std::endl;
		//player1.health -= 10;
		//For testing. delete once testing is done
		if (g.jeflag == 1 && player1.dead == 0) {
		player1.health -= 50;
		} else if (player1.dead == 0){
		if (!player1.block) {    
		if(player2.sPunch == 1){
		player1.health -= 20;
		}   else {
		player1.health -= 10;
		}
		} else {
		if(player2.sPunch == 1){
		player1.health -=10;
		} else {
		player1.health -=5;
		}
		}
		//Jesse - reaction to punches
		player1.pos[1] += 50.0f;
		player1.vel[1] += 20.0f;
		player1.pos[0] -= 35.0f;
		}
		}
		// Punch Detection (Flipped)
		else if (player2.pos[0] + player2.pw2 >= player1.pos[0] + (-player1.w) 
		&& player2.pos[0] < player1.pos[0] && player2.weapon == 0) {
		std::cout << "Player 1 hits Player 2!" << std::endl;
		//player1.health -= 10;
		//For testing. delete once testing is done
		if (g.jeflag == 1 && player1.dead == 0) {
		player1.health -= 50;
		} else if (player1.dead == 0) {
		if (!player1.block) {    
		if(player2.sPunch == 1){
		player1.health -= 20;
		}   else {
		player1.health -= 10;
		}
		} else {
		if(player2.sPunch == 1){
		player1.health -=10;
		} else {
		player1.health -=5;
		}
		}
		//Jesse - raction to punches
		player1.pos[1] += 50.0f;
		player1.vel[1] += 20.0f;
		player1.pos[0] += 35.0f;
		}
		}*/
		if (player2.weapon==1 && (player1.pos[0] < player2.pos[0]) && 
				((player2.pos[0] - player1.pos[0]) <= 230)) {
			//not flipped 
			if (player1.block and !player1.dead) { 
				//player 1 is blocking and is not dead
				player1.health -= 10;
			} else if (!player1.dead) { 
				//player 1 is not blocking and is not dead
				player1.health -= 20;
			}

			//Reaction to punches
			player1.pos[1] += 50.0f;
			player1.vel[1] += 20.0f;
			player1.pos[0] -= 35.0f;

			printf("player 2 swings sword\n");
		} else if (player2.weapon==1 && (player2.pos[0] < player1.pos[0]) && 
				((player1.pos[0] - player2.pos[0]) <= 230)) {
			//flipped
			if (player1.block and !player1.dead) { 
				//player 1 is blocking and is not dead
				player1.health -= 10;
			} else if (!player1.dead) { 
				//player 1 is not blocking and is not dead
				player1.health -= 20;
			}

			//Reaction to punches
			player1.pos[1] += 50.0f;
			player1.vel[1] += 20.0f;
			player1.pos[0] += 35.0f;

			printf("player 2 swings sword\n");
		}
		//player 1 dies
		if (player1.health <= 0) {
			player1.health = 0;
			player1.dead = 1;
			g.nextMysteryBox = -10;
			g.walkFrame = 30;
			g.walk2Frame = 37;
		}
	}

	// Block player 2
	if (g.keyStates[XK_l] && player2.dead == 0 && 
			player2.punch == 0 && 
			!chargeUp2) {
		//printf("player1 blocking");
		player2.block = 1;
		g.walk2Frame = 24;
	} else {
		player2.block = 0;
	}

	//Player 2 powerups and time limit
	player2PU (&player2.puLimit, &player1.vel[0], &player2.vel[0], 
            &player2.health, &player2.timeLimit, &player2.weapon, player1.timeLimit);

	// Punch cooldown player 2
	if (player2.punch == 1) {
		/*if (player2.punchcooldown == 0) {
		  player2.punch = 0;
		  player2.punchcooldown = 250.0f;
		  }
		  else {
		  player2.punchcooldown -= 10.0f;
		  }*/
		punchCooldownPlayer(&player2.punchcooldown, &player2.punch);
	}

	// -- Misc --

	// Players flip
	if (player1.pos[0] > player2.pos[0]) {
		g.punchflip = -1;
	}
	else if (player1.pos[0] < player2.pos[0]) {
		g.punchflip = 1;
	}

	// //Geno - rand platform detection
	// extern void pltPhysics(double plPos0, double plPos1, double plVel,
	//               float rPos0, float rPos1, float rW, float rH, int yres);



	bool onPlat = false;
	bool onPlat2 = false;
	// bool p1 = false;
	// bool p2 = false;
	extern void rplatPhys(float ph, float rh, float rw, 
			float rPos0, float rPos1, float pos0, double *pos1,
			bool *onPlt, bool *tp, int *activate);
	extern void rplatPhys2(double *pos1, double *vel, bool *onPlt, 
			bool *tp, bool *stp, int *activate, int yres);
	if(pltFlg == 1){



		for (int i = 0; i < 3; i++) {


			rplatPhys(player1.h, rplat[i].h, rplat[i].w, rplat[i].pos[0], rplat[i].pos[1], player1.pos[0], 
					&player1.pos[1], &onPlat, &top, &activate);
			rplatPhys(player2.h, rplat[i].h, rplat[i].w, rplat[i].pos[0], rplat[i].pos[1], player2.pos[0], 
					&player2.pos[1], &onPlat2, &top2, &activate2);


			// if (player1.pos[0] >= rplat[i].pos[0] - rplat[i].w &&
			// 		player1.pos[0] <= rplat[i].pos[0] + rplat[i].w &&
			// 		player1.pos[1] - 100.0f >= rplat[i].pos[1] - rplat[i].h &&
			// 		player1.pos[1] - 100.0f <= rplat[i].pos[1] + rplat[i].h) {
			// 	// The player is on this platform, so set their vertical position
			// 	// to the top of the platform
			// 	if(!top){
			// 		player1.pos[1] = rplat[i].pos[1] + rplat[i].h/2 + 
			// 			player1.h/2;


			// 	}
			// 	prevI = i;
			// 	onPlat = true;
			// 	activate = 1;


			// }
			// if (player2.pos[0] >= rplat[i].pos[0] - rplat[i].w &&
			// 		player2.pos[0] <= rplat[i].pos[0] + rplat[i].w &&
			// 		player2.pos[1] - 100.0f >= rplat[i].pos[1] - rplat[i].h &&
			// 		player2.pos[1] - 100.0f <= rplat[i].pos[1] + rplat[i].h) {
			// 	// The player is on this platform, so set their vertical position
			// 	// to the top of the platform
			// 	if(!top2){
			// 		player2.pos[1] = rplat[i].pos[1] + rplat[i].h/2 + 
			// 			player2.h/2;


			// 	}
			// 	prevI = i;
			// 	onPlat2 = true;
			// 	activate2 = 1;


			// }


		}


		rplatPhys2(&player1.pos[1],&player1.vel[1], &onPlat, &top, &stop, &activate, g.yres);
		rplatPhys2(&player2.pos[1],&player2.vel[1], &onPlat2, &top2, &stop2, &activate2, g.yres);
		// if (onPlat) {
		// 	// Player is on a platform, so their vertical velocity should be zero
		// 	player1.vel[1] = 0.0f;
		// 	top = true;


		// } else {

		// 	if(activate == 1){
		// 		// Player is not on a platform, so make them fall
		// 		player1.vel[1] -= gRav;
		// 		// Check if player has reached the bottom of the screen
		// 		if (player1.pos[1] <= 0.0f) {
		// 			// Player has fallen off the bottom of the screen, so reset their position
		// 			if(!stop){
		// 				player1.pos[1] = g.yres/2 - 405.0f;
		// 				player1.vel[1] = 0.0;
		// 				stop = true;
		// 				activate = 0;
		// 			}
		// 		}

		// 		stop = false;


		// 	}
		// }


		// if (onPlat2) {
		// 	// Player is on a platform, so their vertical velocity should be zero


		// 	player2.vel[1] = 0.0f;
		// 	top2 = true;


		// } else {


		// 	if(activate2 == 1){
		// 		// Player is not on a platform, so make them fall
		// 		player2.vel[1] -= gRav;
		// 		// Check if player has reached the bottom of the screen
		// 		if (player2.pos[1] <= 0.0f) {
		// 			// Player has fallen off the bottom of the screen, so reset their position
		// 			if(!stop2){
		// 				player2.pos[1] = g.yres/2 - 405.0f;
		// 				player2.vel[1] = 0.0;
		// 				stop2 = true;
		// 				activate2 = 0;
		// 			}
		// 		}

		// 		stop2 = false;

		// 	}
		// }



	}

	extern void chrgPhys(int *pBar, bool *chrg);



	//Geno - physics for charge up
	if (g.keyStates[XK_c] && player1.pBar <= 150 && player1.pBar >= 0 && player1.block != 1) {

		chrgPhys(&player1.pBar, &chargeUp);
		rndrPbar = true;
	} else {
		chargeUp = false;
	}



	if (g.keyStates[XK_slash] && player2.pBar <= 150 && player2.pBar >= 0 && player2.block != 1) {

		chrgPhys(&player2.pBar, &chargeUp2);
		rndrPbar2 = true;
	} else {
		chargeUp2 = false;
	}

	once = false;



}


bool brend = true;


void render(void)
{
	if(g.start == 1){
		//Clear the screen
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		//float cx = g.xres/3.0;
		//float cy = g.yres/2.0;

		/*if (g.gflag == 0) {
		  Rect r;
		  unsigned int c = 0x0000ff00;
		  r.bot = g.yres/2;
		  r.left = g.xres/2;
		  r.center = 50;
		  ggprint8b(&r, 16, c, "Feature Modes: ");
		  ggprint8b(&r, 16, c, "Geno's Feature Mode: 1");
		  ggprint8b(&r, 16, c, "Jose's Feature Mode: SHIFT-J");
		  ggprint8b(&r, 16, c, "Brian's Feature Mode: 2");
		  ggprint8b(&r, 16, c, "Jesse's Feature Mode: 3");
		  }*/

		//Display Background
		/*glColor3f(1.0, 1.0, 1.0);
		  glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
		  display_map(g.tex.xc[0], g.tex.xc[1],
		  g.tex.yc[0], g.tex.yc[1],
		  g.xres, 	 g.yres);*/

		//Centers Background; 
		//will combine with ^this later
		if (g.mapCenter != 0) {
			g.tex.xc[0] = 0.083;
			g.tex.xc[1] = 1.083;
			g.mapCenter = 0;
		}

		//BACKGROUND 1
		if (g.loadMap == 1) {
			g.time_scroll++;
			if (g.time_scroll % 6 == 0) {
				if (g.scroll == 1) {
					g.scroll = 0;
					g.tex.yc[0] = 0.0f;
					g.tex.yc[1] = 0.125f;
				}
				g.scroll += 0.125;
				g.tex.yc[0] += g.scroll;
				g.tex.yc[1] += g.scroll;
			}
			displayBackground(g.tex.xc[0], g.tex.xc[1],
					g.tex.yc[0], g.tex.yc[1],
					g.xres, g.yres,
					img[1].width, img[1].height,
					g.tex.map1Texture, img[1].data);
		}

		//glClear(GL_COLOR_BUFFER_BIT);
		//static float pos[2] = {g.xres/2.0f, g.yres/2.0f};
		//Player1:
		if (player1.dead == 1) {
			player_hitbox(player1.h, player1.w, player1.pos[0], player1.w);
			restartScreen(1, g.yres, g.xres);
			player1.pBar = 0;
		} else {
			//player_hitbox(player1.w, player1.h, player1.pos[0], player1.pos[1]);
			// Player 1 punch box
			if(player1.sPunch == 0){
				if (player1.punch == 1 ) {
					int pw2 = player1.pw2 * g.punchflip;
					int pw1 = player1.pw1 * g.punchflip;
					if (player1.weapon==1) {
						//call function to swing sword
						useWeapon(player1.w, player1.h, 
								player1.pos[0], player1.pos[1], 
								g.punchflip, player1.weapon);
					} else {
                        if (g.joflag == 1) {
						    punch_hitbox(pw2, pw1, 
							    	player1.ph, player1.pos[0], 
							    	player1.pos[1] + 40.0f);
                        }
					}
				}
			}

			//Player 1 sword
			if (player1.weapon==1 && player1.punch == 0 && !player1.block) {
				holdingWeapon(player1.w, player1.h, 
						player1.pos[0], player1.pos[1], 
						g.punchflip, player1.weapon); 
			}

			// Player 1 block
			if (player1.block) {
				playerBlocking(player1.w, player1.h, 
						player1.pos[0], player1.pos[1], g.punchflip);
			}
		}
		//Player2:
		if (player2.dead == 1) {
			player_hitbox(player2.h, player2.w, player2.pos[0], player2.w);
			restartScreen(2, g.yres, g.xres);
			player2.pBar = 0;
		} else {
			//player_hitbox(player2.w, player2.h, player2.pos[0], player2.pos[1]);
			// Player 2 punch box
			if (player2.punch == 1) {
				int pw2 = player2.pw2 * g.punchflip;
				int pw1 = player2.pw1 * g.punchflip;
				if (player2.weapon == 1) {//call function to swing sword
					useWeapon(player2.w, player2.h, 
							player2.pos[0], player2.pos[1], 
							g.punchflip*-1, player2.weapon);
				} else {
                    if (g.joflag == 1) {
	    				punch_hitbox(pw1, pw2, 
		    					player2.ph, player2.pos[0], 
			    				player2.pos[1] + 40.0f);
                    }
				}
			}
			//Player 2 sword
			if (player2.weapon==1 && player2.punch == 0 && !player2.block) {
				holdingWeapon(player2.w, player2.h, 
						player2.pos[0], player2.pos[1], 
						g.punchflip*-1, player2.weapon); 
			}

			// Player 2 block
			if (player2.block) {
				playerBlocking(player2.w, player2.h, 
						player2.pos[0], player2.pos[1], g.punchflip*-1);
			}
		}

	//Jesse- This resets the players stats 
    //       if the users wants to restart the game
	restartGame(&player1.health, &player2.health, &player1.pos[0], &player2.pos[0],
                &player1.dead, &player2.dead, &player1.weapon, &player2.weapon,
                &player1.timeLimit, &player2.timeLimit, &g.restart,
                &g.nextMysteryBox, &g.mysteryBoxSpawn, g.xres);

    //Spawn Mystery Box
	spawnMysteryBox(&g.mysteryBoxSpawn, &g.nextMysteryBox, &player1.timeLimit,
                    &player1.weapon, &player2.timeLimit, &player2.weapon,
                    player1.health, player2.health, g.xres,
                    g.jeflag, player1.pos[0], player2.pos[0],
                    player1.pos[1], player2.pos[1]);

	//render power up for player 1
	if ((time(NULL) - player1.timeLimit) <= 4) {
		//render for 4 seconds
		showPowerUp(player1.weapon, g.yres, g.xres); 
	}
	//render power up for player 2 
	if ((time(NULL) - player2.timeLimit) <= 4) {
		//render for 4 seconds
		showPowerUp(player2.weapon, g.yres, g.xres); 
	}

		//JOSE: I think this is part of Sprite stuff
		if(g.bflag == 1)
		{
			float cx = g.xres/3.0;
                	float cy = g.yres/3.0;
			float h = 250.0;
                        float w = h;
			//Display player1
                        glPushMatrix();
                        glColor3f(1.0, 1.0, 1.0);
                        glBindTexture(GL_TEXTURE_2D, g.walkTexture);
                        glEnable(GL_ALPHA_TEST);
                        glAlphaFunc(GL_GREATER, 0.0f);
                        glColor4ub(255,255,255,255);
                        int numColumns = 8;
                        int numRows = 5;
                        int ix = g.walkFrame % numColumns;
                        int iy = g.walkFrame / numColumns;

                        float tx = (float)ix / (float)numColumns;
                        float ty = (float)iy / (float)numRows;
                        float tw = 1.0f / (float)numColumns;
                        float th = 1.0f / (float)numRows;

                        glBegin(GL_QUADS);
			glTexCoord2f(tx, ty + th);      glVertex2i(cx - w, cy - h - 70);
			glTexCoord2f(tx, ty);           glVertex2i(cx - w, cy + h - 70);
			glTexCoord2f(tx + tw, ty);      glVertex2i(cx + w, cy + h - 70);
			glTexCoord2f(tx + tw, ty + th); glVertex2i(cx + w, cy - h - 70);
                        glEnd();
                        glPopMatrix();
                        glBindTexture(GL_TEXTURE_2D, 0);
                        glDisable(GL_ALPHA_TEST);
			//display Player2
			if(g.keyStates[XK_k] && !chargeUp2) {
				float cx3 = g.xres/1.5;
                		float cy3 = g.yres/4.0;
                        	float w2 = h+50;
                	        glPushMatrix();
        	                glColor3f(1.0, 1.0, 1.0);
	                        glBindTexture(GL_TEXTURE_2D, g.speTexture);

                        	//JOSE: Sprite stuff; can use later
                        	glEnable(GL_ALPHA_TEST);
                        	glAlphaFunc(GL_GREATER, 0.0f);
                	        glColor4ub(255,255,255,255);
        	                int numColumns3 = 4;
	                        int numRows3 = 1;
                        	int ix2 = g.speFrame % numColumns3;
                        	int iy2 = g.speFrame / numColumns3;

                	        float tx3 = (float)ix2 / (float)numColumns3;
        	                float ty3 = (float)iy2 / (float)numRows3;
	                        float tw3 = 1.0f / (float)numColumns3;
                        	float th3 = 1.0f / (float)numRows3;

                        	glBegin(GL_QUADS);
		       		glTexCoord2f(tx3 + tw3, ty3 + th3);	glVertex2i(cx3 - w2, cy3 - h);
			    	glTexCoord2f(tx3 + tw3, ty3);		glVertex2i(cx3 - w2, cy3 + h);
				glTexCoord2f(tx3, ty3);			glVertex2i(cx3 + w2, cy3 + h);
				glTexCoord2f(tx3, ty3 + th3);		glVertex2i(cx3 + w2, cy3 - h);

                        	glEnd();
                	        glPopMatrix();
        	                glBindTexture(GL_TEXTURE_2D, 0);
	                        glDisable(GL_ALPHA_TEST);
			}else {
				float cx2 = g.xres/1.5;
                		float cy2 = g.yres/4.0;
                	        glPushMatrix();
        	                glColor3f(1.0, 1.0, 1.0);
	                        glBindTexture(GL_TEXTURE_2D, g.walk2Texture);

                        	//JOSE: Sprite stuff; can use later
                        	glEnable(GL_ALPHA_TEST);
                        	glAlphaFunc(GL_GREATER, 0.0f);
                	        glColor4ub(255,255,255,255);
        	                int numColumns2 = 8;
	                        int numRows2 = 5;
                        	int ix2 = g.walk2Frame % numColumns2;
                        	int iy2 = g.walk2Frame / numColumns2;

                	        float tx2 = (float)ix2 / (float)numColumns2;
        	                float ty2 = (float)iy2 / (float)numRows2;
	                        float tw2 = 1.0f / (float)numColumns2;
                        	float th2 = 1.0f / (float)numRows2;

                        	glBegin(GL_QUADS);
		       		glTexCoord2f(tx2 + tw2, ty2 + th2);	glVertex2i(cx2 - w, cy2 - h);
			    	glTexCoord2f(tx2 + tw2, ty2);		glVertex2i(cx2 - w, cy2 + h);
				glTexCoord2f(tx2, ty2);			glVertex2i(cx2 + w, cy2 + h);
				glTexCoord2f(tx2, ty2 + th2);		glVertex2i(cx2 + w, cy2 - h);

                        	glEnd();
                	        glPopMatrix();
        	                glBindTexture(GL_TEXTURE_2D, 0);
	                        glDisable(GL_ALPHA_TEST);
			}
		}


		if(g.bflag == 0) {
			// Players flip
			if(player1.pos[0] > player2.pos[0]) {
				if (player1.dead == 0) {
					Player_1_fliped(player1.w + player1.pos[0], player1.h + player1.pos[1], 
							g.walkFrame, g.walkTexture);
				}
				if (player2.dead == 0) {
					if(g.keyStates[XK_k] && !chargeUp2) {
						Player_2_spe(player2.w + player2.pos[0], player2.h + player2.pos[1], 
								g.speFrame, g.speTexture);
					}else {
						Player_2(player2.w + player2.pos[0], player2.h + player2.pos[1],
							       	g.walk2Frame, g.walk2Texture);
					}
				}
			}
			
			else if (player1.pos[0] < player2.pos[0]) {
				if (player1.dead == 0) {
					Player_1(player1.w + player1.pos[0], player1.h + player1.pos[1],
						       	g.walkFrame, g.walkTexture);
				}
				if(g.keyStates[XK_k] && !chargeUp2) {
					if (player2.dead == 0) {
						Player_2_spe_fliped(player2.w + player2.pos[0], player2.h + player2.pos[1], 
							g.speFrame, g.speTexture);
					}
				}else {
					if (player2.dead == 0) {
						Player_2_fliped(player2.w + player2.pos[0], player2.h + player2.pos[1], 
							g.walk2Frame, g.walk2Texture);
					}
				}
			}
		}

		//JOSE: I JUST HAVE TO KEEP THIS; OTHERWISE, MAP MOVING DOENS'T WORK
		//if (!g.feature_mode) {
		//	extern void display_controls(int wf, int yres);
		//    display_controls(g.walkFrame, g.yres);
		//}

		//
		if(g.gflag == 1) {	




			// extern void newText(int yres, int xres);
			// newText(g.yres, g.xres);

			//glColor3f(0.5, 0.5, 0.5);
			//glRecti(300, 400, 500, 450);
			//glColor3f(1, 1, 1);
			//glRasterPos2i(350, 420);
		}


		unsigned char c4[3] = {0, 128, 0};

		extern void health(float w, float h, unsigned char color[3], float pos0, float pos1, int player, int health);

		//Geno, Jesse health bar for players

		hbar[0].set_width(player1.health);
		hbar[0].set_height(20.0f);
		hbar[0].set_xres(g.xres - 1080.0f);
		hbar[0].set_yres(g.yres + 900.0f);
		hbar[0].set_color(c4);

		hbar[1].set_width(player2.health);
		hbar[1].set_height(20.0f);
		hbar[1].set_xres(g.xres + 1080.0f);
		hbar[1].set_yres(g.yres + 900.0f);
		hbar[1].set_color(c4);










		health(hbar[0].w, hbar[0].h, hbar[0].color, hbar[0].pos[0], hbar[0].pos[1], 1, player1.health);
		health(hbar[1].w, hbar[1].h, hbar[1].color, hbar[1].pos[0], hbar[1].pos[1], 2, player2.health);
		//GENO - Random Platforms 

		rplat[0].set_width(150.0f);
		rplat[0].set_height(25.0f);
		rplat[1].set_width(150.0f);
		rplat[1].set_height(25.0f);
		rplat[2].set_width(150.0f);
		rplat[2].set_height(25.0f);


		//top
		rplat[0].set_xres(g.xres + 25.0f);
		rplat[0].set_yres(g.yres + 130.0f);

		//right
		rplat[1].set_xres(g.xres + 1700.0f);
		rplat[1].set_yres(g.yres - 50.0f);

		//left 
		rplat[2].set_xres(g.xres - 1000.0f);
		rplat[2].set_yres(g.yres - 50.0f );


		unsigned char c3[3] = {184, 2, 2};


		extern void rForms(float w, float h, unsigned char color[3], float pos0, float pos1);

		if((player1.health >= 100 && player1.health <= 130) || (player2.health >= 100 && player2.health <= 130)){


			pltFlg = 1;


			for(int i = 0; i < 3; i++){


				rplat[i].set_color(c3);

				rForms(rplat[i].w, rplat[i].h,  rplat[i].color, rplat[i].pos[0], rplat[i].pos[1]);
			}

		} else if ((player1.health >= 60 && player1.health <= 90) || (player2.health >= 60 && player2.health <= 90)){

			pltFlg = 1;


			//top
			rplat[0].set_xres(g.xres - 1300.0f);
			rplat[0].set_yres(g.yres + 400.0f);

			//right
			rplat[1].set_xres(g.xres + 900.0f);
			rplat[1].set_yres(g.yres - 60.0f);

			//left 
			rplat[2].set_xres(g.xres - 450.0f);
			rplat[2].set_yres(g.yres - 60.0f );

			for(int i = 0; i < 3; i++){

				unsigned char c3[3] = {128, 128, 128};

				rplat[i].set_color(c3);

				rForms(rplat[i].w, rplat[i].h,  rplat[i].color, rplat[i].pos[0], rplat[i].pos[1]);
			}
		}  else if ((player1.health >= 20 && player1.health <= 50) || (player2.health >= 20 && player2.health <= 50)){

			pltFlg = 1;


			//top
			rplat[0].set_xres(g.xres - 400.0f);
			rplat[0].set_yres(g.yres + 400.0f);

			//right
			rplat[1].set_xres(g.xres + 1300.0f);
			rplat[1].set_yres(g.yres - 50.0f);

			//left 
			rplat[2].set_xres(g.xres - 1200.0f);
			rplat[2].set_yres(g.yres  - 50.0f );

			for(int i = 0; i < 3; i++){

				unsigned char c3[3] = {184, 2, 2};

				rplat[i].set_color(c3);

				rForms(rplat[i].w, rplat[i].h,  rplat[i].color, rplat[i].pos[0], rplat[i].pos[1]);
			}
		} else{
			pltFlg = 0;
		}



		//
		if(g.jeflag == 1) {
			//Jesses function
			extern void greenBoxes(int ywin, int xwin);
			greenBoxes(g.yres, g.xres);
		}
		//

		//
		if(g.joflag == 1) {
			//Joses function
			fmBorder(g.xres, g.yres);
            fmHeader(g.xres, g.yres);

            //PLAYER 1 HITBOX
		    player_hitbox(player1.w, player1.h, 
                          player1.pos[0], player1.pos[1]);

            //PLAYER 2 HITBOX
			player_hitbox(player2.w, player2.h, 
                          player2.pos[0], player2.pos[1]);
        }




		unsigned char c5[3] = {0, 0, 255};



		powerBar[0].set_width(player1.pBar);
		powerBar[0].set_height(15.0f);
		powerBar[0].set_xres(g.xres - 1080.0f);
		powerBar[0].set_yres(g.yres + 830.0f);
		powerBar[0].set_color(c5);



		powerBar[1].set_width(player2.pBar);
		powerBar[1].set_height(15.0f);
		powerBar[1].set_xres(g.xres + 1080.0f);
		powerBar[1].set_yres(g.yres + 830.0f);
		powerBar[1].set_color(c5);


		if(rndrPbar){

			powBar(powerBar[0].w, powerBar[0].h, powerBar[0].color, powerBar[0].pos[0],powerBar[0].pos[1]);
		}

		if(rndrPbar2){
			powBar(powerBar[1].w, powerBar[1].h, powerBar[1].color, powerBar[1].pos[0],powerBar[1].pos[1]);
		}

		if(player1.sPunch == 1) {
			int pw2 = player1.pw2 * g.punchflip;
			int pw1 = player1.pw1 * g.punchflip;
			superPunch(pw1, pw2, player1.ph, player1.pos[0], player1.pos[1], g.punchflip, 1);
		}

		if(player2.sPunch == 1) {
			int pw2 = player2.pw2 * g.punchflip;
			int pw1 = player2.pw1 * g.punchflip;
			superPunch(pw1, pw2, player2.ph, player2.pos[0], player2.pos[1], g.punchflip, 2);
		}


	} else if (g.start == 0) {

		//Clear the screen


		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		strMenu(g.yres, g.xres);


	} else if (g.start == 2){
		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		cntrlMenu(g.yres, g.xres);
	}

	if(g.ctrls == 1){
		cntrlMenu(g.yres, g.xres);
	}
}
