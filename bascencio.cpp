//Brian Ascencio
// 2/22/23
//walk.cpp

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <cstring>

/*class Global {
    public:
        int xres, yres;
        int n;
        int m;
        Global();
} g;
*/
const int NUM_BOX = 1;
const float GRAVITY = 0.05;

class Box {
    public:
        float w;
        float h;
        float pos[2];
        float vel[2];
        unsigned char color[3];
        void set_color(unsigned char col[3]) {
            memcpy(color, col, sizeof(unsigned char) * 3);
        }
        Box() {
            w = 65.f;
            h = 15.f;
            pos[0] = 8;
            pos[1] = 1.5;
            vel[0] = 0.0;
            vel[0] = 0.0;

        }
        Box(float wid, float hgt, int x, int y, float v0, float v1) {
            w = wid;
            h = hgt;
            pos[0] = x;
            pos[1] = y;
            vel[0] = v0;
            vel[0] = v1;

        }

} box[NUM_BOX];

int move_box(XEvent *e)
{
    //keyboard input?
    static int shift=0;
    if (e->type != KeyRelease && e->type != KeyPress)
        return 0;
    int key = (XLookupKeysym(&e->xkey, 0) & 0x0000ffff);
    if (e->type == KeyRelease) {
        if (key == XK_Shift_L || key == XK_Shift_R)
            shift = 0;
        return 0;
    }
    if (key == XK_Shift_L || key == XK_Shift_R) {
        shift=1;
        return 0;
    }
    (void)shift;
    switch (key) {
        case XK_w:
            // move player up
            box[0].vel[1] += 0.5;
            break;
        case XK_a:
            // move player left
            box[0].vel[0] -= 0.5;
            break;
        case XK_s:
            // move player down
            box[0].vel[1] -= 0.5;
            break;
        case XK_d:
            // move player right
            box[0].vel[0] += 0.5;
            break;
        case XK_g:
            break;
        case XK_h:
            break;
        case XK_j:
            return 1;
            break;
    }
    return 0;

}
void make_box()
{
    glClear(GL_COLOR_BUFFER_BIT);

    //braw a box 
        glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(box[0].pos[0], box[0].pos[1], 0.0);
    glBegin(GL_QUADS);
    glVertex2f(-box[0].w, -box[0].h);
    glVertex2f(-box[0].w,  box[0].h);
    glVertex2f( box[0].w,  box[0].h);
    glVertex2f( box[0].w, -box[0].h);
    glEnd();
    glPopMatrix();
}
