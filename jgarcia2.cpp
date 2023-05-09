//Jesse Garcia
//walk.cpp

//my libraries
//#include <X11/Xlib.h>
//#include <X11/keysym.h>
//#include <GL/glx.h>
//#include "fonts.h"

//asteroids libraries
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */


class Global {
public:
     GLuint menu; //my menus texture for both players
     GLuint p1Red;
     GLuint p2Red;
     GLuint mysBox;
     GLuint speedUp;
     GLuint slFoe;
     GLuint health;
     GLuint damage;
}gl;

class Image {
public:
        int width, height;
        unsigned char *data;
        ~Image() { delete [] data; }
        Image(const char *fname) {
                if (fname[0] == '\0')
                        return;
                int ppmFlag = 0;
                char name[40];
                strcpy(name, fname);
                int slen = strlen(name);
                char ppmname[80];
                if (strncmp(name+(slen-4), ".ppm", 4) == 0)
                        ppmFlag = 1;
                if (ppmFlag) {
                        strcpy(ppmname, name);
                } else {
                        name[slen-4] = '\0';
                        sprintf(ppmname,"%s.ppm", name);
                        char ts[100];
                        sprintf(ts, "convert %s %s", fname, ppmname);
                        system(ts);
                }
                FILE *fpi = fopen(ppmname, "r");
                if (fpi) {
                        char line[200];
                        fgets(line, 200, fpi);
                        fgets(line, 200, fpi);
                        //skip comments and blank lines
                        while (line[0] == '#' || strlen(line) < 2)
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
                if (!ppmFlag)
                        unlink(ppmname);
        }
} player1Menu("menuPics/playerOneWinsMenu.png"), player2Menu("menuPics/playerTwoWinsMenu.png"),
player2MenuRed("menuPics/playerTwoWinsMenuRed.png"), player1MenuRed("menuPics/playerOneWInsMenuRed.png"),
mysBoxPic("images/mysteryBox.jpg"), playerSpeed("menuPics/doubleSpeed.png"),
playerSlowF("menuPics/slowFoe.png"), playerHealth("menuPics/healthBoost.png"),
playerDamage("menuPics/doubleDamage.png");


void greenBoxes(int ywin, int xwin)
{
        glPushMatrix();
        glColor3f(0.0, 5.0, 0.0);
        glBegin(GL_QUADS);
            glVertex2i(0, 0);
            glVertex2i(0, ywin);
            glVertex2i(20, ywin);
            glVertex2i(20, 0);
            
            glVertex2i(xwin-20, 0);
            glVertex2i(xwin-20, ywin);
            glVertex2i(xwin, ywin);
            glVertex2i(xwin, 0);
            
            glVertex2i(0, 0);
            glVertex2i(0, 20);
            glVertex2i(xwin, 20);
            glVertex2i(xwin, 0);
            
            glVertex2i(0, ywin-20);
            glVertex2i(0, ywin);
            glVertex2i(xwin, ywin);
            glVertex2i(xwin, ywin-20);
        glEnd();
        glPopMatrix();
}

void playerBlocking(float w, float h, float x, float y, int flipped)
{
    if (flipped == 1) { //fliped = 1 so player is facing right
        //before elbow
        glPushMatrix();
        glTranslatef(x, y+50.0, 0.0);
        glBegin(GL_QUADS);
        //glColor3ub(0, 0, 0);
            glVertex2i(-w, -w);
            glVertex2i(-w, 0);
            glVertex2i(w*2+2, 0); //the w*2+2 dictates how far the 
            glVertex2i(w*2+2, -w); //elbow reach is
        glEnd();
        glPopMatrix();
        
        //after elbow
        glPushMatrix();
        glTranslatef(x+w*2+2, y+50.0, 0.0);
        glBegin(GL_QUADS);
        //glColor3ub(0, 0, 0);
            glVertex2i(-w, 0);
            glVertex2i(-w, h/2);
            glVertex2i(0, h/2);
            glVertex2i(0, 0);
        glEnd();
        glPopMatrix();
    
    } else { //player is facing left
        //before elbow
        glPushMatrix();
        glTranslatef(x, y+50.0, 0.0);
        glBegin(GL_QUADS);
        //glColor3ub(0, 0, 0);
            glVertex2i(-w*2-2, -w);
            glVertex2i(-w*2-2, 0);
            glVertex2i(w, 0);
            glVertex2i(w, -w);
        glEnd();
        glPopMatrix();

        //after elbow
        glPushMatrix();
        glTranslatef(x-w*2-2, y+50.0, 0.0);
        glBegin(GL_QUADS);
        //glColor3ub(0, 0, 0);
            glVertex2i(w, 0);
            glVertex2i(w, h/2);
            glVertex2i(0, h/2);
            glVertex2i(0, 0);
        glEnd();
        glPopMatrix();
    }
}

void holdingWeapon(float w, float h, float x, float y, int flipped, int weapon) //fix the colors when done
{
    if (weapon == 1) {
        if (flipped == 1) { //fliped = 1 so player is facing right holding weapon
            //arm
            glPushMatrix();
            glTranslatef(x, y+50.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(0, 0, 0);
                glVertex2i(-w, -w);
                glVertex2i(-w, 0);
                glVertex2i(h, 0);
                glVertex2i(h, -w);
            glEnd();
            glPopMatrix();

            //handle
            glPushMatrix();
            glTranslatef(x+h, y+50.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(1, 200, 1);
                glVertex2i(-10, 0);
                glVertex2i(-10, 10);
                glVertex2i(0, 10);
                glVertex2i(0, 0);
            glEnd();
            glPopMatrix();

            //handle protector
            glPushMatrix();
            glTranslatef(x+h, y+60.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(200, 1, 1);
                glVertex2i(-20, 0);
                glVertex2i(-20, 5);
                glVertex2i(10, 5);
                glVertex2i(10, 0);
            glEnd();
            glPopMatrix();

            //blade
            glPushMatrix();
            glTranslatef(x+h, y+65.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(1, 200, 1);
                glVertex2i(-10, 0);
                glVertex2i(-10, h);
                glVertex2i(0, h);
                glVertex2i(0, 0);
            glEnd();
            glPopMatrix();
        } else { //player is facing left holding weapon
            //arm           
            glPushMatrix();
            glTranslatef(x, y+50.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(0, 0, 0);
                glVertex2i(w, -w);
                glVertex2i(w, 0);
                glVertex2i(-h, 0);
                glVertex2i(-h, -w);
            glEnd();
            glPopMatrix();

            //handle
            glPushMatrix();
            glTranslatef(x-h, y+50.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(1, 200, 1);
                glVertex2i(10, 0);
                glVertex2i(10, 10);
                glVertex2i(0, 10);
                glVertex2i(0, 0);
            glEnd();
            glPopMatrix();

            //handle protector
            glPushMatrix();
            glTranslatef(x-h, y+60.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(200, 1, 1);
                glVertex2i(20, 0);
                glVertex2i(20, 5);
                glVertex2i(-10, 5);
                glVertex2i(-10, 0);
            glEnd();
            glPopMatrix();

            //blade
            glPushMatrix();
            glTranslatef(x-h, y+65.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(1, 200, 1);
                glVertex2i(10, 0);
                glVertex2i(10, h);
                glVertex2i(0, h);
                glVertex2i(0, 0);
            glEnd();
            glPopMatrix();
        }
    }
}

void useWeapon(float w, float h, float x, float y, int flipped, int weapon) 
{
    //code here
    if (weapon == 1) {
        if (flipped == 1) { //fliped = 1 so player is facing right using weapon
            //arm
            glPushMatrix();
            glTranslatef(x, y+50.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(0, 0, 0);
                glVertex2i(-w, -w);
                glVertex2i(-w, 0);
                glVertex2i(h, 0);
                glVertex2i(h, -w);
            glEnd();
            glPopMatrix();

            //handle
            glPushMatrix();
            glTranslatef(x+h, y+40.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(1, 200, 1);
                glVertex2i(0, 0);
                glVertex2i(0, 10);
                glVertex2i(10, 10);
                glVertex2i(10, 0);
            glEnd();
            glPopMatrix();

            //handle protector
            glPushMatrix();
            glTranslatef(x+h+10, y+40.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(200, 1, 1);
                glVertex2i(0, -10);
                glVertex2i(0, 20);
                glVertex2i(5, 20);
                glVertex2i(5, -10);
            glEnd();
            glPopMatrix();

            //blade
            glPushMatrix();
            glTranslatef(x+h+15, y+40.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(1, 200, 1);
                glVertex2i(0, 0);
                glVertex2i(0, 10);
                glVertex2i(h, 10);
                glVertex2i(h, 0);
            glEnd();
            glPopMatrix();
        } else { //player facing left using weapon
            //arm
            glPushMatrix();
            glTranslatef(x, y+50.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(0, 0, 0);
                glVertex2i(w, -w);
                glVertex2i(w, 0);
                glVertex2i(-h, 0);
                glVertex2i(-h, -w);
            glEnd();
            glPopMatrix();

            //handle
            glPushMatrix();
            glTranslatef(x-h, y+50.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(1, 200, 1);
                glVertex2i(0, 0);
                glVertex2i(0, -10);
                glVertex2i(-10, -10);
                glVertex2i(-10, 0);
            glEnd();
            glPopMatrix();

            //handle protector
            glPushMatrix();
            glTranslatef(x-h-10, y+40.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(200, 1, 1);
                glVertex2i(0, -10);
                glVertex2i(0, 20);
                glVertex2i(-5, 20);
                glVertex2i(-5, -10);
            glEnd();
            glPopMatrix();

            //blade
            glPushMatrix();
            glTranslatef(x-h-15, y+40.0, 0.0);
            glBegin(GL_QUADS);
            glColor3ub(1, 200, 1);
                glVertex2i(0, 0);
                glVertex2i(0, 10);
                glVertex2i(-h, 10);
                glVertex2i(-h, 0);
            glEnd();
            glPopMatrix();
        }
    }
}

int randLimit = 0;
int randX = 0;
int mysteryBox(int x, int mbn)
{
    //spawn a mystery box randomly with a fixed y. 
    //x is the only number that will be random

    x -= 50;
    /* initialize random seed: */
    srand (time(NULL));
    if (randLimit == mbn) {
        randX = (rand() % x);
        randLimit += 1;
        if (randLimit >= 3) { //was 2 adding one for health at 100
            randLimit = 0;
        }
    }
    
    //for mystery box
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &gl.mysBox);

    int w = mysBoxPic.width;
    int h = mysBoxPic.height;

    glBindTexture(GL_TEXTURE_2D, gl.mysBox);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
                        GL_RGB, GL_UNSIGNED_BYTE, mysBoxPic.data);
    

    glBindTexture(GL_TEXTURE_2D, gl.mysBox);
    glColor3f(1.0, 1.0, 1.0);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2i(randX, 100);
        glTexCoord2f(0.0f, 0.0f); glVertex2i(randX, 150);
        glTexCoord2f(1.0f, 0.0f); glVertex2i(randX+50, 150);
        glTexCoord2f(1.0f, 1.0f); glVertex2i(randX+50, 100);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    return randX;
}

void showPowerUp(int weapon, int ywin, int xwin)
{
    if (weapon == 2) {  //speed
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &gl.speedUp);
        
        int rbW = playerSpeed.width;
        int rbH = playerSpeed.height;

        glBindTexture(GL_TEXTURE_2D, gl.speedUp);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, rbW, rbH, 0,
                            GL_RGB, GL_UNSIGNED_BYTE, playerSpeed.data);

        glBindTexture(GL_TEXTURE_2D, gl.speedUp);
        glColor3f(1.0, 1.0, 1.0);

        int widthR = 300;
        int heightR = 100;
                                
        
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2i(xwin/2, ywin/1.2);
            glTexCoord2f(0.0f, 0.0f); glVertex2i(xwin/2, ywin/1.2+heightR);
            glTexCoord2f(1.0f, 0.0f); glVertex2i(xwin/2+widthR, ywin/1.2+heightR);
            glTexCoord2f(1.0f, 1.0f); glVertex2i(xwin/2+widthR, ywin/1.2);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
    } else if (weapon == 3) {   //slow foe
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &gl.slFoe);
        
        int rbW = playerSlowF.width;
        int rbH = playerSlowF.height;

        glBindTexture(GL_TEXTURE_2D, gl.slFoe);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, rbW, rbH, 0,
                            GL_RGB, GL_UNSIGNED_BYTE, playerSlowF.data);

        glBindTexture(GL_TEXTURE_2D, gl.slFoe);
        glColor3f(1.0, 1.0, 1.0);

        int widthR = 300;
        int heightR = 100;
                                
        
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2i(xwin/2, ywin/1.2);
            glTexCoord2f(0.0f, 0.0f); glVertex2i(xwin/2, ywin/1.2+heightR);
            glTexCoord2f(1.0f, 0.0f); glVertex2i(xwin/2+widthR, ywin/1.2+heightR);
            glTexCoord2f(1.0f, 1.0f); glVertex2i(xwin/2+widthR, ywin/1.2);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
    } else if (weapon == 4) {   //health
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &gl.health);
        
        int rbW = playerHealth.width;
        int rbH = playerHealth.height;

        glBindTexture(GL_TEXTURE_2D, gl.health);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, rbW, rbH, 0,
                            GL_RGB, GL_UNSIGNED_BYTE, playerHealth.data);

        glBindTexture(GL_TEXTURE_2D, gl.health);
        glColor3f(1.0, 1.0, 1.0);

        int widthR = 300;
        int heightR = 100;
                                
        
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2i(xwin/2, ywin/1.2);
            glTexCoord2f(0.0f, 0.0f); glVertex2i(xwin/2, ywin/1.2+heightR);
            glTexCoord2f(1.0f, 0.0f); glVertex2i(xwin/2+widthR, ywin/1.2+heightR);
            glTexCoord2f(1.0f, 1.0f); glVertex2i(xwin/2+widthR, ywin/1.2);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
    } else if (weapon == 5)if (weapon == 5){    //damage
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &gl.damage);
        
        int rbW = playerDamage.width;
        int rbH = playerDamage.height;

        glBindTexture(GL_TEXTURE_2D, gl.damage);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, rbW, rbH, 0,
                            GL_RGB, GL_UNSIGNED_BYTE, playerDamage.data);

        glBindTexture(GL_TEXTURE_2D, gl.damage);
        glColor3f(1.0, 1.0, 1.0);

        int widthR = 300;
        int heightR = 100;
                                
        
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f); glVertex2i(xwin/2, ywin/1.2);
            glTexCoord2f(0.0f, 0.0f); glVertex2i(xwin/2, ywin/1.2+heightR);
            glTexCoord2f(1.0f, 0.0f); glVertex2i(xwin/2+widthR, ywin/1.2+heightR);
            glTexCoord2f(1.0f, 1.0f); glVertex2i(xwin/2+widthR, ywin/1.2);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void restartGame(int* p1Health, int* p2Health, double* p1Pos, double* p2Pos,
                int* p1Dead, int* p2Dead, int* p1Weapon, int* p2Weapon,
                time_t* p1TimeLimit, time_t* p2TimeLimit, int* gRestart,
                int* nextMB, int* mBS, int xwin, bool* deathP1, bool* deathP2) 
{
    if ((*p1Dead == 1 || *p2Dead == 1) && *gRestart == 1) {    
        *p1Health = 150;
        *p2Health = 150;
        *p1Pos = (double)xwin/4;
        *p2Pos = (double)xwin/1.3;
        *p1Dead = 0;
        *p2Dead = 0;
        *p1Weapon = 0;
	*deathP1 = false;
	*deathP2 = false;
        *p2Weapon = 0;
        *p1TimeLimit = 0;
        *p2TimeLimit = 0;
        *gRestart = 0;
        *nextMB = 100;
        *mBS = 0;
    }
}                

void player1PU (int* p1PUL, double* p1Vel, double* p2Vel, 
                int* p1Health, time_t* p1TL, int* weapon, time_t p2TL) 
{
    if (*p1TL != 0) {
        if (*weapon == 2 && *p1PUL == 0) { //speed
            *p1PUL += 1;
            *p1Vel += 10.0f;
            printf("player 1 speed upgrade\n");
        } else if (*weapon == 3 && *p1PUL == 0) {//slow other player
            *p1PUL += 1;
            *p2Vel -= 8.0f;
            printf("player 2 slowed down\n");
        } else if (*weapon == 4 && *p1PUL == 0) {//add health
            *p1PUL += 1;
            *p1Health += 25;
            printf("player 1 health gain +25\n");
        } else if (*weapon == 5 && *p1PUL == 0) {//damage
            printf("player 1 damage upgrade\n");
            *p1PUL += 1;
        }
        if ((time(NULL) - *p1TL) > 15) {
            //reset player time limit to take power up away
            *p1TL = 0;
            printf("power time limit up for player 1\n");
        }
    } else if (p2TL == 0){
        //take power up away
		*weapon = 0;
		*p1PUL = 0;
		*p1Vel = 10.0f;
		*p2Vel = 10.0f;
    }
}

void player2PU (int* p2PUL, double* p1Vel, double* p2Vel, 
                int* p2Health, time_t* p2TL, int* weapon, time_t p1TL) 
{
    if (*p2TL != 0) {
		if (*weapon == 2 && *p2PUL == 0) { //speed
			*p2PUL += 1;
			*p2Vel += 10.0f;
			printf("player 2 speed upgrade\n");
		} else if (*weapon == 3 && *p2PUL == 0) {//slow other player
			*p2PUL += 1;
			*p1Vel -= 8.0f;
			printf("player 1 slowed down\n");
		} else if (*weapon == 4 && *p2PUL == 0) {//add health
			*p2PUL += 1;
			*p2Health += 25;
			printf("player 2 health gain +25\n");
		} else if (*weapon == 5 && *p2PUL == 0) {//damage
			printf("player 2 damage upgrade\n");
			*p2PUL += 1;
		}
		if ((time(NULL) - *p2TL) > 15) {
			//reset player time limit to take power up away
			*p2TL = 0;
			printf("power time limit up for player 2\n");
		}
	} else if (p1TL == 0) {
		//take power up away
		*weapon = 0;
		*p2PUL = 0;
		*p2Vel = 10.0f;
		*p1Vel = 10.0f;
	}
}

void spawnMysteryBox(int* mBS, int* nMB, time_t* p1TL,
                    int* weapon1, time_t* p2TL, int* weapon2,
                    int p1Health, int p2Health, int xwin,
                    int flag, double p1PosX, double p2PosX,
                    double p1PosY, double p2PosY) 
{
    if (p1Health <= *nMB || p2Health <= *nMB) {
        int temp = mysteryBox(xwin, *mBS);
        
        if (p1PosX >= temp-20 && p1PosX <= (temp+75)
            && (p1PosY-100) <= 150) { 
            //if player 1 within box parameters
            *mBS += 1;
            *nMB -= 25;
			*p1TL = time(NULL);
			srand (time(NULL));
            if (flag) { 
                // for jesse feature
                *weapon1 = 1;
            } else {
				//power up 2-4
				*weapon1 = (rand()%4)+2;
            }
        }
        if (p2PosX >= temp-20 && p2PosX <= (temp+75)
            && (p2PosY-100) <= 150) { 
            //if player 2 within box parameters
            *mBS += 1;
            *nMB -= 25;
			*p2TL = time(NULL);
			srand (time(NULL));
            if (flag) { 
                // for jesse feature
                *weapon2 = 1;
            } else {
				//power up 2-4
				*weapon2 = (rand()%4)+2;
            }
        }
    }
}

int flasher = 0; //variable to flash menu options in menu
void restartScreen(int player, int ywin, int xwin)
{
    /* //stuff for ggprint8b
    Rect r;
    unsigned int c = 0x00ffff44;
	r.bot = ywin - 20;
	r.left = 10;
	r.center = 0;
    */

   //player variable represents which player dies
    
    //for player menu///////////////////////////////////////////////////////////////
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &gl.menu);

    int w;
    int h;

    if (player != 1) {
        w = player1Menu.width;
        h = player1Menu.height;
    } else {
        w = player2Menu.width;
        h = player2Menu.height;
    }

    //for player menu
    glBindTexture(GL_TEXTURE_2D, gl.menu);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        if (player != 1) {
        glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
                            GL_RGB, GL_UNSIGNED_BYTE, player1Menu.data);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
                            GL_RGB, GL_UNSIGNED_BYTE, player2Menu.data);
    }

    glBindTexture(GL_TEXTURE_2D, gl.menu);
    glColor3f(1.0, 1.0, 1.0);

    int width = 500;
    int height = 500;

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2i(xwin/3, ywin/2);
        glTexCoord2f(0.0f, 0.0f); glVertex2i(xwin/3, ywin/2+height);
        glTexCoord2f(1.0f, 0.0f); glVertex2i(xwin/3+width, ywin/2+height);
        glTexCoord2f(1.0f, 1.0f); glVertex2i(xwin/3+width, ywin/2);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
    if (flasher < 20) {
        if (player != 1) {
            //for player 1 menu red//////////////////////////////////////////////////////
            glEnable(GL_TEXTURE_2D);
            glGenTextures(1, &gl.p1Red);
        
            int rbW = player1MenuRed.width;
            int rbH = player1MenuRed.height;

            glBindTexture(GL_TEXTURE_2D, gl.p1Red);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, 3, rbW, rbH, 0,
                                GL_RGB, GL_UNSIGNED_BYTE, player1MenuRed.data);

            glBindTexture(GL_TEXTURE_2D, gl.p1Red);
            glColor3f(1.0, 1.0, 1.0);

            int widthR = 500;
            int heightR = 500;
                                
        
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 1.0f); glVertex2i(xwin/3, ywin/2);
                glTexCoord2f(0.0f, 0.0f); glVertex2i(xwin/3, ywin/2+heightR);
                glTexCoord2f(1.0f, 0.0f); glVertex2i(xwin/3+widthR, ywin/2+heightR);
                glTexCoord2f(1.0f, 1.0f); glVertex2i(xwin/3+widthR, ywin/2);
            glEnd();
            glBindTexture(GL_TEXTURE_2D, 0);
        } else {
            //for player 2 menu red//////////////////////////////////////////////////////////////////
            glEnable(GL_TEXTURE_2D);
            glGenTextures(1, &gl.p2Red);
        
            int mbW = player2MenuRed.width;
            int mbH = player2MenuRed.height;

            glBindTexture(GL_TEXTURE_2D, gl.p2Red);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, 3, mbW, mbH, 0,
                                GL_RGB, GL_UNSIGNED_BYTE, player2MenuRed.data);

            glBindTexture(GL_TEXTURE_2D, gl.p2Red);
            glColor3f(1.0, 1.0, 1.0);

            int widthR = 500;
            int heightR = 500;
                                
        
            glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 1.0f); glVertex2i(xwin/3, ywin/2);
                glTexCoord2f(0.0f, 0.0f); glVertex2i(xwin/3, ywin/2+heightR);
                glTexCoord2f(1.0f, 0.0f); glVertex2i(xwin/3+widthR, ywin/2+heightR);
                glTexCoord2f(1.0f, 1.0f); glVertex2i(xwin/3+widthR, ywin/2);
            glEnd();
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    
    flasher += 1;

    if (flasher > 40) {
        printf("flasher: %i\n", flasher);
        flasher = 0;
    }

    //box for menu -> I want to put a texture over this
    /*
    glPushMatrix();
    glTranslatef(xwin/2, ywin/2, 0.0);
    glBegin(GL_QUADS);
    glColor3ub(0, 0, 0);
        glVertex2i(-xwin/4, -ywin/5);
        glVertex2i(-xwin/4, ywin/5);
        glVertex2i(xwin/4, ywin/5);
        glVertex2i(xwin/4, -ywin/5);
    glEnd();
    glPopMatrix();
    */

    //testing small box on menu
    /*
    glPushMatrix();
    glTranslatef(xwin/2, ywin/2, 0.0);
    glBegin(GL_QUADS);
    glColor3ub(100,1,1);
        glVertex2i(-100,0);
        glVertex2i(-100,100);
        glVertex2i(100,100);
        glVertex2i(100,0);
    glEnd();
    glPopMatrix();
    */

    //words in menu
    /*
    r.left += xwin/2;
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 16, c, "");
    ggprint8b(&r, 50, c, "PLAYER %i WINS", player);
    ggprint8b(&r, 50, c, "PRESS r TO RESTART");
    r.left -= xwin/2;
    */
}

