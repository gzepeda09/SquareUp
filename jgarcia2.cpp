//Jesse Garcia
//walk.cpp

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>



void greenBoxes()
{
    float offSetX = 0.0;
    float offSetY = 0.0;    

    for(int i = 0; i < 2; ++i) {
        glPushMatrix();
        glColor3f(0.0, 5.0, 0.0);
        glTranslatef(0.0f + offSetX, 0.0f, 0.0f);
        glBegin(GL_QUADS);
            glVertex2i(0, 0);
            glVertex2i(0, 600);
            glVertex2i(40, 600);
            glVertex2i(40, 0);
        glEnd();
        glPopMatrix();
        
        offSetX = 760.0;    
    }
    for(int i = 0; i < 2; ++i) {
        glPushMatrix();
        glColor3f(0.0, 5.0, 0.0);
        glTranslatef(0.0f, 0.0f + offSetY, 0.0f);
        glBegin(GL_QUADS);
            glVertex2i(0, 0);
            glVertex2i(0, 40);
            glVertex2i(800, 40);
            glVertex2i(800, 0);
        glEnd();
        glPopMatrix();

        offSetY = 560.0;
    }
}

