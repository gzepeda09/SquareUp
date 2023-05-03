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
extern void display_map(float x0, float x1, float y0, float y1, int xres, int yres);
extern void player_hitbox(int w, int h, float x, float y);
extern void punch_hitbox(int w1, int w2, int h, float x, float y);
//--- Jesse ---
extern void restartScreen(int player, int ywin, int xwin);
extern void playerBlocking(float w, float h, float x, float y, int flipped);

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
Image img[2] = {"images/scott.gif",
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
        int dead = 0;
        int block = 0;
        float w = 20.0f;
        float h = 100.0f;
        float pw1 = 20.0f;
        float pw2 = 100.0f;
        float ph = 10.0f;
        int punch;
        float punchcooldown = 250.0f;
        int health = 100;
} player1;

class Player_2 {
    public:
        Vec pos;
        Vec vel;
        int dead = 0;
        int block = 0;
        float w = 20.0f;
        float h = 100.0f;
        float pw1 = 20.0f;
        float pw2 = 100.0f;
        float ph = 10.0f;
        int punch;
        float punchcooldown = 250.0f;
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
        int restart;
        GLuint walkTexture;
        //GLuint map1Texture;
        int mapCenter;
        int punchflip = 1;
        Texture tex;
        char keyStates[65536];
        Vec box[20];
        Global() {
            done=0;
            xres=1920;
            yres=1080;
            walk=0;
            mapCenter = 1;
            bflag = jeflag = joflag = 0;
            gflag = 1;
            restart = 0;
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


//Geno -  random platform for 
Box rplat[3];

// Box rpl[3] = {Box(g.yres, g.xres), Box(g.yres, g.xres), Box(g.yres, g.xres)};

//Jesse - added values to reset character stats
class Reset_Values {
    public:
        int dead = 0;
        float player1posX = (float)g.xres/4;
        float player2posX = (float)g.xres/1.3;
        int health = 100;
} reset;

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
    MakeVector((float)g.xres/1.3,100.0f,0.0, player2.pos);
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

    // checks keys
    (void)shift;
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
            //Brian: change the flags to "flag =! flag;" make it toggle with less lines of code
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
                //Brian: change the flags to "flag =! flag;" make it toggle with less lines of code
                g.joflag =! g.joflag;
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
        case XK_r:
            g.restart =! g.restart;
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

    // Move left player 1
    if(g.keyStates[XK_a] && player1.dead == 0) {
        if (player1.pos[0] > player1.w) {
            player1.pos[0] -= player1.vel[0];
            std::cout << player1.pos[0] << std::endl;
        }
        else if (player1.pos[0] <= 20.0f && g.tex.xc[0] >= 0) {
            g.tex.xc[0] -= 0.001;
            g.tex.xc[1] -= 0.001;
        }
    }

    // Move right player 1
    if (g.keyStates[XK_d] && player1.dead == 0) {
        if (player1.pos[0] < (float)g.xres - player1.w) {
            player1.pos[0] += player1.vel[0];
            std::cout << player1.pos[0] << std::endl;
        }
        else if (player1.pos[0] >= 1420.0f && g.tex.xc[0] <= 0.166) {
            g.tex.xc[0] += 0.001;
            g.tex.xc[1] += 0.001;
        }

    }

    // Jump player 1
    if (g.keyStates[XK_w] && player1.vel[1] == 0 && player1.pos[1] != 600.0f && player1.dead == 0) {
        player1.vel[1] = 200.0f;
        player1.vel[2] = 200.0f;
        //player1.pos[1] += 500.0ff;
        std::cout << "W key pressed" << std::endl;
    }

    if (player1.vel[2] != 0) {
        player1.vel[2] -= 5.0f;
        player1.pos[1] += 12.5f;
    }

    // Gravity player 1
    if (player1.vel[1] != 0 && player1.vel[2] == 0.0f) {
        std::cout << "yo1" << std::endl;
        player1.vel[1] -= 5.0f;
        player1.pos[1] -= 12.5f;
    }

    //JOSE: apparently after a jump the box doesn't go exactly back to original spot; this would fix it if need be
    /*if (player1.pos[1] < 100.0f) {
      player1.vel[1] = 0;
      player1.pos[1] = 100.0f;
      }*/

    // Punch player 1
    if (g.keyStates[XK_b] && player1.punch == 0 && player1.dead == 0) {

        //Functions can be found in gzepeda.cpp
            extern void playPunchSound();
            playPunchSound();

        player1.punch = 1;

        // Punch Detection player 1
        if (player1.pos[0] + player1.pw2 >= player2.pos[0] + (-player2.w) && player1.pos[0] < player2.pos[0]) {
            std::cout << "Player 1 hits Player 2!" << std::endl;
            //player2.health -= 10;
            //For testing. delete once testing is done
            if (g.jeflag == 1 && player2.dead == 0) {
                player2.health -= 50;
            } else if (player2.dead == 0){
                if (!player2.block) {    
                    player2.health -= 10;
                } else {
                    player2.health -=5;
                }
                //Jesse - reaction to punches
                player2.pos[1] += 50.0f;
                player2.vel[1] += 20.0f;
                player2.pos[0] += 35.0f;
            }
            //player 1 dies
            if (player2.health <= 0) {
                player2.dead = 1;
            }
        }
        // Punch Detection (Flipped)
        else if (player1.pos[0] - player1.pw2 <= player2.pos[0] + (player2.w) && player1.pos[0] > player2.pos[0]) {
            std::cout << "Player 1 hits Player 2!" << std::endl;
            //player2.health -= 10;
            //For testing. delete once testing is done
            if (g.jeflag == 1 && player2.dead == 0) {
                player2.health -= 50;
            } else if (player2.dead == 0){
                if (!player2.block) {    
                    player2.health -= 10;
                } else {
                    player2.health -=5;
                }
                //Jesse - reaction to punches
                player2.pos[1] += 50.0f;
                player2.vel[1] += 20.0f;
                player2.pos[0] -= 35.0f;
            }
            //player 1 dies
            if (player2.health <= 0) {
                player2.dead = 1;
                player2.health = 0;
            }

        }
    }

    // Block player 1
    if (g.keyStates[XK_t] && player1.dead == 0 && player1.punch == 0) {
        //printf("player1 blocking");
        player1.block = 1;
    } else {
        player1.block = 0;
    }

    // Punch cooldown player 1
    if (player1.punch == 1) {
        if (player1.punchcooldown == 0) {
            player1.punch = 0;
            player1.punchcooldown = 250.0f;
        }
        else {
            player1.punchcooldown -= 10.0f;
        }
    }

    // --Player 2 Movement & Abilites--

    // Move left player 2
    if (g.keyStates[XK_Left] && player2.dead == 0) {
        if (player2.pos[0] > player2.w) {
            player2.pos[0] -= player2.vel[0];
        }
        else if (player2.pos[0] <= 20.0f && g.tex.xc[0] >= 0) {
            g.tex.xc[0] -= 0.001;
            g.tex.xc[1] -= 0.001;
        }
    }

    // Move right player 2
    if (g.keyStates[XK_Right] && player2.dead == 0) {
        if (player2.pos[0] < (float)g.xres - player2.w) {
            player2.pos[0] += player2.vel[0];
        }
        else if (player2.pos[0] >= 1420.0f && g.tex.xc[0] <= 0.166) {
            g.tex.xc[0] += 0.001;
            g.tex.xc[1] += 0.001;
        }
    }

    // Jump player 2
    if (g.keyStates[XK_Up] && player2.vel[1] == 0 && player2.pos[1] != 600.0f && player2.dead == 0) {
        player2.vel[1] = 200.0f;
        player2.vel[2] = 200.0f;
        //player2.pos[1] += 500.0f;
        std::cout << "Up Arrow key pressed" << std::endl;
    }

    if (player2.vel[2] != 0) {
        player2.vel[2] -= 5.0f;
        player2.pos[1] += 12.5f;
    }

    // Gravity player 2
    if (player2.vel[1] != 0 && player2.vel[2] == 0.0f) {
        std::cout << "Down" << std::endl;
        player2.vel[1] -= 5.0f;
        player2.pos[1] -= 12.5f;
    }


    // Punch player 2
    if (g.keyStates[XK_m] && player2.punch == 0 && player2.dead == 0) {
        player2.punch = 1;

        // Punch detection player 2
        if (player2.pos[0] - player2.pw2 <= player1.pos[0] + (player1.w) && player2.pos[0] > player1.pos[0]) {
            std::cout << "Player 2 hits Player 1!" << std::endl;
            //player1.health -= 10;
            //For testing. delete once testing is done
            if (g.jeflag == 1 && player1.dead == 0) {
                player1.health -= 50;
            } else if (player1.dead == 0){
                if (!player1.block) {    
                    player1.health -= 10;
                } else {
                    player1.health -=5;
                }
                //Jesse - reaction to punches
                player1.pos[1] += 50.0f;
                player1.vel[1] += 20.0f;
                player1.pos[0] -= 35.0f;
            }
            //player 1 dies
            if (player1.health <= 0) {
                player1.dead = 1;
            }
        }
        // Punch Detection (Flipped)
        else if (player2.pos[0] + player2.pw2 >= player1.pos[0] + (-player1.w) && player2.pos[0] < player1.pos[0]) {
            std::cout << "Player 1 hits Player 2!" << std::endl;
            //player1.health -= 10;
            //For testing. delete once testing is done
            if (g.jeflag == 1 && player1.dead == 0) {
                player1.health -= 50;
            } else if (player1.dead == 0) {
                if (!player1.block) {    
                    player1.health -= 10;
                } else {
                    player1.health -=5;
                }
                //Jesse - raction to punches
                player1.pos[1] += 50.0f;
                player1.vel[1] += 20.0f;
                player1.pos[0] += 35.0f;
            }
            //player 1 dies
            if (player1.health <= 0) {
                player1.dead = 1;
                player1.health = 0;
            }
        }

    }

    // Block player 2
    if (g.keyStates[XK_l] && player2.dead == 0 && player2.punch == 0) {
        //printf("player1 blocking");
        player2.block = 1;
    } else {
        player2.block = 0;
    }

    // Punch cooldown player 2
    if (player2.punch == 1) {
        if (player2.punchcooldown == 0) {
            player2.punch = 0;
            player2.punchcooldown = 250.0f;
        }
        else {
            player2.punchcooldown -= 10.0f;
        }
    }

    // -- Misc --

    // Players flip
    if (player1.pos[0] > player2.pos[0]) {
        g.punchflip = -1;
    }
    else if (player1.pos[0] < player2.pos[0]) {
        g.punchflip = 1;
    }



}


    bool brend = true;


void render(void)
{
    //Clear the screen
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    float cx = g.xres/3.0;
    float cy = g.yres/2.0;

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
      ggprint8b(&r, 16, c, "Jesse's Feature Mode: J");
      }*/

    //Display Background
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
    display_map(g.tex.xc[0], g.tex.xc[1],
            g.tex.yc[0], g.tex.yc[1],
            g.xres, 	 g.yres);

    //Centers Background; 
    //will combine with ^this later
    if (g.mapCenter != 0) {
        g.tex.xc[0] = 0.083;
        g.tex.xc[1] = 1.083;
        g.mapCenter = 0;
    }

    //glClear(GL_COLOR_BUFFER_BIT);
    //static float pos[2] = {g.xres/2.0f, g.yres/2.0f};
    //Player1:
    if (g.jeflag == 1 && player1.dead == 1) { //Jesse- replace once postion function is made
		player_hitbox(player1.h, player1.w, player1.pos[0], player1.w);
        restartScreen(1, g.yres, g.xres);        //player no longer lies down ehrn they die?
    } else {
		player_hitbox(player1.w, player1.h, player1.pos[0], player1.pos[1]);
        // Player 1 punch box
        if (player1.punch == 1) {
            int pw2 = player1.pw2 * g.punchflip;
        	int pw1 = player1.pw1 * g.punchflip;
        	punch_hitbox(pw2, pw1, player1.ph, player1.pos[0], player1.pos[1] + 40.0f);
        }
        // Player 1 block
        if (player1.block) {
            playerBlocking(player1.w, player1.h, player1.pos[0], player1.pos[1], g.punchflip);
        }
    }
    //Player2:
    if (g.jeflag == 1 && player2.dead == 1) { //Jesse- replace once postion function is made
		player_hitbox(player2.h, player2.w, player2.pos[0], player2.w);
        restartScreen(2, g.yres, g.xres);
    } else {
		player_hitbox(player2.w, player2.h, player2.pos[0], player2.pos[1]);
        // Player 2 punch box
        if (player2.punch == 1) {
			int pw2 = player2.pw2 * g.punchflip;
        	int pw1 = player2.pw1 * g.punchflip;
        	punch_hitbox(pw1, pw2, player2.ph, player2.pos[0], player2.pos[1] + 40.0f);
        }
        // Player 2 block
        if (player2.block) {
            playerBlocking(player2.w, player2.h, player2.pos[0], player2.pos[1], g.punchflip*-1);
        }
    }

    //Jesse- This resets the players stats if the users wants to restart the game
    if ( (player1.dead == 1 || player2.dead == 1) && g.restart == 1) {
            player1.health = reset.health;
            player2.health = reset.health;
            player1.pos[0] = reset.player1posX;
            player2.pos[0] = reset.player2posX;
            player1.dead = reset.dead;
            player2.dead = reset.dead;
            g.restart = 0;
        }

    //JOSE: I think this is part of Sprite stuff
    if (g.bflag == 1) {
        float h = 250.0;
        float w = h * 0.5;
        glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glBindTexture(GL_TEXTURE_2D, g.walkTexture);

        //JOSE: Sprite stuff; can use later
        glEnable(GL_ALPHA_TEST);
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
        glDisable(GL_ALPHA_TEST);
    }

    //JOSE: I JUST HAVE TO KEEP THIS; OTHERWISE, MAP MOVING DOENS'T WORK
    //if (!g.feature_mode) {
    //	extern void display_controls(int wf, int yres);
    //    display_controls(g.walkFrame, g.yres);
    //}

    //
    if(g.gflag == 1) {	

        //Genos functions
        extern void playSound();
        playSound();


        // extern void newText(int yres, int xres);
        // newText(g.yres, g.xres);

        //glColor3f(0.5, 0.5, 0.5);
        //glRecti(300, 400, 500, 450);
        //glColor3f(1, 1, 1);
        //glRasterPos2i(350, 420);
    }
    //

    //GENO - Random Platforms 



                                        float x = (std::rand() % static_cast<int>(g.xres)) - g.xres/2;
            float y = (std::rand() % static_cast<int>(g.yres)) - g.yres/2;



          


            rplat[0].set_width(100.0f);
            rplat[0].set_height(25.0f);
            rplat[1].set_width(100.0f);
            rplat[1].set_height(25.0f);
            rplat[2].set_width(100.0f);
            rplat[2].set_height(25.0f);
            rplat[0].set_xres(g.xres + x);
            rplat[0].set_yres(g.yres + y);
            
            rplat[1].set_xres(g.xres + x + 2.0f);
            rplat[1].set_yres(g.yres + y - 2.5f);

            rplat[2].set_xres(g.xres + x + 3.5f);
            rplat[2].set_yres(g.yres + y );
    


    if(player1.health <= rNum || player2.health <= rNum){

        for(int i = 0; i < 3; i++){




            unsigned char c3[3] = {184, 2, 2};

            rplat[i].set_color(c3);




                glPushMatrix();
                glColor3ubv(rplat[i].color);
                glTranslatef(rplat[i].pos[0], rplat[i].pos[1], 0.0f);
                glBegin(GL_QUADS);
                    glVertex2f(-rplat[i].w, -rplat[i].h);
                    glVertex2f(-rplat[i].w,  rplat[i].h);
                    glVertex2f( rplat[i].w,  rplat[i].h);
                    glVertex2f( rplat[i].w, -rplat[i].h);
                glEnd();
                glPopMatrix();
            }

        }




    //
    if(g.jeflag == 1) {
        //Jesses function
        extern void greenBoxes(int ywin, int xwin);
        greenBoxes(g.yres, g.xres);
    }
    //

    //
    if (g.joflag == 1) {
        //Joses function
        extern void fmBorder(int xres, int yres);
        fmBorder(g.xres, g.yres);

        //extern void test_text (int xres, int yres);
        //test_text(g.xres, g.yres);

        extern void display_controls(int wf, int yres);
        display_controls(g.walkFrame, g.yres);
    }
    if (g.bflag == 1) {
    }

    Rect r;
    unsigned int c = 0x0000ff00;
    r.bot = g.yres - 20;
    r.left = g.xres/2;
    r.center = 50;
    ggprint8b(&r, 16, c, "Player 1 Health: %i", player1.health);
    ggprint8b(&r, 16, c, "Player 2 Health: %i", player2.health);

}








