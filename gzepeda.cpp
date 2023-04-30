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

	r.bot = pos1 - 50.0f;
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
