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


ALuint alBufferDrip;
ALuint alSourceDrip;
ALuint punchBuffer;
ALuint punchSource;






// Following code taken from snake framework
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


        alSourcef(alSourceDrip, AL_GAIN, 0.4f);
        alSourcef(alSourceDrip, AL_PITCH, 0.4f);
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


// extern void newText(int yres, int xres)
// {



    // float sxres = xres - 0.5f;

    // Box box(yres , sxres);
    // Box box2(yres + 1.0f, sxres);
    // Box box3(yres - 0.5f, sxres);


    // Box border(yres - 0.5f, sxres);

    // unsigned char c[3] = {176, 172, 172};
    // unsigned char c2[3] = {33, 156, 14};
        // unsigned char c3[3] = {184, 2, 2};


    // unsigned char bc[3] = {255, 255, 255};


    // Rect r;
    // glClear(GL_COLOR_BUFFER_BIT);


    // r.bot = yres - box3.pos[1];
    // r.left = box3.pos[0];
    // r.center = 0;





    // box.set_color(c);
    // box2.set_color(c2);
    // box3.set_color(c3);
    // border.set_color(bc);


    //    //Draw box.
    // glPushMatrix();
    // glColor3ubv(box.color);
    // glTranslatef(box.pos[0], box.pos[1], 0.0f);
    // glBegin(GL_QUADS);
    // glVertex2f(-box.w, -box.h);
    // glVertex2f(-box.w,  box.h);
    // glVertex2f( box.w,  box.h);
    // glVertex2f( box.w, -box.h);
    // glEnd();
    // glPopMatrix();




    // //Draw border.
    // glPushMatrix();
    // glColor3ubv(border.color);
    // glTranslatef(border.pos[0], border.pos[1], 0.0f);
    // glBegin(GL_QUADS);
    // glVertex2f(-border.w + 20, -border.h + 20);
    // glVertex2f(-border.w + 20,  border.h + 20);
    // glVertex2f( border.w + 20,  border.h + 20);
    // glVertex2f( border.w + 20, -border.h + 20);
    // glEnd();
    // glPopMatrix();



    // //Draw box.
    // glPushMatrix();
    // glColor3ubv(box2.color);
    // glTranslatef(box2.pos[0], box2.pos[1], 0.0f);
    // glBegin(GL_QUADS);
    // glVertex2f(-box2.w, -box2.h);
    // glVertex2f(-box2.w,  box2.h);
    // glVertex2f( box2.w,  box2.h);
    // glVertex2f( box2.w, -box2.h);
    // glEnd();
    // glPopMatrix();


    // ggprint8b(&r, 16, 0x000000, "Start");

    // //Draw box.
    // glPushMatrix();
    // glColor3ubv(box3.color);
    // glTranslatef(box3.pos[0], box3.pos[1], 0.0f);
    // glBegin(GL_QUADS);
    // glVertex2f(-box3.w, -box3.h);
    // glVertex2f(-box3.w,  box3.h);
    // glVertex2f( box3.w,  box3.h);
    // glVertex2f( box3.w, -box3.h);
    // glEnd();
    // glPopMatrix();


// }



