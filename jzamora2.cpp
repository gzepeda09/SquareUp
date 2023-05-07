//program: jzamora2.cpp || Source file for project
//author: Jose Zamora //
//date: 02/21/2022

#include "fonts.h"
#include "jzamora2.h"
#include <iostream>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <cstring>

class Global {
    public:
        int i = 0;
} glu; 

extern void test_text (int gres, int yres)
{
    Rect r;
    unsigned int c = 0x00ffff44;
        r.bot = yres - 50;
        r.left = 100;
        r.center = 1;
        ggprint8b(&r, 16, c, "Jose's Feature Mode");
    ggprint8b(&r, 16, c, "As a text");
}

extern void fmBorder(int xres, int yres) 
{
    int t = 10;
    glColor3f(0.0, 1.0, 0.0);
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
        glTexCoord2f(x0, y1);   glVertex2i(px - 100,       
                                           py2 - 10);
        //glTexCoord2f(x0, y0);   glVertex2i(px,       (yres*4)+py);
        glTexCoord2f(x0, y0);   glVertex2i(px - 100,       
                                           (yres + py) - 10);
        //glTexCoord2f(x1, y0);   glVertex2i((xres/2.5) + px2, (yres*4)+py);
        glTexCoord2f(x1, y0);   glVertex2i(((xres/3) + px2) - 75, 
                                           (yres + py) - 10);
        //glTexCoord2f(x1, y1);   glVertex2i((xres/2.5) + px2,  py2);
        glTexCoord2f(x1, y1);   glVertex2i(((xres/3) + px2) - 75,  
                                           py2 - 10);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_ALPHA_TEST);
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

/*extern*/ void punchAbilityPlayer1(int* p1_punch, int jeflag,
                                double* p1_pos_x, double* p1_y, float p1_pw2,
                                double* p2_pos_x, double* p2_pos_y, float p2_w, 
                                double* p2_vel_1, int* p2_dead,
                                int p2_block, int* p2_health)
{
    *p1_punch = 1;
    // Punch Detection
    if (*p1_pos_x + p1_pw2 >= *p2_pos_x + (-p2_w) && 
        *p1_pos_x < *p2_pos_x &&
        *p1_y - 40.0f < *p2_pos_y) {
        if (jeflag == 1 && p2_dead == 0) {
            *p2_health -= 50;
        }
        else if (*p2_dead == 0) {
            if (!p2_block) {
                *p2_health -= 10;
            }
            else {
                *p2_health -= 5;
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
             *p1_y - 40.0f < *p2_pos_y) {
        if (jeflag == 1 && *p2_dead == 0) {
            *p2_health -= 50;
        }
        else if (*p2_dead == 0) {
            if (!p2_block) {
                *p2_health -= 10;
            }
            else {
                *p2_health -= 5;
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

/*extern*/ void punchAbilityPlayer2(int* p2_punch, int jeflag,
                                double* p2_pos_x, double* p2_y, float p2_pw2,
                                double* p1_pos_x, double* p1_pos_y, float p1_w, 
                                double* p1_vel_1, int* p1_dead,
                                int p1_block, int* p1_health)
{
    *p2_punch = 1;
    // Punch Detection
    if (*p2_pos_x - p2_pw2 <= *p1_pos_x + (p1_w) && 
        *p2_pos_x > *p1_pos_x &&
        *p2_y - 40.0f < *p1_pos_y) {
        if (jeflag == 1 && p1_dead == 0) {
            *p1_health -= 50;
        }
        else if (*p1_dead == 0) {
            if (!p1_block) {
                *p1_health -= 10;
            }
            else {
                *p1_health -= 5;
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
             *p2_y - 40.0f < *p1_pos_y) {
        if (jeflag == 1 && *p1_dead == 0) {
            *p1_health -= 50;
        }
        else if (*p1_dead == 0) {
            if (!p1_block) {
                *p1_health -= 10;
            }
            else {
                *p1_health -= 5;
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

/*extern*/ void punchCooldownPlayer(float* cooldown, int* punch)
{
    if (*cooldown == 0) {
        *punch = 0;
        *cooldown = 250.0f;
    }
    else {
        *cooldown -= 10.0f;
    }
}

