//program: jzamora2.cpp || Source file for project
//author: Jose Zamora //
//date: 02/21/2022

#include "fonts.h"
#include "jzamora2.h"
#include <iostream>
#include <cstdlib>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <cstring>

//-----------------------------------------------
//GENERAL TESTING CLASS
//-----------------------------------------------
class Global {
    public:
        int i = 0;
        int c1 = 0;
        int c2 = 255;
        int c3 = 0;
} glu; 

//-----------------------------------------------
//MY FEATURE MODE
//-----------------------------------------------

void fmHeader(int xres, int yres)
{
    Rect r;
    unsigned int c = 0x00ffff44;
    r.bot = yres - 50;
    r.left = (xres / xres) + 50;
    r.center = 0;
    ggprint8b(&r, 16, c, "Jose's Feature Mode:");
    ggprint8b(&r, 16, c, "   ");
    ggprint8b(&r, 16, c, "PLAYER 1 HITBOX:");
    ggprint8b(&r, 16, c, "SHIFT + 7 - Increase Player 1 Width");
    ggprint8b(&r, 16, c, "SHIFT + 8 - Increase Player 1 Height");
    ggprint8b(&r, 16, c, "SHIFT + F7 - Decrease Player 1 Width");
    ggprint8b(&r, 16, c, "SHIFT + F8 - Decrease Player 1 Height");
    ggprint8b(&r, 16, c, "   ");
    ggprint8b(&r, 16, c, "PLAYER 2 HITBOX:");
    ggprint8b(&r, 16, c, "SHIFT + 9 - Increase Player 2 Width");
    ggprint8b(&r, 16, c, "SHIFT + 0 - Increase Player 2 Height");
    ggprint8b(&r, 16, c, "SHIFT + F9 - Decrease Player 2 Width");
    ggprint8b(&r, 16, c, "SHIFT + F10 - Decrease Player 2 Height");
    ggprint8b(&r, 16, c, "   ");
    ggprint8b(&r, 16, c, "PLAYER 1 PUNCH HITBOX:");
    ggprint8b(&r, 16, c, "CTRL + 7 - Increase Player 1 Punch Length");
    ggprint8b(&r, 16, c, "CTRL + 8 - Increase Player 1 Punch Width");
    ggprint8b(&r, 16, c, "CTRL + F7 - Decrease Player 1 Punch Length");
    ggprint8b(&r, 16, c, "CTRL + F8 - Decrease Player 1 Punch Width");
    ggprint8b(&r, 16, c, "   ");
    ggprint8b(&r, 16, c, "PLAYER 2 PUNCH HITBOX:");
    ggprint8b(&r, 16, c, "CTRL + 9 - Increase Player 2 Punch Length");
    ggprint8b(&r, 16, c, "CTRL + 0 - Increase Player 2 Punch Width");
    ggprint8b(&r, 16, c, "CTRL + F9 - Decrease Player 2 Punch Length");
    ggprint8b(&r, 16, c, "CTRL + F10 - Decrease Player 2 Punch Width");
    ggprint8b(&r, 16, c, "   ");
    ggprint8b(&r, 16, c, "PLAYER SPEED:");
    ggprint8b(&r, 16, c, "'+' - Increase Player Speed");
    ggprint8b(&r, 16, c, "'-' - Decrease Player Speed");
}

void fmBorder(int xres, int yres) 
{
    int t = 10;
    // Providing a seed value
	srand((unsigned) time(NULL));
	// Get a random number
    if (glu.i % 20 == 0) {            
	    glu.c1 = 1 + (rand() % 255);
        glu.c2 = 1 + (rand() % 255);
        glu.c3 = 1 + (rand() % 255);
    }
    glColor3ub(glu.c1, glu.c2, glu.c3);
    glBegin(GL_TRIANGLE_STRIP);
        glVertex2f(0,           0);
        glVertex2f(t,           t);
        glVertex2f(0,           yres);
        glVertex2f(t,           yres - t);
        glVertex2f(xres,        yres);
        glVertex2f(xres - t,    yres - t);
        glVertex2f(xres,        0);
        glVertex2f(xres - t,    t);
        glVertex2f(0,           0);
        glVertex2f(t,           t);
    glEnd();
}

/*extern*/ void display_controls(int wf, int yres) 
{
    Rect r;
    unsigned int c = 0x00ffff44;
    r.bot = yres - 20;
    r.left = 10;
    r.center = 0;
    ggprint8b(&r, 16, c, "Player 1: ");
    ggprint8b(&r, 16, c, "a - move left");
    ggprint8b(&r, 16, c, "d - move rightr");
    ggprint8b(&r, 16, c, "Player 2: ");
    ggprint8b(&r, 16, c, "left arrow - move left");
    ggprint8b(&r, 16, c, "right arrow - move right");
    ggprint8b(&r, 16, c, "frame: %i", wf);
}

//-----------------------------------------------
//BACKGROUND TEXTURE
//-----------------------------------------------

void displayBackground(float x0, float x1,
                       float y0, float y1,
                       int xres, int yres,
                       int width, int height, 
                       GLuint texture, GLvoid* data) 
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data);

    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
        glTexCoord2f(x0, y1);   glVertex2i(0,           0);
        glTexCoord2f(x0, y0);   glVertex2i(0,           yres);
        glTexCoord2f(x1, y0);   glVertex2i(xres*1.2,    yres);
        glTexCoord2f(x1, y1);   glVertex2i(xres*1.2,    0);
    glEnd();
}

//-----------------------------------------------
//PLAYER TEXTURE
//-----------------------------------------------

void displayCharacter(float x0, float x1,
                      float y0, float y1,
                      double px, double px2,
                      double py, double py2,
                      int xres, int yres,
                      int width, int height, 
                      GLuint texture, GLvoid* data) 
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);

    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255, 255, 255, 255);
    glu.i++;
    glBegin(GL_QUADS);
        glTexCoord2f(x0, y1);   glVertex2i(px - 90,       
                                           py2 - 10);

        glTexCoord2f(x0, y0);   glVertex2i(px - 90,       
                                           (yres + py) - 200);

        glTexCoord2f(x1, y0);   glVertex2i(((xres/3) + px2) - 90, 
                                           (yres + py) - 200);

        glTexCoord2f(x1, y1);   glVertex2i(((xres/3) + px2) - 90,  
                                           py2 - 10);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
}

void displayPlayerLeft(double p_x,  double o_x, int* time, 
                       float* wx0,  float* wx1, 
                       float* wx2,  float* wx3,
                       float* wy0,  float* wy1,
                       float* wy2,  float* wy3,
                       float* wkx0, float* wkx1,
                       float* wky0, float* wky1) 
{
    if (p_x > o_x) {
        *time += 1;
        if (*time % 6 == 0) {
            if (*wx1 < -0.5f) {
                *wx0 = 0.0f;
                *wx1 = 0.125f;
            }
            *wx0 -= 0.132f;
            *wx1 -= 0.132f;
            *wkx0 = -(*wx0);
            *wkx1 = -(*wx1);
            *wky0 = (*wy0);
            *wky1 = (*wy1);
        }
    }

    else if (p_x < o_x) { 
        *time += 1;
        if (*time % 6 == 0) {
            if (*wx3 < 0.5f) {
                *wx2 = 0.875f;
                *wx3 = 1.0f;
            }
            *wx2 -= 0.1262f;
            *wx3 -= 0.1262f;
            *wkx0 = *wx2;
            *wkx1 = *wx3;
            *wky0 = *wy2;
            *wky1 = *wy3;
        }
    }
}

void displayPlayerRight(double p_x,  double o_x, int* time, 
                        float* wx0,  float* wx1, 
                        float* wx2,  float* wx3,
                        float* wy0,  float* wy1,
                        float* wy2,  float* wy3,
                        float* wkx0, float* wkx1,
                        float* wky0, float* wky1) 
{
    if (p_x > o_x) {
        *time += 1;
        if (*time % 6 == 0) {
            if (*wx3 > -0.5f) {
                *wx2 = -0.75f;
                *wx3 = -0.875f;
            }
            *wx2 += 0.1234f;
            *wx3 += 0.1234f;
            *wkx0 = *wx2;
            *wkx1 = *wx3;
            *wky0 = *wy2;
            *wky1 = *wy3;
        }
    }

    else if (p_x < o_x) { 
        *time += 1;
        if (*time % 6 == 0) {
            if (*wx1 > 0.625f) {
                *wx0 = 0.0f;
                *wx1 = 0.125f;
            }
            *wx0 += 0.132f;
            *wx1 += 0.132f;
            *wkx0 = *wx0;
            *wkx1 = *wx1;
            *wky0 = *wy0;
            *wky1 = *wy1;
        }
    }

}

void displayPlayerPunch(double p_x,  double o_x, int* frame,
                        float p_w,   float o_w,  int p_cd,
                        float* px0,  float* px1,
                        float* px2,  float* px3,
                        float* py0,  float* py1,
                        float* py2,  float* py3,
                        float* wkx0, float* wkx1,
                        float* wky0, float* wky1) 
{

    //IS PLAYER TO THE LEFT OF OPPONENT?
    if (p_x + p_w < o_x - o_w) {
        if (p_cd % 35 == 0) {
            *wkx0 = *px0;
            *wkx1 = *px1;
            *wky0 = *py0;
            *wky1 = *py1;
        }

        //ARE WE ON THE LAST FRAME
        //IF YES, GO TO SECOND FRAME
        if (*px1 + 0.2f > 0.525f) {
            *px0 -= 0.150f;
            *px1 -= 0.250f;
            *frame = 1;
        }
        //IF WE ARE ON THE SECOND FRAME COMING FROM LAST FRAME
        //GO TO FIRST FRAME
        else if (*frame == 1) {
            *px0 -= 0.150f;
            *px1 -= 0.150f;
            *frame = 0;
        }
        //GOING FROM FIRST FRAME TO SECOND FRAME
        else if (*px0 != 0.150f) {
            *px0 += 0.150f;
            *px1 += 0.150f;
        }
        //GOING FROM SECOND FRAME TO THIRD FRAME
        else if (*px0 != 0.300f) {
            *px0 += 0.150f;
            *px1 += 0.250f;
        }
    }

    //IS PLAYER TO THE RIGHT OF OPPONENT?
    if (p_x - p_w > o_x + o_w) {
        if (p_cd % 35 == 0) {
            *wkx0 = -(*px2);
            *wkx1 = -(*px3);
            *wky0 = *py2;
            *wky1 = *py3;
        }

        //ARE WE ON THE LAST FRAME
        //IF YES, GO TO SECOND FRAME
        if (*px2 - 0.2f < 0.475f) {
            *px3 += 0.150f;
            *px2 += 0.250f;
            *frame = 1;
        }
        //IF WE ARE ON THE SECOND FRAME COMING FROM LAST FRAME
        //GO TO FIRST FRAME
        else if (*frame == 1) {
            *px3 += 0.150f;
            *px2 += 0.150f;
            *frame = 0;
        }
        //GOING FROM FIRST FRAME TO SECOND FRAME
        else if (*px3 != 0.850f) {
            *px3 -= 0.150f;
            *px2 -= 0.150f;
        }
        //GOING FROM SECOND FRAME TO THIRD FRAME
        else if (*px3 != 0.700f) {
            *px3 -= 0.150f;
            *px2 -= 0.250f;
        }
    }

}

//-----------------------------------------------
//HITBOXES FOR PLAYERS & THEIR PUNCHES
//-----------------------------------------------

/*extern*/ void player_hitbox(int w, int h, float x, float y) 
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glBegin(GL_QUADS);
        glColor3ub(0, 0, 0);
        glVertex2f(-w,  -h);
        glVertex2f(-w,   h);
        glVertex2f( w,   h);
        glVertex2f( w,  -h);
    glEnd();
    glPopMatrix();
}

/*extern*/ void punch_hitbox(int w1, int w2, int h, float x, float y) 
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glBegin(GL_QUADS);
        glColor3ub(0, 150, 0);
        glVertex2f(-w2,  -h);
        glVertex2f(-w2,   h);
        glVertex2f( w1,   h);
        glVertex2f( w1,  -h);
    glEnd();
    glPopMatrix();
}

//-----------------------------------------------
//PHYSICS FOR PLAYER 1 AND PLAYER 2:
//-----------------------------------------------

void movePlayerLeft(double* pos_x, double* pos2_x, 
                    double* pos_y, double* pos2_y,
                    double vel_x,  float p2_w,
                    float width,   float* lft,     float* rgt)
{
    //Lets player on left side move on right side of other player
    if (*pos_x > *pos2_x + (2 * p2_w) || 
        *pos_y > *pos2_y || *pos_y < *pos2_y) {
        if (*pos_x > width) {
            *pos_x -= vel_x;
        } 
    }
    //Lets player on left side move on left side of other player
    else if (*pos_x < *pos2_x - (2 * p2_w) || *pos_y > *pos2_y) {
        if (*pos_x > width) {
            *pos_x -= vel_x;
        }
        else if (*pos_x <= 20.0f && *lft >= 0) {
            *lft -= 0.001;
            *rgt -= 0.001;
        }
    }
    else if(*pos_x == *pos2_x - (2* p2_w)) {
        *pos_x -= vel_x;
    }
}

void movePlayerRight(double* pos_x, double* pos2_x, 
                     double* pos_y, double* pos2_y,
                     double vel_x,  float p2_w,
                     float width,   float* lft, 
                     float* rgt,    int xres)
{
    if (*pos_x < *pos2_x - (2 * p2_w) || 
        *pos_y > *pos2_y || *pos_y < *pos2_y) {
        if (*pos_x < (float)xres - width) {
            *pos_x += vel_x;
        }
    }
    else if (*pos_x > *pos2_x + (2 * p2_w) || *pos_y > *pos2_y) { 
        if (*pos_x < (float)xres - width) {
            *pos_x += vel_x;
        }
        else if (*pos_x >= 1420.0f && *lft <= 0.166) {
            *lft += 0.001;
            *rgt += 0.001;
        }
    }
    else if(*pos_x == *pos2_x + (2* p2_w)) {
        *pos_x += vel_x;
    }
}

/*extern*/ void jumpPlayer(double* vel_down, double* vel_up)
{
    *vel_down = 200.0f;
    *vel_up = 200.0f;
}

/*extern*/ void movePlayerUp(double* vel_up, double* pos_y)
{
    *vel_up -= 5.0f;
    *pos_y += 12.5f;
}

/*extern*/ void movePlayerDown(double* vel_down, double* pos_y)
{
    *vel_down -= 5.0f;
    *pos_y -= 12.5f;
}

/*extern*/ void punchAbilityPlayer1(int* p1_punch, int* sPunch, int jeflag,
                                double* p1_pos_x, double* p1_y, float p1_pw2,
                                double* p2_pos_x, double* p2_pos_y, float p2_w, 
                                double* p2_vel_1, int* p2_dead,
                                int p2_block, int* p2_health, int weapon)
{
    if((*sPunch) == 1){
        *p1_punch = 0;
    }
    else{
        *p1_punch = 1;
    }

    // Punch Detection
    if (*p1_pos_x + p1_pw2 >= *p2_pos_x + (-p2_w) && 
        *p1_pos_x < *p2_pos_x &&
        *p1_y - 40.0f < *p2_pos_y &&
        *p2_pos_y < *p1_y + 40.0f) {
        if (jeflag == 1 && *p2_dead == 0) {
            *p2_health -= 25;
        }
        else if (*p2_dead == 0) {
            if (!p2_block) {
                if((*sPunch) == 1 || weapon == 5){
                     *p2_health -= 20;
                }   else {
                     *p2_health -= 10;
                }
            }
            else {
                if((*sPunch) == 1 || weapon == 5){
                     *p2_health -= 10;
                }   else {
                     *p2_health -= 5;
                }
            }
            *p2_pos_y += 50.0f;
            *p2_vel_1 += 20.0f;
            *p2_pos_x += 35.0f;
        }
        if (*p2_health <= 0) {
            *p2_health = 0;
            *p2_dead = 1;
        }
    }
    // Punch Detection (Flipped)
    else if (*p1_pos_x - p1_pw2 <= *p2_pos_x + p2_w && 
             *p1_pos_x > *p2_pos_x &&
             *p1_y - 40.0f < *p2_pos_y &&
             *p2_pos_y < *p1_y + 40.0f) {
        if (jeflag == 1 && *p2_dead == 0) {
            *p2_health -= 25;
        }
        else if (*p2_dead == 0) {
            if (!p2_block) {
                if((*sPunch) == 1 || weapon == 5){
                     *p2_health -= 20;
                }   else {
                     *p2_health -= 10;
                }
            }
            else {
                if((*sPunch) == 1 || weapon == 5){
                     *p2_health -= 10;
                }   else {
                     *p2_health -= 5;
                }
            }
            *p2_pos_y += 50.0f;
            *p2_vel_1 += 20.0f;
            *p2_pos_x -= 35.0f;
        }
        if (*p2_health <= 0) {
            *p2_health = 0;
            *p2_dead = 1;
        }
    }
}

/*extern*/ void punchAbilityPlayer2(int* p2_punch, int* sPunch, int jeflag,
                                double* p2_pos_x, double* p2_y, float p2_pw2,
                                double* p1_pos_x, double* p1_pos_y, float p1_w, 
                                double* p1_vel_1, int* p1_dead,
                                int p1_block, int* p1_health, int weapon)
{
    if((*sPunch) == 1){
        *p2_punch = 0;
    }
    else{
        *p2_punch = 1;
    }

    // Punch Detection
    if (*p2_pos_x - p2_pw2 <= *p1_pos_x + (p1_w) && 
        *p2_pos_x > *p1_pos_x &&
        *p2_y - 40.0f < *p1_pos_y &&
        *p1_pos_y < *p2_y + 40.0f) {
        if (jeflag == 1 && *p1_dead == 0) {
            *p1_health -= 25;
        }
        else if (*p1_dead == 0) {
            if (!p1_block) {
                if((*sPunch) == 1 || weapon == 5){
                         *p1_health -= 20;
                } else {
                         *p1_health -= 10;
                    }
                }
                else {
                    if((*sPunch) == 1 || weapon == 5){
                         *p1_health -= 10;
                    }   else {
                         *p1_health -= 5;
                    }
                }
            *p1_pos_y += 50.0f;
            *p1_vel_1 += 20.0f;
            *p1_pos_x -= 35.0f;
        }
        if (*p1_health <= 0) {
            *p1_health = 0;
            *p1_dead = 1;
        }
    }
    // Punch Detection (Flipped)
    else if (*p2_pos_x + p2_pw2 >= *p1_pos_x - p1_w && 
             *p2_pos_x < *p1_pos_x &&
             *p2_y - 40.0f < *p1_pos_y &&
             *p1_pos_y < *p2_y + 40.0f) {
        if (jeflag == 1 && *p1_dead == 0) {
            *p1_health -= 25;
        }
        else if (*p1_dead == 0) {
            if (!p1_block) {
                if((*sPunch) == 1 || weapon == 5){
                         *p1_health -= 20;
                } else {
                         *p1_health -= 10;
                    }
                }
                else {
                    if((*sPunch) == 1 || weapon == 5){
                         *p1_health -= 10;
                    }   else {
                         *p1_health -= 5;
                    }
                }
            *p1_pos_y += 50.0f;
            *p1_vel_1 += 20.0f;
            *p1_pos_x += 35.0f;
        }
        if (*p1_health <= 0) {
            *p1_health = 0;
            *p1_dead = 1;
        }
    }
}

/*extern*/ void punchCooldownPlayer(int* cooldown, int* punch)
{
    if (*cooldown == 0) {
        *punch = 0;
        *cooldown = 250.0f;
    }
    else {
        *cooldown -= 10.0f;
    }
}

//-----------------------------------------------
//UTILITY:
//-----------------------------------------------

void fillArrays(float ryu_wx[], float ryu_wy[],
                float ryu_punchx[], float ryu_punchy[],
                float ryu_walkx[], float ryu_walky[]) 
{
    //PLAYER 1 TEXTURE COORDINATES: MOVING RIGHT:
    ryu_wx[0] = 0.0f;
    ryu_wx[1] = 0.125f;
    ryu_wy[0] = 0.0f;
    ryu_wy[1] = 0.33f;

    //PLAYER 1 TEXTURE COORDINATES: MOVING LEFT
    ryu_wx[2] = 0.875f;
    ryu_wx[3] = 1.0f;
    ryu_wy[2] = 0.33f;
    ryu_wy[3] = 0.66f;

    //PUNCH FOR PLAYER 1: NORMAL
    ryu_punchx[0] = 0.0f;
    ryu_punchx[1] = 0.125f;
    ryu_punchy[0] = 0.66f;
    ryu_punchy[1] = 0.99f;

    //PUNCH FOR PLAYER 1: FLIPPED
    ryu_punchx[2] = 0.875f;
    ryu_punchx[3] = 1.0f;
    ryu_punchy[2] = 0.66f;
    ryu_punchy[3] = 0.99f;

    //SETTING CHARACTER TEXTURES FOR PLAYER 1:
    ryu_walkx[0] = ryu_wx[0];
    ryu_walkx[1] = ryu_wx[1];
    ryu_walky[0] = ryu_wy[0];
    ryu_walky[1] = ryu_wy[1];

    //PLAYER 2 TEXTURE COORDINATES: MOVING RIGHT
    ryu_wx[4] = 0.0f;
    ryu_wx[5] = 0.125f;
    ryu_wy[4] = 0.0f;
    ryu_wy[5] = 0.33f;

    //PLAYER 2 TEXTURE COORDINATES: MOVING LEFT
    ryu_wx[6] = 0.875f;
    ryu_wx[7] = 1.0f;
    ryu_wy[6] = 0.33f;
    ryu_wy[7] = 0.66f;

    //PUNCH FOR PLAYER 2: NORMAL
    ryu_punchx[4] = 0.0f;
    ryu_punchx[5] = 0.123f;
    ryu_punchy[4] = 0.66f;
    ryu_punchy[5] = 0.99f;
 
    //PUNCH FOR PLAYER 2: FLIPPED
    ryu_punchx[6] = 0.875f;
    ryu_punchx[7] = 1.0f;
    ryu_punchy[6] = 0.66f;
    ryu_punchy[7] = 0.99f;
 
    //SETTING CHARACTER TEXTURES FOR PLAYER 2:
    ryu_walkx[4] = ryu_wx[4];
    ryu_walkx[5] = ryu_wx[5];
    ryu_walky[4] = ryu_wy[4];
    ryu_walky[5] = ryu_wy[5];
}
