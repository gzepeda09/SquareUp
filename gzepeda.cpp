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
#include "image.h"


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
extern void powBar(float w, float h, unsigned char color[3], float pos0, float pos1);
extern void strMenu(int yres, int xres);
extern void superPunch(int w, int w2, int h, float x, float y, int flipped, int player);
extern void cntrlMenu(int yres, int xres);
extern void chrgPhys(int *pBar, bool *chrg);
extern void rplatPhys(float ph, float rh, float rw, float rPos0, float rPos1, float pos0, 
						  double *pos1,
						  bool *onPlt, bool *tp, int *activate);
extern void rplatPhys2(double *pos1, double *vel, bool *onPlt, 
	                   bool *tp, bool *stp, int *activate, int yres);
extern void supPunchPhys1(bool *btnPress, int *pB, int *sPnch);
extern void supPunchPhys2(bool *btnPress, int *sPnch);


ALuint alBufferDrip;
ALuint alSourceDrip;
ALuint punchBuffer;
ALuint punchSource;


GLuint menuBgrnd; //my strMenu bacground
GLuint cntrlBg;
GLuint pltText; //my platform textures


Image sMenuBg("images/strtBackground.jpg");
Image cMenuBG("images/controls.jpg");
Image pltBG("images/plt.jpg");

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

extern void rForms(float w, float h, unsigned char color[3], float pos0, float pos1)
{


    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &pltText);


    glBindTexture(GL_TEXTURE_2D, pltText);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, pltBG.width, pltBG.height, 0,
				GL_RGB, GL_UNSIGNED_BYTE, pltBG.data);
    glPushMatrix();
    glColor3ubv(color);
    glTranslatef(pos0, pos1, 0.0f); // move the platform
    glBindTexture(GL_TEXTURE_2D, pltText);
    glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-w, -h);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-w, h);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(w, h);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(w, -h);
    glEnd();
    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);




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



extern void strMenu(int yres, int xres)
{

    	//for player menu


	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &menuBgrnd);

	glBindTexture(GL_TEXTURE_2D, menuBgrnd);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, sMenuBg.width, sMenuBg.height, 0,
				GL_RGB, GL_UNSIGNED_BYTE, sMenuBg.data);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2i(0, yres);
		glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
		glTexCoord2f(1.0f, 1.0f); glVertex2i(xres, 0);
		glTexCoord2f(1.0f, 0.0f); glVertex2i(xres, yres);
        glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);




}
extern void powBar(float w, float h, unsigned char color[3], float pos0, float pos1)
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
extern void superPunch(int w, int w2, int h, float x, float y, int flipped, int player)
{
	if(player == 1){
	    if (flipped == 1) { 

			glPushMatrix();
			glTranslatef(x + 30.0f, y, 0.0f);
			glBegin(GL_QUADS);
	        glColor3ub(255, 215, 0);
		        glVertex2f( w2,  -h);
		        glVertex2f( w2,   h);
		        glVertex2f( -w,   h);
		        glVertex2f( -w,  -h);
			glEnd();
			glPopMatrix();
	    
	    } else { 
	    	//reverse the super punch
	        glPushMatrix();
	        glTranslatef(x - 30.0f, y, 0.0f);
	        glBegin(GL_QUADS);
	        glColor3ub(255, 215, 0);
				glVertex2f(w2, h);
				glVertex2f(w2, -h);
				glVertex2f(-w, -h);
				glVertex2f(-w, h);
			glEnd();
	        glPopMatrix();
	    }
	} else if(player == 2) {
	    if (flipped == 1) { 

			glPushMatrix();
			glTranslatef(x - 115.0f, y , 0.0f);
			glBegin(GL_QUADS);
	        glColor3ub(255, 215, 0);
				glVertex2f( w2,  -h);
		        glVertex2f( w2,   h);
		        glVertex2f( -w,   h);
		        glVertex2f( -w,  -h);
			glEnd();
			glPopMatrix();
	    
	    } else { 
	    	//reverse the super punch
	        glPushMatrix();
	        glTranslatef(x + 115.0f, y, 0.0f);
	        glBegin(GL_QUADS);
	        glColor3ub(255, 215, 0);
		        glVertex2f(w2, h);
				glVertex2f(w2, -h);
				glVertex2f(-w, -h);
				glVertex2f(-w, h);
			glEnd();
	        glPopMatrix();
	    }
	}
}

extern void cntrlMenu(int yres, int xres)
{

    	//for control menu

       glEnable(GL_TEXTURE_2D);
       glGenTextures(1, &cntrlBg);

       glBindTexture(GL_TEXTURE_2D, cntrlBg);
       glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
       glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
       glTexImage2D(GL_TEXTURE_2D, 0, 3, cMenuBG.width, cMenuBG.height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, cMenuBG.data);
       glBegin(GL_QUADS);
           glTexCoord2f(0.0f, 0.0f); glVertex2i(0, yres);
           glTexCoord2f(0.0f, 1.0f); glVertex2i(0, 0);
           glTexCoord2f(1.0f, 1.0f); glVertex2i(xres, 0);
           glTexCoord2f(1.0f, 0.0f); glVertex2i(xres, yres);
       glEnd();
       glBindTexture(GL_TEXTURE_2D, 0);
}

extern void chrgPhys(int *pBar, bool *chrg)
{
	(*pBar)++;

	std::cout << "Charging up" << std::endl;
	(*chrg) = true;

}




extern void rplatPhys(float ph, float rh, float rw, float rPos0, float rPos1, float pos0, 
						  double *pos1, 
						  bool *onPlt, bool *tp, int *activate)
{
		
	if (pos0 >= rPos0 - rw &&
			pos0 <= rPos0 + rw &&
			(*pos1) - 95.5f >= rPos1 - rh &&
			(*pos1) - 95.5f <= rPos1 + rh){
		// The player is on this platform, so set their vertical position
		// to the top of the platform
		if(!tp){
			(*pos1) = rPos1 + rh/2 + 
				  ph/2;
		}
		
		(*onPlt) = true;
		(*activate) = 1;
	}

}

extern void rplatPhys2(double *pos1, double *vel, bool *onPlt, 
	                   bool *tp, bool *stp, int *activate, int yres)
{

	double gRav = 0.0000001;


	if ((*onPlt)) {
	// Player is on a platform, so their vertical velocity should be zero
	(*vel) = 0.0f;
	(*tp) = true;


	} else {

		if((*activate) == 1){
			// Player is not on a platform, so make them fall
			(*vel) -= gRav;
			// Check if player has reached the bottom of the screen
			if ((*pos1) <= 0.0f) {
				// Player has fallen off the bottom of the screen, so reset their position
				if(!(*stp)){
					(*pos1) = yres/2 - 440.0f;
					(*vel) = 0.0;
					(*stp) = true;
					(*activate) = 0;
				}
			}

			(*stp) = false;


		}
	}

}
extern void supPunchPhys1(bool *btnPress, int *pB, int *sPnch)
{
        if (!(*btnPress)) { // Check if the key is not already held down
	        (*btnPress) = true; // Set the flag to indicate that the key is held down
	        if ((*pB) >= 150) {
	            (*pB) -= 150;

	       		 (*sPnch) = 1;  	
	        }

	    }
}



extern void supPunchPhys2(bool *btnPress, int *sPnch)
{
      (*btnPress) = false;
      (*sPnch) = 0;
}
