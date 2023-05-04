/*
NAME: Geno Zepeda
FILE: gzepeda.cpp
ORGN: CSUB - CMPS 3350

Purpose: Draws text 


*/
#include "fonts.h"
#include "box.h"
#include <GL/glx.h>
#include <GL/glut.h>
#include <cstring>
// #include <AL/al.h>
// #include <AL/alc.h>





#define USE_OPENAL_SOUND
#ifdef USE_OPENAL_SOUND
// #include </usr/include/AL/alut.h>
#include <AL/alut.h>
#endif //USE_OPENAL_SOUND
#include <vector>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


using namespace std;











#ifdef USE_OPENAL_SOUND
extern void initSound();
extern void cleanupSound();
extern void playSound();
extern void playPunchSound();
#endif //USE_OPENAL_SOUND



extern void rForms(float w, float h, unsigned char color[3], float pos0, 
				   float pos1);
extern void health(float w, float h, unsigned char color[3], float pos0, 
				   float pos1, int player, int health);
extern void powBar(float w, float h, unsigned char color[3], float pos0, float pos1, int bar);
extern void strMenu(int yres, int xres);
extern void superPunch(int w, int w2, int h, float x, float y, int flipped);

// extern void pltPhysics(double plPos0, double plPos1, double plVel,
//                       float rPos0, float rPos1, float rW, float rH);  


ALuint alBufferDrip;
ALuint alSourceDrip;
ALuint punchBuffer;
ALuint punchSource;






// Following code taken from snake framework 
// with modifications for our game
extern void initSound()
{
	#ifdef USE_OPENAL_SOUND
	alutInit(0, NULL);
	if (alGetError() != AL_NO_ERROR) {
		printf("ERROR: alutInit()\n");
		return;
	}
	//Clear error state.
	alGetError();
	//
	//Setup the listener.
	//Forward and up vectors are used.
	float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, vec);
	alListenerf(AL_GAIN, 1.0f);
	//
	//Buffer holds the sound information.
	alBufferDrip = alutCreateBufferFromFile("./Audio/music.wav");
	punchBuffer = alutCreateBufferFromFile("./Audio/grunt.wav");
	//
	//Source refers to the sound.
	//Generate a source, and store it in a buffer.
	alGenSources(1, &alSourceDrip);
	alSourcei(alSourceDrip, AL_BUFFER, alBufferDrip);


	alSourcef(alSourceDrip, AL_GAIN, 1.0f);
	alSourcef(alSourceDrip, AL_PITCH, 1.0f);
	alSourcei(alSourceDrip, AL_LOOPING, AL_TRUE);
	if (alGetError() != AL_NO_ERROR) {
		printf("ERROR: setting source\n");
		return;
	}


	//Generate a source, and store it in a buffer.
	alGenSources(1, &punchSource);
	alSourcei(punchSource, AL_BUFFER, punchBuffer);
	//Set volume and pitch to normal, no looping of sound.
	alSourcef(punchSource, AL_GAIN, 1.0f);
	alSourcef(punchSource, AL_PITCH, 1.0f);
	alSourcei(punchSource, AL_LOOPING, AL_FALSE);
	if (alGetError() != AL_NO_ERROR) {
		printf("ERROR: setting source\n");
		return;
	}

	#endif //USE_OPENAL_SOUND
}

extern void cleanupSound()
{
	#ifdef USE_OPENAL_SOUND
	//First delete the source.
	alDeleteSources(1, &alSourceDrip);
	//Delete the buffer.
	alDeleteBuffers(1, &alBufferDrip);
	alDeleteSources(1, &punchSource);
	//Delete the buffer.
	alDeleteBuffers(1, &punchBuffer);
	//Close out OpenAL itself.
	//Get active context.
	ALCcontext *Context = alcGetCurrentContext();
	//Get device for active context.
	ALCdevice *Device = alcGetContextsDevice(Context);
	//Disable context.
	alcMakeContextCurrent(NULL);
	//Release context(s).
	alcDestroyContext(Context);
	//Close device.
	alcCloseDevice(Device);
	#endif //USE_OPENAL_SOUND
}

extern void playSound()
{
	#ifdef USE_OPENAL_SOUND
	alSourcePlay(alSourceDrip);
	#endif //USE_OPENAL_SOUND
}


extern void playPunchSound()
{
	#ifdef USE_OPENAL_SOUND
	alSourcePlay(punchSource);
	#endif //USE_OPENAL_SOUND
}


// End of code taken from snake framework


// extern void newText(int yres, int xres){



// }


extern void rForms(float w, float h, unsigned char color[3], float pos0, float pos1)
{


	glPushMatrix();
	glColor3ubv(color);
	glTranslatef(pos0, pos1, 0.0f);
	glBegin(GL_QUADS);
	    glVertex2f(-w, -h);
	    glVertex2f(-w,  h);
	    glVertex2f( w,  h);
	    glVertex2f( w, -h);
	glEnd();
	glPopMatrix();
}


extern void health(float w, float h, unsigned char color[3], float pos0, float pos1, int player, int health)
{

	Rect r;

	unsigned int c = 0x000000;

	r.bot = pos1 - 65.0f;
    r.left = pos0;
    r.center = 10;
    // ggprint8b(&r, 16, c, "Player 1 Health: %i", player1.health);
    ggprint8b(&r, 16, c, "Player %i", player);



    glPushMatrix();
    glColor3ubv(color);
    glTranslatef(pos0, pos1, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-w, -h);
        glVertex2f(-w,  h);
        glVertex2f( w,  h);
        glVertex2f( w, -h);
    glEnd();
    glPopMatrix();



    r.bot = pos1 - 8.0f;
    r.left = pos0;
    r.center = 20;

    ggprint8b(&r, 16, c, "%i", health);
}



// extern void pltPhysics(double plPos0, double plPos1, double plVel,
//                       float rPos0, float rPos1, float rW, float rH, int yres)
// {
// 	bool onPlat = false;

//     double gRav = 0.0000001;

//         if (plPos0 >= rPos0 - rW &&
//             plPos0 <= rPos0 + rW &&
//             plPos1 >= rPos1 - rH &&
//             plPos1 <= rPos1 + rH) {
//             // The player is on this platform, so set their vertical position
//             // to the top of the platform
//             if(!top){
//                 plPos1 = (rPos1 * 2) + rH;
//                 top = true;
//             }
//             onPlat = true;
//             activate = 1;
//         }

        


//         if (onPlat) {
//             // Player is on a platform, so their vertical velocity should be zero
//             plVel = 0.0f;
//             plPos1 = (rPos1 * 2) + rH;
//         } else {

//             if(activate == 1){
//                 // Player is not on a platform, so make them fall
//                 plVel -= gRav;
//                 // Check if player has reached the bottom of the screen
//                 if (plPos1 <= 0.0f) {
//                     // Player has fallen off the bottom of the screen, so reset their position
//                     if(!stop){
//                         plPos1 = yres/2 - 440.5f;
//                         plVel = 0.0;
//                         stop = true;
//                         activate = 0;
//                     }
//                 }

//                 stop = false;
//             }
//         }
    




// }



extern void strMenu(int yres, int xres)
{

    Rect r;


    unsigned int d = 0xffffff;


    Box sMenu[3];

    int y = 0;


    for (int i = 0; i < 3; i++){
        sMenu[i].set_width(100.0f);
        sMenu[i].set_height(25.0f);
        sMenu[i].set_xres(xres);
        sMenu[i].set_yres(yres - y);

        y += 200;
    }

    r.bot = yres - sMenu[0].pos[1];
    r.left = sMenu[0].pos[0] - 10.0f;
    r.center = 0;



    unsigned char c[3] = {0, 128, 0};
    unsigned char c2[3] = {0, 128, 0};
    unsigned char c3[3] = {184, 2, 2};



    sMenu[0].set_color(c2);
    sMenu[1].set_color(c);
    sMenu[2].set_color(c3);


    unsigned char bc[3] = {255, 255, 255};



     

    for (int i = 0; i < 3; ++i){

        glPushMatrix();
        glColor3ubv(sMenu[i].color);
        glTranslatef(sMenu[i].pos[0], sMenu[i].pos[1], 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(-sMenu[i].w, -sMenu[i].h);
            glVertex2f(-sMenu[i].w,  sMenu[i].h);
            glVertex2f( sMenu[i].w,  sMenu[i].h);
            glVertex2f( sMenu[i].w, -sMenu[i].h);
        glEnd();
        glPopMatrix();



    }

    ggprint8b(&r, 16, d, "(F1)-Start");


    r.bot = yres - sMenu[1].pos[1] - 300.0f;
    r.left = sMenu[1].pos[0] - 15.5f;
    r.center = 0;

    ggprint8b(&r, 16, d, "(F2)-Controls");


    r.bot = yres - sMenu[2].pos[1] - 300.0f;
    r.left = sMenu[2].pos[0] - 10.0f;
    r.center = 0;

    ggprint8b(&r, 16, d, "(F3)-Quit");


}
extern void powBar(float w, float h, unsigned char color[3], float pos0, float pos1, int bar)
{
	Rect r;
	unsigned int d = 0x000000;


    r.bot = pos1 - 8.0f;
    r.left = pos0;
    r.center = 20;



   //draw power bars


    glPushMatrix();
    glColor3ubv(color);
    glTranslatef(pos0, pos1, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-w, -h);
        glVertex2f(-w,  h);
        glVertex2f( w,  h);
        glVertex2f( w, -h);
    glEnd();
    glPopMatrix();



    ggprint8b(&r, 16, d, "ENERGY");
}
extern void superPunch(int w, int w2, int h, float x, float y, int flipped)
{
    if (flipped == 1) { 

		glPushMatrix();
		glTranslatef(x , y, 0.0f);
		glBegin(GL_QUADS);
		//glColor3ub(0, 0, 0);
        glColor3ub(255, 0, 0);
	        glVertex2f( w2,  -h);
	        glVertex2f( w2,   h);
	        glVertex2f( -w,   h);
	        glVertex2f( -w,  -h);
		glEnd();
		glPopMatrix();
    
    } else { 

        //after elbow
        glPushMatrix();
        glTranslatef(x, y+20.0f, 0.0f);
        glBegin(GL_QUADS);
        glColor3ub(0, 150, 0);
	        glVertex2f( -w2,  -h);
	        glVertex2f( -w2,   h);
	        glVertex2f(  w,   h);
	        glVertex2f(  w,  -h);
		glEnd();
        glPopMatrix();
    }
}